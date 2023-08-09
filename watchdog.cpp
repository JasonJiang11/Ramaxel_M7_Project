#include "stdafx.h"
#include "tester.h"
#include "watchdog.h"

using namespace std;

CSTRING_VARIABLE(WaferID, "", "Wafer ID"){}
CSTRING_VARIABLE(LotNo, "", "Lot Number"){}

void watchdog_host_worker_thread();

// Wait for all sites to load before starting the watchdog
VOID_VARIABLE(ui_ProgLoaded, "") {
	if (OnHost()) {
		apps::WatchDog::WatchDog::getInstance().configure();
		output("<h4><b>WatchDog Has Been Started</b></h4>");
	}
}

BOOL_VARIABLE(watchdog_enable, false, "watchdog enable")
{
	if (watchdog_enable) {
		output("<h3><color=blue>The watchdog has been enabled</color></h3>");
	}
	else {
		output("<h3><color=red>The watchdog has been disabled</color></h3>");
	}
}

BOOL_VARIABLE(watchdog_bark_enable, true, "watchdog enable")
{
	if (watchdog_enable) {
		if (watchdog_bark_enable) {
			output("<h3><color=blue>The watchdog bark has been enabled</color></h3>");
		}
		else {
			output("<h3><color=red>The watchdog bark has been disabled</color></h3>");
		}
	}
	else {
		output("<h3><color=red>Please enable watchdog to enable watchdog bark</color></h3>");
		watchdog_bark_enable = false;
	}
}

//// Getter to our global Watchdog
//apps::watchdog::WatchDog& get_watchdog()
//{
//	static apps::watchdog::WatchDog ret;
//	return ret;
//}

CSTRING_VARIABLE(fetch_wd_state, "", "")
{
	apps::WatchDog::WatchDogProgramState wd_copy;
	apps::WatchDog::WatchDog::getInstance().get_wd_state(wd_copy);
	ULONGLONG curtick = GetTickCount64();

	CStringArchive(&fetch_wd_state) <<
		wd_copy.flow_running <<
		wd_copy.check_test_time <<
		CString(wd_copy.current_test.c_str()) <<
		(curtick - wd_copy.test_start_timestamp) <<
		(curtick - wd_copy.flow_start_timestamp) <<
		(curtick - wd_copy.tb_extend_timestamp);
}

CSTRING_VARIABLE(fetch_site_id_string, "Execute Fetch", "This is for testing")
{
	CString slot_id_identifier;
	if (OnSite()) {
		int chassis, slot, site;
		slot_id(&chassis, &slot, &site);
		slot_id_identifier.Format("%d_%d_%d", chassis, slot, site);
	}
	else if (OnHost()) {
		slot_id_identifier = "host";
	}
	else {
		slot_id_identifier = "error";
	}

	DWORD pid = GetCurrentProcessId();

	CStringArchive(&fetch_site_id_string) << slot_id_identifier << pid;
}

// Initially tried to combine the following 3 initialization hooks into 1 hook.
INITIALIZATION_HOOK(ih_Watchdog_TB)
{
	output("<color=blue>Registering Debug Hook for before and after the testblocks</color>");

	// Install a debug hook
	struct local
	{
		static void tb_debug_hook(BOOL& after, int& result) {
			// If we're just about to enter a TestBlock
			static apps::WatchDog::WatchDog& wd = apps::WatchDog::WatchDog::getInstance();
			if (!after) {
				wd.test_start(current_test_block());
			}
			else {
				wd.test_end();
			}
		}
	};

	install_debug_hook(local::tb_debug_hook, dhp_before | dhp_after, dhp_test_block);
}

INITIALIZATION_HOOK(ih_Watchdog_BTB)
{
	output("<color=blue>Registering Debug Hook for for the before_testing_block</color>");

	// Install a debug hook
	struct local
	{
		static void btb_debug_hook(BOOL& after, int& result) {
			static apps::WatchDog::WatchDog& wd = apps::WatchDog::WatchDog::getInstance();
			wd.flow_start();
		}
	};

	install_debug_hook(local::btb_debug_hook, dhp_after, dhp_before_testing_block);
}

INITIALIZATION_HOOK(Watchdog_ATB)
{
	output("<color=blue>Registering Debug Hook for for the after_testing_block</color>");

	// Install a debug hook
	struct local
	{
		static void atb_debug_hook(BOOL & after, int& result) {
			static apps::WatchDog::WatchDog& wd = apps::WatchDog::WatchDog::getInstance();
			wd.flow_end();
		}
	};

	install_debug_hook(local::atb_debug_hook, dhp_before, dhp_after_testing_block);
}

using namespace apps::WatchDog;

void WatchDog::configure(double AbsoluteMaxTestTimeout, double TestTimeout, 
	double TbExtensionCallbackTimeout, double RemoteSetTimeout)
{
	if (!configured) {
		if(TRUE == watchdog_enable && TRUE == atoi(remote_get("ui_EngineeringMode", -1))) {
			remote_set("ui_ShowOutputTab", 0, -1, TRUE, INFINITE);  // Switch UI to show the host tab
			output("<h4><b><color=red>\nUI is running in Engineering Mode!\n</color></h4>");
			output("Debugging while the watchdog is enabled could cause it to \"bark\" erroneously.");
			output("Set user variable, <i>watchdog_enable</i>, to FALSE will avoid this\n</b>");
		}
		output("<h4><b>WatchDog Has Been Configured</b></h4>");
		absolute_max_test_timeout = AbsoluteMaxTestTimeout;
		test_timeout = TestTimeout;
		tb_extend_timeout = TbExtensionCallbackTimeout;
		remote_set_timeout = RemoteSetTimeout;
		remote_set_retry_rate_ms = 500;
		//	remote_set_retry_rate_ms = 1000;  //debug only
		rest_time_ms = 1000; //1 seconds between poll of the sites
		wd_state.flow_running = false;
		wd_state.current_test = "";
		exit_request = false;
		polling_active = true;
		configured = true;
		run_in_worker_thread(watchdog_host_worker_thread);
	}
	else {
		warning("WatchDog::configure() may only be called once. This call will be ignored.");
	}
}

void WatchDog::flow_start()
{
	//exclusion list lives in the class not the ProgramState so it doesn't need to be within the lock guard
	wd_exclusions.clear();
	std::lock_guard<std::mutex> lock(wd_state_lock);
	wd_state.flow_start_timestamp = wd_state.test_start_timestamp = wd_state.tb_extend_timestamp = GetTickCount64();
	wd_state.flow_running = true;
	wd_state.current_test = "";
}

void WatchDog::flow_end()
{
	std::lock_guard<std::mutex> lock(wd_state_lock);
	wd_state.flow_running = false;
	wd_state.current_test = "";
}

void WatchDog::test_start(CString test_name)
{
	std::string _test_name(test_name.GetBuffer());
	test_start(_test_name);
}

void WatchDog::test_start(std::string test_name)
{
	//exclusion list lives in the class not the ProgramState so it doesn't need to be within the lock guard
	//look up test_name to see if it should be excluded
	auto it = wd_exclusions.find(test_name);

	//Access wd_state
	std::lock_guard<std::mutex> lock(wd_state_lock);
	wd_state.test_start_timestamp = wd_state.tb_extend_timestamp = GetTickCount64();
	wd_state.flow_running = true;
	if (it == wd_exclusions.end())
	{
		//We do NOT have a listed exclusion
		wd_state.check_test_time = true;
		wd_state.current_test = test_name;
	}
	else {
		//Exclude this test from being watched
		wd_state.check_test_time = false;
		wd_state.current_test = test_name + " (disabled)";
	}
	return;
}

void WatchDog::test_end()
{
	std::lock_guard<std::mutex> lock(wd_state_lock);
	wd_state.test_start_timestamp = GetTickCount64();
	wd_state.check_test_time = false;
	wd_state.current_test += " (stopped)";
}

//This method should be called WITHIN the test block if that test determines that it
//is doing something that will take a long time. Currently unused.
void WatchDog::disable_for_this_test()
{
	std::lock_guard<std::mutex> lock(wd_state_lock);
	wd_state.current_test += " (disabled)";
	wd_state.check_test_time = false;
}

void WatchDog::add_exclusion(std::string exclusion)
{
	wd_exclusions.insert(std::pair<std::string, bool>(exclusion, true));
	return;
}

void WatchDog::extend_timeout()
{
	std::lock_guard<std::mutex> lock(wd_state_lock);
	wd_state.tb_extend_timestamp = GetTickCount64();
}

void WatchDog::get_wd_state(WatchDogProgramState &wd_state_copy)
{
	std::lock_guard<std::mutex> lock(wd_state_lock);
	wd_state_copy = wd_state;
}

/*
void WatchDog::deserialize(CString serialized, WatchDogProgramState& wdps)
{
	CString current_test;
	CStringArchive(&serialized) 
		>> current_test 
		>> wdps.flow_running 
		>> wdps.flow_start_timestamp.QuadPart
		>> wdps.tb_extend_timestamp.QuadPart 
		>> wdps.test_start_timestamp.QuadPart;
	wdps.current_test = current_test;
}
*/

//This function assumes that it is only run AFTER ui_ProgramLoaded gets
//executed. 
void watchdog_interrupter()
{
	//output("Thread interrupter has been called!!!!");
	apps::WatchDog::WatchDog::getInstance().exit_request = true;
}

void watchdog_host_worker_thread()
{
	//This function should only be executed on the host
	thread_interrupter(watchdog_interrupter);
	WatchDog& wd = WatchDog::getInstance();
	if (OnSite()) {
		wd.bark("none","The watchdog_host_worker_thread should only be run on the host");
	}

	SiteMask loaded_sitemask = string_to_sitemask(remote_get("ui_LoadedMask", -1));
	ValueProxies vp_site_id_strings;
	FOR_EACH_SITE(loaded_sitemask, site) {
		vp_site_id_strings[site] = CString("none");
	}
	int retries = 0;

	//Ask each site for chassis_slot_site and PID
	while (int busy = remote_set(fetch_site_id_string, vp_site_id_strings, TRUE, wd.remote_set_retry_rate_ms, TRUE))
	{
		// Retry asking for this data until we hit timeout
		retries++;
		if ((retries * wd.remote_set_retry_rate_ms * (1 MS)) > wd.remote_set_timeout) {
			std::string barkmsg, barkids;
			barkmsg = "Communications Timeout (5 sec) waiting for site to respond to fetch of site IDs." + std::to_string(busy);
			barkids = "SiteNumbers =";
			FOR_EACH_SITE(loaded_sitemask, site) {
				if (!vp_site_id_strings[site]) { barkids += " " + std::to_string(site); }
			}
			wd.bark(barkids,barkmsg);
			return;
		}
	}

	//This is the main Watchdog retry loop. We should be done initializing at this point.
	do {
		if (watchdog_enable && wd.polling_active) {
			//output("Start a Watchdog poll loop.");  //Debug message letting user know that watchdog is alive.
			ValueProxies vp;
			SiteMask current_sitemask = string_to_sitemask(remote_get("ui_LoadedMask", -1));

			//Check to see if we have lost any sites
			if (current_sitemask != loaded_sitemask) {
				SiteMask missing_sitemask = loaded_sitemask ^ current_sitemask;
				std::string barkids;
				FOR_EACH_SITE(missing_sitemask, missing_site)
				{
					CString cur_site_id_info = vp_site_id_strings[missing_site];
					CString site_id; DWORD pid;
					CStringArchive(cur_site_id_info) >> site_id >> pid;
					barkids += " " + std::string(site_id) + "(" + std::to_string(pid) + ")";
				}
				loaded_sitemask = current_sitemask;
				wd.bark(barkids, "Site unloaded unexpectantly.");
			}
			//Add a default string to the Value Proxy for each site that is active
			FOR_EACH_SITE(loaded_sitemask, site) { vp[site] = CString("Request"); }

			//Set to all loaded sites in parallel. This will trigger the body of the
			//fetch_wd_state to execute. We should receive a CString reply from each
			//site for processing
			//Wait for a reply for 100ms. Wait up to 50 times
			//Get watchdog information from each site
			ULONGLONG timeouttime = GetTickCount64() + ULONGLONG(wd.remote_set_timeout / (1 MS));
			bool remote_set_successful = true;
			if (wd.polling_active == false) continue;
			while (int busy = remote_set(fetch_wd_state, vp, TRUE, wd.remote_set_retry_rate_ms, TRUE))
			{
				//Check for communications errors
				if (GetTickCount64() > timeouttime)
				{
					std::string barkmsg;
					std::string barkids;
					barkmsg = "Communications Timeout (5 sec) waiting for site to respond to fetch of job state information." + std::to_string(busy) + " sites:";
					barkids = "Site IDs =";
					FOR_EACH_SITE(loaded_sitemask, site) {
						CString cur_site_id_info = vp_site_id_strings[site];
						CString site_id; DWORD pid;
						CStringArchive(cur_site_id_info) >> site_id >> pid;
						if (!vp[site]) { barkids += " " + std::string(site_id) + "(" + std::to_string(pid) + ")"; }
					}
					wd.bark(barkids, barkmsg);
					remote_set_successful = false;
					break;
				}
				if (wd.polling_active == false) {
					remote_set_successful = false;
					break;
				}

			}

			if (remote_set_successful == false) {
				continue;
			}

			//At this point, vp[site] should contain all the CStringArchives of the watchdog data
			//We need to process the data returned to see if there are any additional WatchDog errors.
			FOR_EACH_SITE(loaded_sitemask, site2) {
				CString cur_site_job_info;
				cur_site_job_info = vp[site2];
				CString current_test;
				bool flow_running;
				bool check_test_time;
				ULONGLONG test_start_delta_ms, flow_start_delta_ms, tb_extend_last_delta_ms;
				try {
					//Decode data sent back from the sites
					CStringArchive(cur_site_job_info) >>
						flow_running >>
						check_test_time >>
						current_test >>
						test_start_delta_ms >>
						flow_start_delta_ms >>
						tb_extend_last_delta_ms;
				}
				catch (CArchiveException* e) {
					//If data was bad, catch the exception and bark
					CString cur_site_id_info = vp_site_id_strings[site2];
					CString site_id; DWORD pid;
					CStringArchive(cur_site_id_info) >> site_id >> pid;
					wd.bark(std::string(site_id) + "(" + std::to_string(pid) + ")",
						"Error processing response for site. ");
					e->Delete();
				}

				//flow_running needs to depend on each site so this is data that was returned from the site
				if (flow_running && check_test_time) {
					//Check to make sure that the site hasn't exceeded the test time or the time since last FLS time
					if ((((double)test_start_delta_ms * (1 MS)) > wd.test_timeout) &&
						(((double)tb_extend_last_delta_ms * (1 MS)) > wd.tb_extend_timeout)) {
						std::string barkids;
						CString cur_site_id_info = vp_site_id_strings[site2];
						CString site_id; DWORD pid;
						CStringArchive(cur_site_id_info) >> site_id >> pid;
						barkids += " " + std::string(site_id) + "(" + std::to_string(pid) + ")";
						std::string errmsg = "Test " + std::string(current_test) +
							" has timed out. " + std::to_string(test_start_delta_ms) + "ms since test was started and " +
							std::to_string(tb_extend_last_delta_ms) + "ms since the last TB Extension message was received.";
						wd.bark(barkids, errmsg);
					}

					//Check to see if the test has exceeded the ABSOLUTE maximum time for any test
					if (((double)test_start_delta_ms * (1 MS)) > wd.absolute_max_test_timeout) {
						std::string barkids;
						CString cur_site_id_info = vp_site_id_strings[site2];
						CString site_id; DWORD pid;
						CStringArchive(cur_site_id_info) >> site_id >> pid;
						barkids += " " + std::string(site_id) + "(" + std::to_string(pid) + ")";
						std::string errmsg = "Test " + std::string(current_test) +
							" has timed out on the ABSOLUTE time maximimum (test stuck looping?). " + std::to_string(test_start_delta_ms) + "ms since test was started";
						wd.bark(barkids, errmsg);
					}
				}
			} //FOR_EACH_SITE

			ULONGLONG startingtick = GetTickCount64();
			//output("Watchdog resting for a sec, curtick=%ul.",startingtick);
			while ((!wd.exit_request) && (GetTickCount64() < (startingtick + wd.rest_time_ms))) {
				//Wait for a total of wd.rest_time_ms, but check every 100ms to see if we have been
				//requested to shut down the thread
				Sleep(100);
			}
			//output("Watchdog done resting. total time = %ul.", GetTickCount64() - startingtick);
		}
		else {
			Sleep(1000);  //GWB Increased speed of checking whether watchdog is enabled in case the user is
							// is using enable_polling programmatically
		}
	} while (!wd.exit_request);
	output("<h4><b>Thread was asked to shutdown.</b></h4>");
	return;
}

void write_watchdog_log(std::string errmsg)
{
	CreateDirectory(MGN_WatchDog_LOG_DIR.c_str(), NULL);

	FILE *fp = NULL;
	time_t timep;
	char Logname[256] = { 0 };

	time(&timep);

	strftime(Logname, sizeof(Logname), "%Y_%m_%d.log", localtime(&timep));//Don't change file name, because delete files function check file name

	stringstream path;
	path << MGN_WatchDog_LOG_DIR << "\\" << Logname;
	ofstream fout(path.str(), ios::app);

	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S  -->  ", localtime(&timep));

	fout << (string)tmp << errmsg << endl;
	fout.close();	
}

void WatchDog::bark(std::string site_id, std::string message)
{
  if (watchdog_bark_enable) {
    std::string lotnumber = LotNo;
    std::string waferid = WaferID;
    std::string errmsg = "WatchDog| ERROR: " + message + " site(pid): " + site_id + " Lot number:" + lotnumber + " Wafer_ID: " + waferid;
    //std::string errmsg = "WatchDog: ERROR: " + message + " site(pid): " + site_id;
    remote_set("ui_ShowOutputTab", 0, -1, TRUE, INFINITE);  //This automatically switches UI to show the host tab
    output("<color=red><b><h2>\n***WATCHDOG ERROR DETECTED***\n</h2></b>");
    output("\n%s</color>\n", errmsg.c_str());

	write_watchdog_log(errmsg);
	//remote_set("JOB_ERROR",errmsg.c_str(),0,TRUE,INFINITE);

	//only bark once unless reset
	set_value("watchdog_bark_enable", false);
  }
}

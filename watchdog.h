#pragma once
#include "tester.h"
#include <string>
#include <fstream>
#include <iostream>
#include <time.h>
#include <mutex>
#include <atomic>
#include <unordered_map>

// This switch determines if the watchdog debug configuration will be available
// The watchdog will run whether the switch is defined or undefined
#undef WD_DEBUG

const std::string MGN_WatchDog_LOG_DIR = "C:\\nextest\\Logs\\Watchdog";

namespace apps {
	namespace WatchDog {
		struct WatchDogProgramState
		{
			std::string current_test;
			ULONGLONG test_start_timestamp;
			ULONGLONG flow_start_timestamp;
			ULONGLONG tb_extend_timestamp;
			bool flow_running;
			bool check_test_time;
		};

		class WatchDog
		{
		private:
			WatchDogProgramState wd_state;
			std::mutex wd_state_lock;
			std::unordered_map<std::string, bool> wd_exclusions;
			bool configured = false;
			WatchDog() {}
		public:
			//make sure there is only one instance of this
			WatchDog(WatchDog const&) = delete;
			void operator=(WatchDog const&) = delete;
			static WatchDog& getInstance()
			{
				static WatchDog instance;
				return instance;
			}

			double absolute_max_test_timeout = 0.0;
			double test_timeout = 0.0;
			double tb_extend_timeout = 0.0;
			double remote_set_timeout = 0.0;
			ULONGLONG rest_time_ms = 0;
			int remote_set_retry_rate_ms = 0;
			bool exit_request = false;

			//These run on each of the Sites. They only log data therefore they do not depend
			//on the test being configured
			void flow_start();
			void flow_end();
			void test_start(CString test_name);
			void test_start(std::string test_name);
			void test_end();
			void add_exclusion(std::string);
			void disable_for_this_test();
			void extend_timeout();

			//These run on the Host
			void host_worker_thread();
			void configure(double AbsoluteMaxTestTimeout = (100 S), double TestTimeout = (60 S), double TbExtensionCallbackTimeout = (5 S), double RemoteSetTimeout = (5 S));
			void check();
			void bark(std::string site_ids, std::string message);
			void get_wd_state(WatchDogProgramState& wd_state_copy);
			std::atomic<bool> polling_active;  //This will ONLY controlling polling IF set from the HOST
		};
	}
}

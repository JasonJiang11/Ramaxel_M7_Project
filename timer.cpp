
#include "stdafx.h"
#include "timer.h"

#define __STRING_(s) #s
#define __STRING(s) __STRING_(s)

static time_data_t time_data;

void start_timer(const std::string& name)
{
#ifndef DISABLE_TIMER
	auto now = std::chrono::high_resolution_clock::now();
	time_data.add(name, now, true);
#endif
}

void end_timer(const std::string& name)
{
#ifndef DISABLE_TIMER
	auto now = std::chrono::high_resolution_clock::now();
	time_data.add(name, now, false);
#endif
}

void print_time_summary()
{
#ifndef DISABLE_TIMER
	time_data.print();
	time_data.clear();
#endif
}

void time_data_t::add(const std::string& keyword, time_t time, bool start)
{
	auto it = std::find_if(data.begin(), data.end(),
		[=](const std::pair<std::string, std::array<time_t, 2>> & elem)
		{
			return elem.first == keyword;
		});

	if (start)
	{
		std::array<time_t, 2> times;
		times[0] = time;
		times[1] = time;
		if (it == data.end())
			data.emplace_back(std::make_pair(keyword, times));
		else
		{
			warning("Duplicated TIME_IN(%s). Replaced to the latest.", keyword.c_str());
			it->second = times;
		}
	}
	else
	{
		if (it == data.end())
			warning("TIME_IN(%s) was not specified.", keyword.c_str());
		else
			it->second[1] = time;
	}
}

void time_data_t::print()
{
	std::stringstream os;

	os << std::endl;
	os << std::setw(54) << std::setfill('-') << " " << std::endl;
	os << "TIME DATA" << std::endl;
	os << std::setw(54) << std::setfill('-') << " " << std::endl;

	for (auto& elem : data)
	{
		auto key = elem.first;
		auto start = elem.second[0];
		auto end = elem.second[1];

		if (start == end)
			warning("TIME_OUT[%s] was not specified", key.c_str());
		else
		{
			auto time = std::chrono::duration_cast<std::chrono::MEASURE_UNIT>(end - start).count();
			os << std::setfill(' ') << std::left << std::setw(30) << key << std::right << std::setw(10) << time << " " << __STRING(MEASURE_UNIT) << std::endl;
		}
	}
	output(os.str().c_str());
}

void time_data_t::clear()
{
	data.clear();
}

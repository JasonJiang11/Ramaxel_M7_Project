#ifndef TIMER_H
#define TIMER_H
#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <array>

//#define DISABLE_TIMER    // serves as a compile switch for the timer code

void start_timer(const std::string& name);
void end_timer(const std::string& name);
void print_time_summary();

#define MEASURE_UNIT    milliseconds // milliseconds, microseconds, nanoseconds
#define TIME_IN(x)      start_timer(x)
#define TIME_OUT(x)     end_timer(x)
#define TIME_SUMMARY()  print_time_summary()

class time_data_t
{
public:
	using time_t = std::chrono::high_resolution_clock::time_point;
	void add(const std::string& keyword, time_t time, bool start);
	void print();
	void clear();



private:
	using time_data_type = std::vector<std::pair<std::string, std::array<time_t, 2>>>;
	time_data_type data;
	using time_data_iterator = time_data_type::iterator;
};

#endif

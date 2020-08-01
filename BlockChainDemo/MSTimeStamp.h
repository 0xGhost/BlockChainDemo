#pragma once
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

class TimeStamp
{
public:
	TimeStamp();
	~TimeStamp() {}

	std::string ToString() const;

	int compare(const TimeStamp& rhs); // -1: rhs after this, 0: same, 1: rhs before this

private:
	std::chrono::time_point<std::chrono::system_clock> now;
	long long nowAsTimeT;
	std::chrono::milliseconds nowMs;
};


std::string getTimestamp();
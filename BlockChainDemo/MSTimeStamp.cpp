#include "MSTimeStamp.h"

std::ostream& operator<<(std::ostream& out, const TimeStamp& ts)
{
	out << ts.ToString();
	return out;
}

std::istream& operator>>(std::istream& in, TimeStamp& ts)
{
	std::tm t;
	int i;
	in >> std::get_time(&t, "%a/%b/%d/%Y/%T.") >> i;
	ts.nowAsTimeT = mktime(&t);
	ts.nowMs = std::chrono::milliseconds(i);
	ts.now = std::chrono::system_clock::from_time_t(ts.nowAsTimeT);
	return in;
}

std::string getTimestamp() {
	// get a precise timestamp as a string
	const auto now = std::chrono::system_clock::now();

	const auto nowAsTimeT = std::chrono::system_clock::to_time_t(now);
	const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()) % 1000;
	std::stringstream nowSs;
	nowSs
		<< std::put_time(std::localtime(&nowAsTimeT), "%a/%b/%d/%Y/%T")
		<< '.' << std::setfill('0') << std::setw(3) << nowMs.count();// << "#data:" << now;
	return nowSs.str();
}

TimeStamp::TimeStamp()
{
	now = std::chrono::system_clock::now();

	nowAsTimeT = std::chrono::system_clock::to_time_t(now);
	nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()) % 1000;
}

std::string TimeStamp::ToString() const
{
	std::stringstream nowSs;
	nowSs
		<< std::put_time(std::localtime(&nowAsTimeT), "%a/%b/%d/%Y/%T")
		<< '.' << std::setfill('0') << std::setw(3) << nowMs.count();
	return nowSs.str();
}

int TimeStamp::compare(const TimeStamp& rhs)
{
	int result = difftime(nowAsTimeT, rhs.nowAsTimeT);
	if (result == 0)
	{
		long long d = nowMs.count() - rhs.nowMs.count();
		result = d >= 0 ? 1 : -1;
		result = d == 0 ? 0 : d;
	}
	else
	{
		result = result > 0 ? 1 : -1;
	}
	return result;
}

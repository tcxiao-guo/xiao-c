/*****************************************************************//**
 * @file   Date.cpp
 * @author xiao guo
 * 
 *
 * @date   2024-5-25 
 *********************************************************************/
#include "Date.h"
#include <chrono>
#include <vector>
#include "Funcs.h"

BEGIN_NAMESPACE(xiao)

const Date Date::date()
{
	auto now = std::chrono::system_clock::now();
	auto since_epoch = now.time_since_epoch();
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(since_epoch).count();
	return Date(microseconds);
}

const Date Date::after(double second) const
{
	return Date(static_cast<int64_t>(microSecondSinceEpoch_ + 
		second * MICRO_SECONDS_PER_SEC));
}
const Date Date::roundSecond() const
{
	return Date(microSecondSinceEpoch_ -
		(microSecondSinceEpoch_ % MICRO_SECONDS_PER_SEC));
}
const Date Date::roundDay() const
{
	std::tm t;
	time_t seconds =
		static_cast<time_t>(microSecondSinceEpoch_ / MICRO_SECONDS_PER_SEC);
#ifndef _WIN32
	localtime_r(&seconds, &t);
#else
	localtime_s(&t, &seconds);
#endif // !_WIN32
	t.tm_hour = 0;
	t.tm_min = 0;
	t.tm_sec = 0;
	return Date(mktime(&t) * MICRO_SECONDS_PER_SEC);
}
std::string Date::toFormattedString(bool showMicroseconds) const
{
	char buf[128]{ 0 };
	time_t seconds = static_cast<time_t>(microSecondSinceEpoch_ / MICRO_SECONDS_PER_SEC);
	std::tm tm_time;
#ifndef _WIN32
	gmtime_r(&seconds, &tm_time);
#else
	gmtime_s(&tm_time, &seconds);
#endif // !_WIN32

	if (showMicroseconds)
	{
		int microseconds = static_cast<int>(microSecondSinceEpoch_ % MICRO_SECONDS_PER_SEC);
		snprintf(buf,
			sizeof(buf),
			"%4d%02d%02d %02d:%02d:%02d.%06d",
			tm_time.tm_year + 1900,
			tm_time.tm_mon + 1,
			tm_time.tm_mday,
			tm_time.tm_hour,
			tm_time.tm_min,
			tm_time.tm_sec,
			microseconds);
	}
	else
	{
		snprintf(buf,
			sizeof(buf),
			"%4d%02d%02d %02d:%02d:%02d",
			tm_time.tm_year + 1900,
			tm_time.tm_mon + 1,
			tm_time.tm_mday,
			tm_time.tm_hour,
			tm_time.tm_min,
			tm_time.tm_sec);
	}
	return buf;
}

std::string Date::toCustomedFormattedString(const std::string& fmtStr,
	bool showMicroseconds) const
{
	char buf[256]{ 0 };
	time_t seconds = static_cast<time_t>(microSecondSinceEpoch_ / MICRO_SECONDS_PER_SEC);
	std::tm tm_time;
#ifndef _WIN32
	gmtime_t(&seconds, &tm_time);
#else
	gmtime_s(&tm_time, &seconds);
#endif // !_WIN32
	strftime(buf, sizeof(buf), fmtStr.c_str(), &tm_time);
	if (!showMicroseconds)
		return std::string(buf);
	char decimals[12]{ 0 };
	int microseconds = static_cast<int>(microSecondSinceEpoch_ % MICRO_SECONDS_PER_SEC);
	snprintf(decimals, sizeof(decimals), ".%06d", microseconds);
	return std::string(buf) + decimals;
}

void Date::toCustomedFormattedString(const std::string& fmtStr,
	char* str, size_t len)const
{
	time_t seconds = static_cast<time_t>(microSecondSinceEpoch_ / MICRO_SECONDS_PER_SEC);
	std::tm tm_time;
#ifndef _WIN32
	gmtime_r(&seconds, &tm_time);
#else
	gmtime_s(&tm_time, &seconds);
#endif
	strftime(str, len, fmtStr.c_str(), &tm_time);
}

std::string Date::toFormattedStringLocal(bool showMicroseconds) const
{
	char buf[128] = { 0 };
	time_t seconds = static_cast<time_t>(microSecondSinceEpoch_ / MICRO_SECONDS_PER_SEC);
	std::tm tm_time;
#ifndef _WIN32
	localtime_r(&seconds, &tm_time);
#else
	localtime_s(&tm_time, &seconds);
#endif

	if (showMicroseconds)
	{
		int microseconds = static_cast<int>(microSecondSinceEpoch_ % MICRO_SECONDS_PER_SEC);
		snprintf(buf,
			sizeof(buf),
			"%4d%02d%02d %02d:%02d:%02d.%06d",
			tm_time.tm_year + 1900,
			tm_time.tm_mon + 1,
			tm_time.tm_mday,
			tm_time.tm_hour,
			tm_time.tm_min,
			tm_time.tm_sec,
			microseconds);
	}
	else
	{
		snprintf(buf,
			sizeof(buf),
			"%4d%02d%02d %02d:%02d:%02d",
			tm_time.tm_year + 1900,
			tm_time.tm_mon + 1,
			tm_time.tm_mday,
			tm_time.tm_hour,
			tm_time.tm_min,
			tm_time.tm_sec);
	}
	return buf;
}

std::string Date::toDbStringLocal() const
{
	char buf[128] = { 0 };
	time_t seconds = static_cast<time_t>(microSecondSinceEpoch_ / MICRO_SECONDS_PER_SEC);
	std::tm tm_time;
#ifndef _WIN32
	localtime_r(&seconds, &tm_time);
#else
	localtime_s(&tm_time, &seconds);
#endif
	bool showMicroseconds = (microSecondSinceEpoch_ % MICRO_SECONDS_PER_SEC != 0);
	if (showMicroseconds)
	{
		int microseconds = static_cast<int>(microSecondSinceEpoch_ % MICRO_SECONDS_PER_SEC);
		snprintf(buf,
			sizeof(buf),
			"%4d%02d%02d %02d:%02d:%02d.%06d",
			tm_time.tm_year + 1900,
			tm_time.tm_mon + 1,
			tm_time.tm_mday,
			tm_time.tm_hour,
			tm_time.tm_min,
			tm_time.tm_sec,
			microseconds);
	}
	else
	{
		if (*this == roundDay())
		{
			snprintf(buf,
				sizeof(buf),
				"%4d-%02d-%02d",
				tm_time.tm_year + 1900,
				tm_time.tm_mon + 1,
				tm_time.tm_mday);
		}
		else
		{
			snprintf(buf,
				sizeof(buf),
				"%4d%02d%02d %02d:%02d:%02d",
				tm_time.tm_year + 1900,
				tm_time.tm_mon + 1,
				tm_time.tm_mday,
				tm_time.tm_hour,
				tm_time.tm_min,
				tm_time.tm_sec);
		}
	}
	return buf;
}

std::string Date::toDbString() const
{
	return after(static_cast<double>(-timezoneOffset())).toDbStringLocal();
}

Date Date::fromDbStringLocal(const std::string& datetime)
{
	uint32_t year{ 0 }, month{ 0 }, day{ 0 }, hour{ 0 }, minute{ 0 }, second{ 0 }, microSecond{ 0 };

	std::vector<std::string>&& v = splitString(datetime, " ");
	if (2 == v.size())
	{
		std::vector<std::string> date = splitString(v[0], "-");
		if (3 == v.size())
		{
			year = std::stol(date[0]);
			month = std::stol(date[1]);
			day = std::stol(date[2]);
			std::vector<std::string> time = splitString(v[1], ":");
			if (2 < time.size())
			{
				hour = std::stol(time[0]);
				minute = std::stol(time[1]);
				auto seconds = splitString(time[2], ".");
				second = std::stol(seconds[0]);
				if (1 < seconds.size())
				{
					if (seconds[1].length() > 6)
					{
						seconds[1].resize(6);
					}
					else if (seconds[1].length() < 6)
					{
						seconds[1].append(6 - seconds[1].length(), '0');
					}
					microSecond = std::stol(seconds[1]);
				}
			}
		}
	}
	return Date(year, month, day, hour, minute, second, microSecond);
}

Date Date::fromDbString(const std::string& datetime)
{
	return fromDbStringLocal(datetime).after(
		static_cast<double>(timezoneOffset()));
}

std::string Date::toCustomedFormattedStringLocal(const std::string& fmtStr,
	bool showMicroseconds) const
{
	char buf[256] = { 0 };
	time_t seconds =
		static_cast<time_t>(microSecondSinceEpoch_ / MICRO_SECONDS_PER_SEC);
	struct tm tm_time;
#ifndef _WIN32
	localtime_r(&seconds, &tm_time);
#else
	localtime_s(&tm_time, &seconds);
#endif
	strftime(buf, sizeof(buf), fmtStr.c_str(), &tm_time);
	if (!showMicroseconds)
		return std::string(buf);
	char decimals[12]{ 0 };
	int microseconds =
		static_cast<int>(microSecondSinceEpoch_ % MICRO_SECONDS_PER_SEC);
	snprintf(decimals, sizeof(decimals), ".%06d", microseconds);
	return std::string(buf) + decimals;
}

Date::Date(uint32_t year, uint32_t month, uint32_t day, uint32_t hour,
	uint32_t minute, uint32_t second, uint32_t microSecond)
{
	std::tm tm;
	memset(&tm, 0, sizeof(tm));
	tm.tm_isdst = -1;
	time_t epoch;
	tm.tm_year = year - 1900;
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = second;
	epoch = mktime(&tm);
	microSecondSinceEpoch_ = static_cast<int64_t>(epoch) * MICRO_SECONDS_PER_SEC + microSecond;
}
END_NAMESPACE(xiao)

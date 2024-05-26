/*****************************************************************//**
 * @file   Date.h
 * @author xiao guo
 * 
 *
 * @date   2024-5-25 
 *********************************************************************/

#pragma once

#include <xiao/exports.h>
#include <stdint.h>
#include <xiao/utils/xiao_marco.h>
#include <string>

#define MICRO_SECONDS_PER_SEC 1000000LL

BEGIN_NAMESPACE(xiao)


/**
 * @brief This class represents a time point.
 */
class XIAO_EXPORT Date
{
public:
	Date() :microSecondSinceEpoch_(0) {};

	explicit Date(int64_t microSec) : microSecondSinceEpoch_(microSec) {};

	Date(
		uint32_t year,
		uint32_t month,
		uint32_t day,
		uint32_t hour = 0,
		uint32_t minute = 0,
		uint32_t second = 0,
		uint32_t microSecond = 0
	);

	static const Date date();

	static const Date now()
	{
		return Date::date();
	}

	static int64_t timezoneOffset()
	{
		static int64_t offset = -(
			Date::fromDbStringLocal("1970-01-03 00:00:00").secondsSinceEpoch() -
			2LL * 3600LL * 24LL);
		return offset;
	}

	const Date after(double second) const;

	const Date roundSecond() const;

	const Date roundDay() const;

	~Date() {};

	bool operator==(const Date& date) const
	{
		return microSecondSinceEpoch_ == date.microSecondSinceEpoch_;
	}

	bool operator!=(const Date& date) const
	{
		return microSecondSinceEpoch_ != date.microSecondSinceEpoch_;
	}

	bool operator<(const Date& date) const
	{
		return microSecondSinceEpoch_ < date.microSecondSinceEpoch_;
	}

	bool operator>(const Date& date) const
	{
		return microSecondSinceEpoch_ > date.microSecondSinceEpoch_;
	}

	bool operator>=(const Date& date) const
	{
		return microSecondSinceEpoch_ > date.microSecondSinceEpoch_;
	}

	bool operator<=(const Date& date) const
	{
		return microSecondSinceEpoch_ < date.microSecondSinceEpoch_;
	}

	int64_t microSecondsSinceEpoch() const
	{
		return microSecondSinceEpoch_;
	}

	int64_t secondsSinceEpoch() const
	{
		return microSecondSinceEpoch_ / MICRO_SECONDS_PER_SEC;
	}

	struct tm tmStruct() const;

	std::string toFormattedString(bool showMicroseconds) const;

	std::string toCustomedFormattedString(const std::string& fmtStr,
		bool showMicroseconds = false) const;

	std::string toFormattedStringLocal(bool showMicroseconds) const;

	std::string toCustomedFormattedStringLocal(
		const std::string& fmtStr,
		bool showMicroseconds = false)const;

	std::string toDbStringLocal() const;
	
	std::string toDbString() const;

	static Date fromDbStringLocal(const std::string& datetime);

	static Date fromDbString(const std::string& datetime);

	void toCustomedFormattedString(const std::string& fmtStr,
								   char* str,
								   size_t len) const;

	bool isSameSecond(const Date& date) const
	{
		return microSecondSinceEpoch_ / MICRO_SECONDS_PER_SEC ==
			date.microSecondSinceEpoch_ / MICRO_SECONDS_PER_SEC;
	}

	void swap(Date& that)
	{
		std::swap(microSecondSinceEpoch_, that.microSecondSinceEpoch_);
	}

private:
	int64_t microSecondSinceEpoch_{ 0 };
};


END_NAMESPACE(xiao)


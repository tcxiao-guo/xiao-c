/*****************************************************************//**
 * @file   LogStream.cpp
 * @author xiao guo
 * 
 *
 * @date   2024-5-25 
 *********************************************************************/

#include <xiao/utils/LogStream.h>


using namespace xiao;
using namespace xiao::detail;

BEGIN_NAMESPACE(xiao)
BEGIN_NAMESPACE(detail)
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

const char digitHex[] = "0123456789ABCDEF";

template <typename T>
size_t convert(char buf[], T value)
{
	T i = value;
	char* p = buf;

	do
	{
		int lsd = static_cast<int>(i % 10);
		i /= 10;
		*p++ = zero[lsd];
	} while (i != 0);

	if (value < 0)
	{
		*p++ = '-';
	}
	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

size_t convertHex(char buf[], uintptr_t value)
{
	uintptr_t i = value;
	char* p = buf;

	do
	{
		int lsd = static_cast<int>(i % 16);
		i /= 16;
		*p++ = digitHex[lsd];
	} while (i != 0);
	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

template class FixedBuffer<xSmallBuffer>;
template class FixedBuffer<xLargeBuffer>;
END_NAMESPACE(detail)
END_NAMESPACE(xiao)

template <int SIZE>
const char* FixedBuffer<SIZE>::debugString()
{
	*cur_ = '\0';
	return data_;
}

template <int SIZE>
void FixedBuffer<SIZE>::cookieStart()
{
}

template <int SIZE>
void FixedBuffer<SIZE>::cookieEnd()
{
}

template <typename T>
void LogStream::formatInteger(T v)
{
	constexpr static int xMaxNumericSize = std::numeric_limits<T>::digits10 + 4;
	if (exBuffer_.empty())
	{
		if (buffer_.avail() >= xMaxNumericSize)
		{
			size_t len = convert(buffer_.current(), v);
			buffer_.add(len);
			return;
		}
		else
		{
			exBuffer_.append(buffer_.data(), buffer_.length());
		}
	}
	auto oldLen = exBuffer_.length();
	exBuffer_.resize(oldLen + xMaxNumericSize);
	size_t len = convert(&exBuffer_[oldLen], v);
	exBuffer_.resize(oldLen + len);
}

LogStream& LogStream::operator<<(short v)
{
	*this << static_cast<int>(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
	*this << static_cast<unsigned int>(v);
	return *this;
}

LogStream& LogStream::operator<<(int v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(const long long& v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(const unsigned long long& v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
	uintptr_t v = reinterpret_cast<uintptr_t>(p);
	constexpr static int xMaxNumberSize = std::numeric_limits<uintptr_t>::digits / 4 + 4;

	if (exBuffer_.empty())
	{
		if (buffer_.avail() >= xMaxNumberSize)
		{
			char* buf = buffer_.current();
			buf[0] = '0';
			buf[1] = 'x';
			size_t len = convertHex(buf + 2, v);
			buffer_.add(len + 2);
			return *this;
		}
		else
		{
			exBuffer_.append(buffer_.data(), buffer_.length());
		}
	}
	auto oldLen = exBuffer_.length();
	exBuffer_.resize(oldLen + xMaxNumberSize);
	char* buf = &exBuffer_[oldLen];
	buf[0] = '0';
	buf[1] = 'x';
	size_t len = convertHex(buf + 2, v);
	exBuffer_.resize(oldLen + len + 2);
	return *this;
}

LogStream& LogStream::operator<<(const double& v)
{
	constexpr static int kMaxNumericSize = 32;
	if (exBuffer_.empty())
	{
		if (buffer_.avail() >= kMaxNumericSize)
		{
			int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
			buffer_.add(len);
			return *this;
		}
		else
		{
			exBuffer_.append(buffer_.data(), buffer_.length());
		}
	}
	auto oldLen = exBuffer_.length();
	exBuffer_.resize(oldLen + kMaxNumericSize);
	int len = snprintf(&(exBuffer_[oldLen]), kMaxNumericSize, "%.12g", v);
	exBuffer_.resize(oldLen + len);
	return *this;
}

LogStream& LogStream::operator<<(const long double& v)
{
	constexpr static int kMaxNumericSize = 48;
	if (exBuffer_.empty())
	{
		if (buffer_.avail() >= kMaxNumericSize)
		{
			int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12Lg", v);
			buffer_.add(len);
			return *this;
		}
		else
		{
			exBuffer_.append(buffer_.data(), buffer_.length());
		}
	}
	auto oldLen = exBuffer_.length();
	exBuffer_.resize(oldLen + kMaxNumericSize);
	int len = snprintf(&(exBuffer_[oldLen]), kMaxNumericSize, "%.12Lg", v);
	exBuffer_.resize(oldLen + len);
	return *this;
}

template <typename T>
Fmt::Fmt(const char* fmt, T val)
{
	length_ = snprintf(buf_, sizeof buf_, fmt, val);
	assert(static_cast<size_t>(length_) < sizeof buf_);
}


template XIAO_EXPORT Fmt::Fmt(const char* fmt, char);

template XIAO_EXPORT Fmt::Fmt(const char* fmt, short);
template XIAO_EXPORT Fmt::Fmt(const char* fmt, unsigned short);
template XIAO_EXPORT Fmt::Fmt(const char* fmt, int);
template XIAO_EXPORT Fmt::Fmt(const char* fmt, unsigned int);
template XIAO_EXPORT Fmt::Fmt(const char* fmt, long);
template XIAO_EXPORT Fmt::Fmt(const char* fmt, unsigned long);
template XIAO_EXPORT Fmt::Fmt(const char* fmt, long long);
template XIAO_EXPORT Fmt::Fmt(const char* fmt, unsigned long long);

template XIAO_EXPORT Fmt::Fmt(const char* fmt, float);
template XIAO_EXPORT Fmt::Fmt(const char* fmt, double);
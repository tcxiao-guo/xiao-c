/**
 * @file   AsyncStream.h
 * @author xiao guo
 * 
 *
 * @date   2024-5-26 
 */
#pragma once

#include <xiao/utils/NonCopyable.h>
#include <string>
#include <memory>

BEGIN_NAMESPACE(xiao)

/**
 * @brief This class represents a data stream that can be sent asynchronously.
 */
class XIAO_EXPORT AsyncStream : public NonCopyable
{
public:
	virtual ~AsyncStream() = default;

	/**
	 * @brief Send data asynchronously.
	 * 
	 * \param data
	 * \param len
	 * \return 
	 */
	virtual bool send(const char* data, size_t len) = 0;
	bool send(const std::string& data)
	{
		return send(data.data(), data.length());
	}
	/**
	 * @brief Terminate the stream..
	 */
	virtual void close() = 0;
};
using AsyncStreamPtr = std::unique_ptr<AsyncStream>;

END_NAMESPACE(xiao)

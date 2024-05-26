/**
 * @file   WindowsSupport.h
 * @author xiao guo
 * 
 *
 * @date   2024-5-26 
 */
#pragma once

#include <xiao/exports.h>

struct iovec
{
	void* iov_base;
	int iov_len;
};

XIAO_EXPORT int readv(int fd, const struct iovec* vector, int count);

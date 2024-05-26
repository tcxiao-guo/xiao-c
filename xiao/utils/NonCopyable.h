/*****************************************************************//**
 * @file   NonCopyable.h
 * @author xiao guo
 * 
 *
 * @date   2024-5-25 
 *********************************************************************/
#pragma once

#include <xiao/exports.h>
#include <xiao/utils/xiao_marco.h>

BEGIN_NAMESPACE(xiao)

class XIAO_EXPORT NonCopyable
{
protected:
	NonCopyable(){}
	~NonCopyable(){}

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) noexcept(true) = default;
	NonCopyable& operator=(NonCopyable&&) noexcept(true) = default;
};

END_NAMESPACE(xiao)

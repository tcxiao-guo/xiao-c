/**
 * @file   Utilities.h
 * @author xiao guo
 * 
 *
 * @date   2024-5-26 
 */

#pragma once

#include <xiao/exports.h>
#include <xiao/utils/xiao_marco.h>
#include <string>

BEGIN_NAMESPACE(xiao)
BEGIN_NAMESPACE(utils)

XIAO_EXPORT std::string toUtf8(const std::wstring& wstr);
XIAO_EXPORT std::wstring fromUtf8(const std::string& str);

XIAO_EXPORT std::string fromWidePath(const std::wstring& strPath);
XIAO_EXPORT std::wstring toWidePath(const std::string& strUtf8pPath);

#if defined(_WIN32) && !defined(__MINGW32__)
inline std::wstring toNativePath(const std::string& strPath)
{
	return toWidePath(strPath);
}

inline const std::wstring& toNativePath(const std::wstring& strPath)
{
	return strPath;
}
#else
inline const std::string& toNativePath(const std::string& strPath)
{
	return strPath;
}

inline std::string toNativePath(const std::wstring& strPath)
{
	return fromWidePath(strPath);
}
#endif

inline const std::string& fromNativePath(const std::string& strPath)
{
	return strPath;
}

inline std::string fromNativePath(const std::wstring& strPath)
{
	return fromWidePath(strPath);
}

bool verifySslName(const std::string& certName, const std::string& hostName);

XIAO_EXPORT std::string tlsBackend();

struct Hash128
{
	unsigned char bytes[16];
};

struct Hash160
{
	unsigned char bytes[20];
};

struct Hash256
{
	unsigned char bytes[32];
};

XIAO_EXPORT Hash128 md5(const void* data, size_t len);
inline Hash128 md5(const std::string& str)
{
	return md5(str.data(), str.size());
}

XIAO_EXPORT Hash160 sha1(const void* data, size_t len);
inline Hash160 sha1(const std::string& str)
{
	return sha1(str.data(), str.size());
}

XIAO_EXPORT Hash256 sha256(const void* data, size_t len);
inline Hash256 sha256(const std::string& str)
{
	return sha256(str.data(), str.size());
}

XIAO_EXPORT Hash256 sha3(const void* data, size_t len);
inline Hash256 sha3(const std::string& str)
{
	return sha3(str.data(), str.size());
}

XIAO_EXPORT Hash256 blake2b(const void* data, size_t len);
inline Hash256 blake2b(const std::string& str)
{
	return blake2b(str.data(), str.size());
}

XIAO_EXPORT std::string toHexString(const void* data, size_t len);
inline std::string toHexString(const Hash128& hash)
{
	return toHexString(hash.bytes, sizeof(hash.bytes));
}

inline std::string toHexString(const Hash160& hash)
{
	return toHexString(hash.bytes, sizeof(hash.bytes));
}

inline std::string toHexString(const Hash256& hash)
{
	return toHexString(hash.bytes, sizeof(hash.bytes));
}

XIAO_EXPORT bool secureRandomBytes(void* ptr, size_t size);

END_NAMESPACE(utils)
END_NAMESPACE(xiao)

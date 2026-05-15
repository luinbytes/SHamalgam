#pragma once

#ifdef __linux__
#include <cfloat>
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>
#include <algorithm>
#include <cmath>
#include <xmmintrin.h>

#ifndef __int8
#define __int8 char
#endif
#ifndef __int16
#define __int16 short
#endif
#ifndef __int32
#define __int32 int
#endif
#ifndef __int64
#define __int64 long long
#endif

#ifndef __CONFIGURATION__
#define __CONFIGURATION__ "Linux"
#endif

#ifndef FORCEINLINE
#define FORCEINLINE inline __attribute__((always_inline))
#endif
#ifndef __forceinline
#define __forceinline inline __attribute__((always_inline))
#endif
struct _CrtMemState {};

inline int sprintf_s(char* buffer, size_t size, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int result = std::vsnprintf(buffer, size, format, args);
	va_end(args);
	return result;
}

inline int strcpy_s(char* dest, const char* src)
{
	std::strcpy(dest, src);
	return 0;
}

inline int strcpy_s(char* dest, size_t size, const char* src)
{
	if (!dest || !size)
		return 1;
	std::strncpy(dest, src ? src : "", size - 1);
	dest[size - 1] = '\0';
	return 0;
}

inline int strncpy_s(char* dest, const char* src, size_t size)
{
	return strcpy_s(dest, size, src);
}

inline int strncpy_s(char* dest, size_t size, const char* src, size_t count)
{
	if (!dest || !size)
		return 1;
	size_t n = std::min(size - 1, count);
	if (src)
		std::strncpy(dest, src, n);
	dest[n] = '\0';
	return 0;
}

inline void* _aligned_malloc(size_t size, size_t alignment)
{
	size_t rounded = (size + alignment - 1) / alignment * alignment;
	return std::aligned_alloc(alignment, rounded);
}

inline void _aligned_free(void* ptr)
{
	std::free(ptr);
}
#ifndef __FUNCSIG__
#define __FUNCSIG__ ""
#endif

#ifndef stricmp
#define stricmp strcasecmp
#endif

template <class T>
inline void V_swap(T& lhs, T& rhs)
{
	std::swap(lhs, rhs);
}

using std::isfinite;
#endif

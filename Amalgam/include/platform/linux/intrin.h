#pragma once

#include <x86intrin.h>
#include <immintrin.h>

static inline unsigned char _BitScanForward(unsigned long* index, unsigned long mask)
{
	if (!mask)
		return 0;
	*index = static_cast<unsigned long>(__builtin_ctzl(mask));
	return 1;
}

static inline unsigned char _BitScanReverse(unsigned long* index, unsigned long mask)
{
	if (!mask)
		return 0;
	*index = static_cast<unsigned long>((sizeof(unsigned long) * 8 - 1) - __builtin_clzl(mask));
	return 1;
}


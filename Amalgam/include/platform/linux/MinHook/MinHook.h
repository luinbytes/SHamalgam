#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum MH_STATUS
{
	MH_UNKNOWN = -1,
	MH_OK = 0,
	MH_ERROR_ALREADY_INITIALIZED,
	MH_ERROR_NOT_INITIALIZED,
	MH_ERROR_ALREADY_CREATED,
	MH_ERROR_NOT_CREATED,
	MH_ERROR_ENABLED,
	MH_ERROR_DISABLED,
	MH_ERROR_NOT_EXECUTABLE,
	MH_ERROR_UNSUPPORTED_FUNCTION,
	MH_ERROR_MEMORY_ALLOC,
	MH_ERROR_MEMORY_PROTECT,
	MH_ERROR_MODULE_NOT_FOUND,
	MH_ERROR_FUNCTION_NOT_FOUND
} MH_STATUS;

#define MH_ALL_HOOKS NULL

MH_STATUS MH_Initialize(void);
MH_STATUS MH_Uninitialize(void);
MH_STATUS MH_CreateHook(void* pTarget, void* pDetour, void** ppOriginal);
MH_STATUS MH_RemoveHook(void* pTarget);
MH_STATUS MH_EnableHook(void* pTarget);
MH_STATUS MH_DisableHook(void* pTarget);
MH_STATUS MH_QueueEnableHook(void* pTarget);
MH_STATUS MH_QueueDisableHook(void* pTarget);
MH_STATUS MH_ApplyQueued(void);
const char* MH_StatusToString(MH_STATUS status);

#ifdef __cplusplus
}
#endif


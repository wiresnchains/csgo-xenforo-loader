#pragma once
#include <string>
#include <Windows.h>
#define RC_INVOKED
#include <handleapi.h>
#include <TlHelp32.h>

namespace helpers
{
	extern DWORD GetProcId(const char* procName);
}
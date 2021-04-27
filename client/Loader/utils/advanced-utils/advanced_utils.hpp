/*
			ADVANCED UTILITIES LIBRARY BY FLOWXRC
			MADE FOR SWIFTWARE
*/
#pragma once
#include <string>
#include <random>
#define VC_EXTRALEAN
#include <Windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <shlwapi.h>
#include <memory>
#include <comdef.h>
#pragma comment(lib, "th32.lib")
#include "../lazy-importer/lazy_importer.hpp"

namespace utilities
{
	extern __forceinline std::string get_random_string(size_t length);
	extern __forceinline std::string get_hwid();
	extern HANDLE GetProcessByName(const char* name);
	BOOL IsProcessRunning(DWORD pid);
}
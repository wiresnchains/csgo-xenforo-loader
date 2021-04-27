#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <Windows.h>
#include <TlHelp32.h>
#include <io.h>
#define RC_INVOKED
#include <handleapi.h>
#include <wininet.h>
#include <string>
#include <tchar.h>
#include <vector>
#include <urlmon.h>
#include <shlobj.h>
#include <filesystem>
#include <fstream>
#include <cstdio>
#pragma comment(lib, "WinINet.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "urlmon.lib")
#include "Helpers/GetProcId.hpp"
#include "../webclient/webclient.hpp"
#include "../encrypt-decrypt/xorstr.hpp"
#include "../../headers/globals.hpp"

namespace uLoaderInjector {
	extern int inject_cheat();
}
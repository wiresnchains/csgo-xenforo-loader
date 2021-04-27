/*
			WEBCLIENT LIBRARY BY FLOWXRC
			MADE FOR SWIFTWARE
*/
#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <wininet.h>
#include <string>
#include <tchar.h>
#include <tlhelp32.h>
#include <vector>
#include <urlmon.h>
#pragma comment(lib, "WinINet.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "urlmon.lib")
using namespace std;

namespace WebClient {
	extern string replaceAll(string subject, const string& search, const string& replace);
	extern string DownloadString(string URL);
	extern void DownloadFile(LPCSTR URL, LPCSTR file_loc);
}
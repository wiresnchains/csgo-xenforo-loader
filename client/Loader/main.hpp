#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "headers/globals.hpp"
#include "utils/webclient/webclient.hpp"
#include "utils/encrypt-decrypt/xorstr.hpp"
#include "utils/inject/Helpers/GetProcId.hpp"
#include "utils/inject/inject.hpp"
#include "protection/ErasePEHeaders.hpp"
#include "protection/AntiDebug/AntiDebug.hpp"

namespace uLoader
{
	extern bool check_version();
	extern void setup_loader();
	extern int checkUser();
	extern void userPanel();
	extern void authPanel();
}
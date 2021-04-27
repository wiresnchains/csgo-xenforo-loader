#pragma once
#include <string>
#include "../utils/advanced-utils/advanced_utils.hpp"

using namespace utilities;

namespace Globals
{
	static struct
	{
		std::string server = xorstr_("mysite.com");
		std::string forum_dir = xorstr_("/");
		std::string secret_key = xorstr_("1234567890");
	} server_side;

	static struct
	{
		std::string version = xorstr_("1.0");
		std::string client_key = xorstr_("0987654321");
		std::string cheat = xorstr_("Pastaware");
	} client_side;
};
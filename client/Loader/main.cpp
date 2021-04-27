#include "main.hpp"

using namespace std;

static string username;
static string password;
static string stop;

namespace uLoader
{
    bool check_version()
    {
        string server_version = WebClient::DownloadString(xorstr_("https://") + Globals::server_side.server + xorstr_("/client/client.php?key=") + Globals::client_side.client_key + xorstr_("&check_version=1"));
        if (server_version == Globals::client_side.version) {
            return true;
        }
        else {
            return false;
        }
    }
    void setup_loader()
    {
        cout << xorstr_("Welcome to ") + Globals::client_side.cheat + xorstr_("\n");
        SetConsoleTitleA(utilities::get_random_string(15).c_str());
    }
    int checkUser()
    {
        string output = WebClient::DownloadString(xorstr_("https://") + Globals::server_side.server + xorstr_("/") + Globals::server_side.forum_dir + xorstr_("loader.php?username=") + username + xorstr_("&password=") + password + xorstr_("&hwid=") + utilities::get_hwid());
        if (output == xorstr_("success")) {
            return 1;
        }
        else if (output == xorstr_("hwid:fail")) {
            return 2;
        }
        else {
            return 0;
        }
    }
    void userPanel()
    {
        system("CLS");
        static string inject;
        cout << xorstr_("Welcome to Swiftware, ") + username + xorstr_("\n");
        cout << xorstr_("Inject cheat? (y/n): ");
        cin >> inject;
        if (inject == xorstr_("y")) {
            if (helpers::GetProcId(xorstr_("csgo.exe")) == 0) {
                MessageBox(NULL, xorstr_("Launch csgo before loading cheat!"), Globals::client_side.cheat.c_str(), MB_ICONINFORMATION | MB_DEFBUTTON2);
                ErasePEHeaderFromMemory();
                return;
            }
            else {
                cout << xorstr_("Injecting...\n");

                uLoaderInjector::inject_cheat();
                ErasePEHeaderFromMemory();
                return;
            }
        }
        else {
            ErasePEHeaderFromMemory();
            return;
        }
    }
    void authPanel()
    {
        cout << xorstr_("Username: ");
        cin >> username;
        cout << xorstr_("Password: ");
        cin >> password;
    }
}

int main()
{
    if (protection::check_security() != protection::debug_results::none)
        return 0;

    if (!uLoader::check_version()) {
        MessageBox(NULL, xorstr_("Please, download new loader version!"), Globals::client_side.cheat.c_str(), MB_ICONINFORMATION | MB_DEFBUTTON2);
        ErasePEHeaderFromMemory();
        return 0;
    }

    uLoader::setup_loader();
    uLoader::authPanel();

    if (uLoader::checkUser() == 1) {
        uLoader::userPanel();
    }
    if (uLoader::checkUser() == 2) {
        MessageBox(NULL, xorstr_("HWID Error"), Globals::client_side.cheat.c_str(), MB_ICONERROR | MB_DEFBUTTON2);
        ErasePEHeaderFromMemory();
        return 0;
    }
    else {
        ErasePEHeaderFromMemory();
        return 0;
    }
}
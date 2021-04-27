// for swiftware

#include "inject.hpp"

std::string getPath(int csidl) {
    char out[MAX_PATH];
    if (SHGetSpecialFolderPathA(NULL, out, csidl, 0))
        return out;
    return NULL;
}

void HideConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

namespace uLoaderInjector
{
	int inject_cheat()
	{
        std::string dllPath = getPath(CSIDL_APPDATA);
        dllPath += "\\";
        dllPath += utilities::get_random_string(16);
        dllPath += ".dll";
        string link = xorstr_("https://") + Globals::server_side.server + xorstr_("/client/download.php?key=") + Globals::server_side.secret_key;
        WebClient::DownloadFile(link.c_str(), dllPath.c_str());
        DWORD procId = helpers::GetProcId("csgo.exe");
        HANDLE hProcessId = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        auto h_proc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

        if (h_proc && h_proc != INVALID_HANDLE_VALUE)
        {
            const LPVOID nt_open_file = GetProcAddress(LoadLibraryW(L"ntdll"), "NtOpenFile");
            if (nt_open_file)
            {
                char original_bytes[5];
                memcpy(original_bytes, nt_open_file, 5);
                WriteProcessMemory(h_proc, nt_open_file, original_bytes, 5, nullptr);
            }
            auto* loc = VirtualAllocEx(h_proc, nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            WriteProcessMemory(h_proc, loc, dllPath.c_str(), strlen(dllPath.c_str()) + 1, nullptr);
            auto* const h_thread = CreateRemoteThread(h_proc, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA), loc, 0, nullptr);
            if (h_thread) CloseHandle(h_thread);
        }
        cout << xorstr_("[+] Cheat Loaded. Have Fun!\n");
        Sleep(3000);
        HideConsole();
        while (true)
        {
            if (!utilities::IsProcessRunning(helpers::GetProcId("csgo.exe"))) {
                std::remove(dllPath.c_str());
                break;
            }
            Sleep(500);
        }
        return 0;
	}
}
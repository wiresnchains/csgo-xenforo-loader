#include "webclient.hpp"

namespace WebClient
{
    string replaceAll(string subject, const string& search,
        const string& replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    }

    string DownloadString(string URL) {
        HINTERNET interwebs = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
        HINTERNET urlFile;
        string rtn;
        if (interwebs) {
            urlFile = InternetOpenUrlA(interwebs, URL.c_str(), NULL, NULL, NULL, NULL);
            if (urlFile) {
                char buffer[2000];
                DWORD bytesRead;
                do {
                    InternetReadFile(urlFile, buffer, 2000, &bytesRead);
                    rtn.append(buffer, bytesRead);
                    memset(buffer, 0, 2000);
                } while (bytesRead);
                InternetCloseHandle(interwebs);
                InternetCloseHandle(urlFile);
                string p = replaceAll(rtn, "|n", "\r\n");
                return p;
            }
        }
        InternetCloseHandle(interwebs);
        string p = replaceAll(rtn, "|n", "\r\n");
        return p;
    }

    void DownloadFile(LPCSTR URL, LPCSTR file_loc) {
        URLDownloadToFileA(0, URL, file_loc, 0, 0);
    }
}
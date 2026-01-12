#include "WinHttpClient.h"
#include "Logger.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

#include <string>

namespace tradex {

    static std::wstring Utf8ToWide(const std::string& s) {
        if (s.empty()) return {};
        int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
        if (len <= 0) return {};
        std::wstring ws(len, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), ws.data(), len);
        return ws;
    }

    static bool CrackUrl(const std::wstring& url, std::wstring& host, std::wstring& path, bool& isHttps, INTERNET_PORT& port) {
        URL_COMPONENTS uc{};
        uc.dwStructSize = sizeof(uc);

        wchar_t hostBuf[256]{};
        wchar_t pathBuf[2048]{};
        wchar_t extraBuf[2048]{};

        uc.lpszHostName = hostBuf;
        uc.dwHostNameLength = (DWORD)_countof(hostBuf);

        uc.lpszUrlPath = pathBuf;
        uc.dwUrlPathLength = (DWORD)_countof(pathBuf);

        uc.lpszExtraInfo = extraBuf;
        uc.dwExtraInfoLength = (DWORD)_countof(extraBuf);

        std::wstring tmp = url; 
        if (!WinHttpCrackUrl(tmp.data(), (DWORD)tmp.size(), 0, &uc)) return false;

        isHttps = (uc.nScheme == INTERNET_SCHEME_HTTPS);
        port = (uc.nPort != 0) ? uc.nPort : (isHttps ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT);

        host.assign(uc.lpszHostName, uc.dwHostNameLength);

        path.assign(uc.lpszUrlPath, uc.dwUrlPathLength);
        if (uc.lpszExtraInfo && uc.dwExtraInfoLength > 0) {
            path.append(uc.lpszExtraInfo, uc.dwExtraInfoLength);
        }

        return !host.empty() && !path.empty();
    }

    static void Cleanup(HINTERNET req, HINTERNET con, HINTERNET ses) {
        if (req) WinHttpCloseHandle(req);
        if (con) WinHttpCloseHandle(con);
        if (ses) WinHttpCloseHandle(ses);
    }

    std::string WinHttpClient::Get(const std::string& urlUtf8, unsigned long timeoutMs) {
        const std::wstring url = Utf8ToWide(urlUtf8);

        std::wstring host, path;
        bool isHttps = true;
        INTERNET_PORT port = INTERNET_DEFAULT_HTTPS_PORT;

        if (!CrackUrl(url, host, path, isHttps, port)) {
            Logger::Log("TradeX WinHTTP: Failed to parse URL."); //for debugging
            return {};
        }

        HINTERNET ses = WinHttpOpen(L"TradeX/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);
        if (!ses) {
			Logger::Log("TradeX WinHTTP: WinHttpOpen failed."); //for debugging
            return {};
        }

        WinHttpSetTimeouts(ses, timeoutMs, timeoutMs, timeoutMs, timeoutMs);

        HINTERNET con = WinHttpConnect(ses, host.c_str(), port, 0);
        if (!con) {
            Logger::Log("TradeX WinHTTP: WinHttpConnect failed."); //for debugging
            Cleanup(nullptr, con, ses);
            return {};
        }

        DWORD flags = isHttps ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET req = WinHttpOpenRequest(con, L"GET", path.c_str(),
            nullptr, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            flags);
        if (!req) {
            Logger::Log("TradeX WinHTTP: WinHttpOpenRequest failed."); //for debugging
            Cleanup(req, con, ses);
            return {};
        }

        const wchar_t* headers =
            L"Accept: application/json\r\n"
            L"Connection: close\r\n";
        (void)WinHttpAddRequestHeaders(req, headers, (DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD);

        if (!WinHttpSendRequest(req, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
            Logger::Log("TradeX WinHTTP: WinHttpSendRequest failed."); //for debugging
            Cleanup(req, con, ses);
            return {};
        }

        if (!WinHttpReceiveResponse(req, nullptr)) {
            Logger::Log("TradeX WinHTTP: WinHttpReceiveResponse failed."); //for debugging
            Cleanup(req, con, ses);
            return {};
        }

        
        std::string out;
        DWORD avail = 0;
        while (true) {
            if (!WinHttpQueryDataAvailable(req, &avail)) break;
            if (avail == 0) break;

            std::string chunk;
            chunk.resize(avail);

            DWORD read = 0;
            if (!WinHttpReadData(req, chunk.data(), avail, &read)) break;

            chunk.resize(read);
            out += chunk;
        }

        Cleanup(req, con, ses);
        return out;
    }

} 

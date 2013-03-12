

#include "stdafx.h"

wstring s2ws(const string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    wstring r(buf);
    delete[] buf;
    return r;
}

std::string ws2s(const std::wstring& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0); 
    char* buf = new char[len];
    WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0); 
    std::string r(buf);
    delete[] buf;
    return r;
}

void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
     // Note that LONGLONG is a 64-bit value
     LONGLONG ll;

     ll = Int32x32To64(t, 10000000) + 116444736000000000;
     pft->dwLowDateTime = (DWORD)ll;
     pft->dwHighDateTime = ll >> 32;
}



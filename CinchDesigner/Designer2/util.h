
#include "stdafx.h"
#include <time.h>
#include <stdio.h>

namespace Designer {
std::wstring s2ws(const std::string& s);
std::string ws2s(const std::wstring& s);

void UnixTimeToFileTime(time_t t, LPFILETIME pft);
}
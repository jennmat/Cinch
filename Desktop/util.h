
#include <stdlib.h>
#include <Rpc.h>


using namespace std;

wstring s2ws(const string& s);

std::string ws2s(const std::wstring& s);

void UnixTimeToFileTime(time_t t, LPFILETIME pft);

string CreateUUID();
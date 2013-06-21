#pragma once

#include "resource.h"
#include "stdafx.h"

HWND edit;
HWND edit2;
CURLM *multi_handle;
CURL *http_handle;
CURL *http_handle2;

void download_url(const char *url, void* writeFunc);
	
void do_curl_setup(void);
void do_curl_processing(void);
void do_curl_cleanup(void);

std::string ws2s(const std::wstring& s);
std::wstring s2ws(const std::string& s);

AsyncWinHttp http;
			
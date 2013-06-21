#pragma once

#include "resource.h"
#include "stdafx.h"

HWND edit;
HWND edit2;
CURLM *multi_handle;
CURL *http_handle;
CURL *http_handle2;

	
void do_curl_setup(void);
void do_curl_processing(void);
void do_curl_cleanup(void);


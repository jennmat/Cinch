

#include "stdafx.h"
#include "JsonBox.h"

using namespace JsonBox;

#define MAX_DETAIL_PAGES 50

#define TABLE_CONTENT 2000
#define TEXTAREA_CONTENT 2001

class Detail {
private:
	HWND detailPages[MAX_DETAIL_PAGES];
	int contentType[MAX_DETAIL_PAGES];
	wchar_t* fieldName[MAX_DETAIL_PAGES];
	HWND detail;
	HWND tabControl;
	
public:
	Detail();
	Array JsonFormat();
	void load(Object obj);
	void addDetailPage(LPWSTR title);
	void removeAllDetailPages();
	int getDetailPageCount();
	void getDetailPageTitle(int, wchar_t*);
	void CreateTableForPage(wchar_t*, int i);
	void CreateTextareaForPage(wchar_t*, int i);
	HWND GetDetailPage(int i);
	void ShowPage(int i);
	void show(HWND parent, HINSTANCE hInst, RECT displayArea);

	void LoadDocument(Object obj);
	void StoreDocument();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};


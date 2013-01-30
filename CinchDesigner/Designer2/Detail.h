

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
	Form* form;
public:
	Detail();
	void setForm(Form *);
	Form* getForm();
	Array serializeUIElements();
	void deserializeUIElements(Object obj);
	void addDetailPage(LPWSTR title);
	void removeAllDetailPages();
	int getDetailPageCount();
	void getDetailPageTitle(int, wchar_t*);
	void CreateTableForPage(const wchar_t*, GridDelegate* delegate, int i);
	void CreateTextareaForPage(const wchar_t*, int i);
	HWND GetDetailPage(int i);
	void ShowPage(int i);
	void show(HWND parent, HINSTANCE hInst, RECT displayArea);

	void LoadDocument(Object obj);
	Object StoreValuesToDocument(int changedFieldId, Object obj);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};


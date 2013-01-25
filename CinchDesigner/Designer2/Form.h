
#include "stdafx.h"
#include "CouchDB.hpp"

using namespace CouchDB;

class Form
{
private:
	FormLayout layout;
	Detail detail;
	HWND parent;
	string id;
	Object obj;
public:
	Form();
	void NewDocument();
	void LoadDocument(string _id, Object obj);
	void SaveDocument(int changedFieldId);
	void addField(FormField* field);
	void addDetail(LPWSTR title);
	void removeAllDetailPages();
	Detail* getDetail();
	void show(HWND parent, HINSTANCE hInst);
	void adjust(HWND parent, HINSTANCE hInst);
	int minWidth();
	int minHeight();
	void save(wchar_t* filename);
	void open(HWND parent, wchar_t* filename);
};

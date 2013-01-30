
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

	FormDelegate* delegate;
public:
	Form();
	void setDelegate(FormDelegate *);
	FormDelegate* getDelegate();
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
	Value serializeForm();
	void deserializeForm(HWND parent, Value v);
	void save(wchar_t* filename);
	void open(HWND parent, wchar_t* filename);
};

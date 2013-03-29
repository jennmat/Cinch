
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
	bool hasDocument;
	FormDelegate* delegate;
public:
	Form();
	void setDelegate(FormDelegate *);
	FormDelegate* getDelegate();
	void NewDocument(string type);
	void LoadDocument(string _id, Object obj);
	void ReloadDocument();
	void RefreshValues();
	void SaveDocument(int changedFieldId);
	void addField(FormField* field);
	void addDetail(LPWSTR title);
	void removeAllDetailPages();
	void removeAllFields();
	Detail* getDetail();
	FormLayout* getLayout();
	void show(HWND parent, HINSTANCE hInst);
	void adjust(HWND parent, HINSTANCE hInst);
	int minWidth();
	int minHeight();
	Object serializeFormToObject(Object obj);
	void deserializeForm(HWND parent, Value v);
	void save(wchar_t* filename);
	void open(HWND parent, wchar_t* filename);
};

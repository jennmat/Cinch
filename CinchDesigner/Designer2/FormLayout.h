
#include "stdafx.h"
#include <stdlib.h>

using namespace std;

class FormLayout
{
private:
	int minimumHeight;
	vector<FormField*> fields;
	HWND detail;
public:
	FormLayout();
	void addField(FormField* field);
	int getFieldCount();
	FormField* getField(int);
	void removeField(int);
	void swapFields(int, int);
	void update(HWND parent);
	void show(HWND parent, HINSTANCE hInst, RECT displayArea);
	void removeAllFields();
	int minWidth();
	int minHeight();
};


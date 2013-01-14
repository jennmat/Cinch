
#include "stdafx.h"


#define MAX_CHILD_WINDOWS 500


class FormLayout
{
private:
	int i;
	int minimumHeight;
	FormField fields[MAX_CHILD_WINDOWS];
	HWND detail;
public:
	FormLayout();
	void addField(FormField field);\
	int getFieldCount();
	FormField getField(int);
	void update(HWND parent);
	void show(HWND parent, HINSTANCE hInst, RECT displayArea);
	int minWidth();
	int minHeight();
};


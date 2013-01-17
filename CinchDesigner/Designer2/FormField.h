
#include "stdafx.h"

using namespace JsonBox;

#define MAX_LABEL 100

class EditField;
class DatePickerField;

class FormField
{
private:
	HWND label;
	HWND control;
	char* controlType;
	const wchar_t* name;
public:
	void setLabel(HWND label);
	void setControl(HWND control);
	char* getControlType();
	const wchar_t* getName();
	HWND getLabel();
	HWND getControl();
	virtual void loadValue(Object obj) = 0;
	static FormField* createEditField(HWND parent, HINSTANCE hInst, const wchar_t * label);
	static FormField* createComboBox(HWND parent, HINSTANCE hInst, const wchar_t * label);
	static FormField* createDatePicker(HWND parent, HINSTANCE hInst, const wchar_t * label);
	static FormField* createCheckBox(HWND parent, HINSTANCE hInst, const wchar_t * label);
	static FormField* createRadioGroup(HWND parent, HINSTANCE hInst, const wchar_t * label);
	static FormField* createMultilineText(HWND parent, HINSTANCE hInst, const wchar_t * label);
};


class DatePickerField : public FormField {
public:
	void loadValue(Object obj);
};

class EditField : public FormField {
public:
	void loadValue(Object obj);
};
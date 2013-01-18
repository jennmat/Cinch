
#include "stdafx.h"

#define MAX_LABEL 100

class FormField
{
private:
	HWND label;
	HWND control;
	char* controlType;
	const wchar_t* name;
	

public:
	int controlChildId;

	void setLabel(HWND label);
	void setControl(HWND control);
	char* getControlType();
	const wchar_t* getName();
	HWND getLabel();
	HWND getControl();

	static FormField createEditField(HWND parent, HINSTANCE hInst, const wchar_t * label);
	static FormField createComboBox(HWND parent, HINSTANCE hInst, const wchar_t * label);
	static FormField createDatePicker(HWND parent, HINSTANCE hInst, const wchar_t * label);
	static FormField createCheckBox(HWND parent, HINSTANCE hInst, const wchar_t * label);
	static FormField createRadioGroup(HWND parent, HINSTANCE hInst, const wchar_t * label);
	static FormField createMultilineText(HWND parent, HINSTANCE hInst, const wchar_t * label);

	
};

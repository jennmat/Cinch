
#include "stdafx.h"
#include <CommCtrl.h>

using namespace JsonBox;

#define MAX_LABEL 100

class EditField;
class DatePickerField;

using namespace std;

class FormField
{
private:
	HWND label;
	HWND control;
	char* controlType;
	const wchar_t* name;
	Value config;

public:
	int controlChildId;

	void setLabel(HWND label);
	void setControl(HWND control);
	char* getControlType();
	const wchar_t* getName();
	HWND getLabel();
	HWND getControl();

	Value getConfig();

	virtual void loadValue(Object obj) = 0;
	virtual void clearValue() = 0;
	virtual Object storeValue(Object obj) = 0;
	virtual string serializeForJS() = 0;

	static FormField* createEditField(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label);
	static FormField* createNumberField(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label);
	static FormField* createComboBox(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label, Value config);
	static FormField* createDatePicker(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label);
	static FormField* createCheckBox(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label);
	static FormField* createRadioGroup(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label);
	static FormField* createYesNoField(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label);
	static FormField* createMultilineText(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label);
	static FormField* createReferenceField(HWND parent, HINSTANCE hInst, const wchar_t* name, const wchar_t * label, Value config);
};


class DatePickerField : public FormField {
public:
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
};

class EditField : public FormField {
public:
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
};


class NumberField : public FormField {
public:
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
};

#define YES_RADIO 130908
#define NO_RADIO 130909

class YesNoField : public FormField {
public:
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
};

class ReferenceField : public FormField {
public:
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
};


class ComboBoxField : public FormField {
public:
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
};
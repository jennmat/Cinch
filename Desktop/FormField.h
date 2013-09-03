
#include "stdafx.h"
#include <CommCtrl.h>

using namespace JsonBox;

#define MAX_LABEL 100

class EditField;
class DatePickerField;
class ViewAutocompleteSource;

using namespace std;

class FormField
{
private:
	HWND label;
	HWND control;
	char* controlType;
	string name;
protected:
	Value config;

public:
	virtual ~FormField();
	int controlChildId;

	void setLabel(HWND label);
	void setControl(HWND control);
	char* getControlType();
	string getName();
	HWND getLabel();
	HWND getControl();

	Value getConfig();

	virtual void loadValue(Object obj) = 0;
	virtual void clearValue() = 0;
	virtual Object storeValue(Object obj) = 0;
	virtual string serializeForJS() = 0;
	virtual string toString(Object obj);

	static FormField* createEditField(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare=false);
	static FormField* createAutocompletingEditField(HWND parent, HINSTANCE hInst, string enclosingType, string name, const wchar_t * label, bool bare=false);
	static FormField* createIdentifierField(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare=false);
	static FormField* createIntegerField(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare=false);
	static FormField* createDecimalField(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare=false);
	static FormField* createComboBox(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, string type, bool bare=false);
	static FormField* createDatePicker(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare=false);
	static FormField* createCheckBox(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare=false);
	static FormField* createRadioGroup(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare=false);
	static FormField* createYesNoField(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare=false);
	static FormField* createMultilineText(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, bool bare=false);
	static FormField* createReferenceField(HWND parent, HINSTANCE hInst, string name, const wchar_t * label, Value config, bool bare=false);
};


class DatePickerField : public FormField {
public:
	~DatePickerField();
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
};

class EditField : public FormField {
public:
	ViewAutocompleteSource* autoCompleteSource;
	EditField();
	~EditField();
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
	void setupAutocompleteValues();
};

class IdentifierField : public FormField {
public:
	~IdentifierField();
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
};


class DecimalField : public FormField {
public:
	~DecimalField();
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
	string toString(Object obj);
};


class IntegerField : public FormField {
public:
	~IntegerField();
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
	string toString(Object obj);
};

#define YES_RADIO 130908
#define NO_RADIO 130909

class YesNoField : public FormField {
public:
	~YesNoField();
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
};

class ReferenceField : public FormField {
public:
	~ReferenceField();
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
	string toString(Object obj);
	void setupValues();
};


class ComboBoxField : public FormField {
public:
	~ComboBoxField();
	void loadValue(Object obj);
	void clearValue();
	Object storeValue(Object obj);
	string serializeForJS();
};

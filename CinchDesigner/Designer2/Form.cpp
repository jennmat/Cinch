
#include "stdafx.h"
#include "JsonBox.h"
#include <stdio.h>

using namespace JsonBox;
using namespace std;

Form::Form(){
	detail.setForm(this);
	delegate = 0;
}

void Form::addField(FormField* field){
	layout.addField(field);
}

void Form::addDetail(LPWSTR title){
	detail.addDetailPage(title);
}

void Form::removeAllDetailPages(){
	detail.removeAllDetailPages();
}

int Form::minHeight()
{
	return 250;
}

int Form::minWidth()
{
	return layout.minWidth();
}

Detail* Form::getDetail(){
	return &detail;
}

void Form::show(HWND parent, HINSTANCE hInst){
	RECT window;
	GetWindowRect(parent, &window);

	int window_width = window.right - window.left;
	int window_height = window.bottom - window.top;

	RECT form;
	form.left = 0;
	form.top = 0;
	form.right = window_width;
	form.bottom = window_height - 275;

	RECT detailArea;
	detailArea.left = 0;
	detailArea.right = window_width;
	detailArea.top = window_height - 275;
	detailArea.bottom = window_height;


	layout.show(parent, hInst, form);
	detail.show(parent, hInst, detailArea);
}

void Form::adjust(HWND parent, HINSTANCE hInst){
//layout.show(parent, hInst);
}

//void Form::load(){

//	Value v;
//	v.loadFromFile(
//}

void Form::open(HWND parent, wchar_t* filename)
{
	Value v;
	char cfilename[200];
	size_t t;
	wcstombs_s(&t, cfilename, filename, 200);

	v.loadFromFile(std::string(cfilename));

	deserializeForm(parent, v);
}

void Form::deserializeForm(HWND parent, Value v){
	Object o = v.getObject();
	Array fields = o["fields"].getArray();

	for(unsigned int i=0; i<fields.size(); i++){
		Object field = fields[i].getObject();
		string label = field["name"].getString();
		string name = field["name"].getString();
		wstring wlabel = Designer::s2ws(label);
		wstring wname = Designer::s2ws(name);
		string type = field["type"].getString();

		wchar_t* wcname = new wchar_t[name.length()+sizeof(wchar_t)];
		memset(wcname, 0, name.length() + sizeof(wchar_t));
		wcscpy_s(wcname, name.length()+sizeof(wchar_t), wname.c_str());

		FormField* formField;

		if ( type.compare("DatePicker") == 0 ){
			formField = FormField::createDatePicker(parent, GetModuleHandle(0), wcname);
		} else if ( type.compare("Radio") == 0 ){
			formField = FormField::createRadioGroup(parent, GetModuleHandle(0), wcname);
		} else if ( type.compare("Number") == 0 ){
			formField = FormField::createNumberField(parent, GetModuleHandle(0), wcname);
		} else if ( type.compare("YesNo") == 0 ){
			formField = FormField::createYesNoField(parent, GetModuleHandle(0), wcname);
		} else {
			formField = FormField::createEditField(parent, GetModuleHandle(0), wcname);
		}

		addField(formField);
		int a = 1;
	}

	show(parent, GetModuleHandle(0));

	detail.deserializeUIElements(o);

}

Value Form::serializeForm(){
	Object o;
	
	Array fields;
	for(int i=0; i<layout.getFieldCount(); i++){
		FormField* fld = layout.getField(i);
		Object f;
		char label[80];
		wchar_t wlab[80];
		GetWindowText(fld->getLabel(), wlab, 80);
		size_t t;
		wcstombs_s(&t, label, wlab, 80);

		f["name"] = Value(label);
		f["type"] = Value(fld->getControlType());
		fields.push_back(f);
	}

	o["fields"] = fields;

	o["tabs"] = detail.serializeUIElements();

	Value v(o);
	
	return v;
}

void Form::save(wchar_t* filename){
	Value v = serializeForm();
	
	char cfilename[80];
	size_t t;

	wcstombs_s(&t, cfilename, filename, 80);
	v.writeToFile(cfilename);
}

void Form::NewDocument(){
	Object * obj = new Object();
	LoadDocument("", *obj);
}

void Form::setDelegate(FormDelegate *_delegate){
	delegate = _delegate;
}

FormDelegate* Form::getDelegate(){
	return delegate;
}

void Form::LoadDocument(string _id, Object _obj){
	id = _id;
	obj = _obj;
	for(int i=0; i<layout.getFieldCount(); i++){
		FormField* field = layout.getField(i);
		field->clearValue();
		field->loadValue(obj);
	}
	string nickname = obj["nickname"].getString();
	const wchar_t* nicknamew = Designer::s2ws(nickname).c_str();

	detail.LoadDocument(obj);
}

void Form::SaveDocument(int changedFieldId){
	for(int i=0; i<layout.getFieldCount(); i++){
		FormField* field = layout.getField(i);
		//if ( field->controlChildId == changedFieldId ){
			obj = field->storeValue(obj);
			
		//}
	}

	obj = detail.StoreValuesToDocument(changedFieldId, obj);
	obj = detail.StoreValuesToDocument(13, obj);
	Connection conn;
	
	Database db2 = conn.getDatabase("property");
	Document updatedDoc = db2.createDocument(Value(obj), id);
	Value v = updatedDoc.getData();

	LoadDocument(id, v.getObject());

}



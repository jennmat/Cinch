
#include "stdafx.h"
#include "JsonBox.h"
#include <stdio.h>

using namespace JsonBox;
using namespace std;

Form::Form(){

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

	Object o = v.getObject();
	Array fields = o["fields"].getArray();

	for(unsigned int i=0; i<fields.size(); i++){
		Object field = fields[i].getObject();
		string label = field["label"].getString();
		wstring wlabel = Designer::s2ws(label);
		string type = field["type"].getString();
		
		FormField* formField;

		if ( type.compare("DatePicker") == 0 ){
			formField = FormField::createDatePicker(parent, GetModuleHandle(0), wlabel.c_str());
		} else if ( type.compare("Radio") == 0 ){
			formField = FormField::createRadioGroup(parent, GetModuleHandle(0), wlabel.c_str());
		} else {
			formField = FormField::createEditField(parent, GetModuleHandle(0), wlabel.c_str());
		}

		addField(formField);
		int a = 1;
	}

	show(parent, GetModuleHandle(0));

	detail.load(o);

}

void Form::save(wchar_t* filename){
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

		f["label"] = Value(label);
		f["type"] = Value(fld->getControlType());
		fields.push_back(f);
	}

	o["fields"] = fields;

	o["tabs"] = detail.JsonFormat();

	JsonBox::Value v(o);
	char cfilename[80];
	size_t t;

	wcstombs_s(&t, cfilename, filename, 80);
	v.writeToFile(cfilename);
}


void Form::LoadDocument(Document& doc){
	Value v = doc.getData();
	Object obj = v.getObject();
	for(int i=0; i<layout.getFieldCount(); i++){
		FormField* field = layout.getField(i);
		field->loadValue(obj);
	}
	string nickname = obj["nickname"].getString();
	const wchar_t* nicknamew = Designer::s2ws(nickname).c_str();

}
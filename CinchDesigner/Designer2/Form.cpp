
#include "stdafx.h"
#include "JsonBox.h"
#include <stdio.h>

using namespace JsonBox;
using namespace std;

Form::Form(){

}

void Form::addField(FormField field){
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
		wstring wlabel = s2ws(label);
		string type = field["type"].getString();
		
		FormField formField;

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
		FormField fld = layout.getField(i);
		Object f;
		char label[80];
		wchar_t wlab[80];
		GetWindowText(fld.getLabel(), wlab, 80);
		size_t t;
		wcstombs_s(&t, label, wlab, 80);

		f["label"] = Value(label);
		f["type"] = Value(fld.getControlType());
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


void Form::LoadDocument(Database *_db, Document* doc){
	id = doc->getID();
	db = _db;
	value = doc->getData();
	Object obj = value.getObject();
	for(int i=0; i<layout.getFieldCount(); i++){
		FormField field = layout.getField(i);
		const wchar_t* name = field.getName();
		string n = ws2s(name);
		if ( obj[n.c_str()].isString() ){
			string val = obj[n.c_str()].getString();
			wstring valw = s2ws(val);
			LPCWSTR r = valw.c_str();
			SetWindowText(field.getControl(), r); 
		}

	}
	string nickname = obj["nickname"].getString();
	const wchar_t* nicknamew = s2ws(nickname).c_str();

	for(int i=0; i<detail.getDetailPageCount(); i++){
		if ( i == 2 ){
			HWND inspectionsDetail = detail.GetDetailPage(i);
			CinchGrid* gridcontrol = (CinchGrid *)GetWindowLong(inspectionsDetail, GWL_USERDATA);
			Array a = obj["inspections"].getArray();

			ArrayOfObjectsDelegate* d = new ArrayOfObjectsDelegate(obj["inspections"].getArray());
			gridcontrol->setDelegate(d);

			gridcontrol->reloadData();
			
		}
	}
}

void Form::SaveDocument(int changedFieldId){
	for(int i=0; i<layout.getFieldCount(); i++){
		FormField field = layout.getField(i);
		if ( field.controlChildId == changedFieldId ){
			LPWSTR str = new wchar_t[80];
			GetWindowText(field.getControl(), str, 80);
			OutputDebugStringW(str);
			Object obj = value.getObject();
			string key = ws2s(field.getName());
			string value = ws2s(str);
			obj[key.c_str()] = value;
			Connection conn;
			
			Database db2 = conn.getDatabase("property");
			db2.createDocument(Value(obj), id);

		}
	}
}
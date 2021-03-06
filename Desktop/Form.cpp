
#include "stdafx.h"
#include "JsonBox.h"
#include <stdio.h>

using namespace JsonBox;
using namespace std;

Form::Form(){
	detail.setForm(this);
	hasDocument = false;
	delegate = 0;

}

Form::~Form(){
	removeAllDetailPages();
	removeAllFields();
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

void Form::removeAllFields(){
	layout.removeAllFields();
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

FormLayout* Form::getLayout(){
	return &layout;
}

void Form::show(HWND parent, HINSTANCE hInst){
	RECT client;
	GetClientRect(parent, &client);

	
	RECT form;
	form.left = 0;
	form.top = 0;
	form.right = client.right;
	form.bottom = client.bottom - 275;

	RECT detailArea;
	detailArea.left = 0;
	detailArea.right = client.right;
	detailArea.top = client.bottom - 275;
	detailArea.bottom = client.bottom;


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
	
	removeAllDetailPages();
	removeAllFields();

	Object o = v.getObject();
	Array fields = o["fields"].getArray();
	string type = o["target_type"].getString();

	preloadTypeDefinitions();
	
	for(unsigned int i=0; i<fields.size(); i++){

		string id = fields[i].getString();
		
		FormField * formField = createFieldForType(parent, type, id, false);

		/*if ( type.compare(DATEPICKER) == 0 ){
			formField = FormField::createDatePicker(parent, GetModuleHandle(0), name, wclabel);
		} else if ( type.compare(RADIO) == 0 ){
			formField = FormField::createRadioGroup(parent, GetModuleHandle(0), name, wclabel);
		} else if ( type.compare(NUMBER) == 0 ){
			formField = FormField::createNumberField(parent, GetModuleHandle(0), name, wclabel);
		} else if ( type.compare(YESNO) == 0 ){
			formField = FormField::createYesNoField(parent, GetModuleHandle(0), name, wclabel);
		} else if ( type.compare(MULTILINE) == 0 ){
			formField = FormField::createMultilineText(parent, GetModuleHandle(0), name, wclabel);
		} else if ( type.compare(EDIT) == 0 ){
			formField = FormField::createEditField(parent, GetModuleHandle(0), name, wclabel);
		} else if ( type.compare(COMBO) == 0 ){
			formField = FormField::createComboBox(parent, GetModuleHandle(0), name, wclabel, config);
		} else if ( type.compare(REFERENCE) == 0 ){
			formField = FormField::createReferenceField(parent, GetModuleHandle(0), name, wclabel, config);
		} else {
			formField = FormField::createEditField(parent, GetModuleHandle(0), name, wclabel);
		}*/

		addField(formField);
		int a = 1;
	}

	show(parent, GetModuleHandle(0));

	detail.deserializeUIElements(o);

}

Object Form::serializeFormToObject(Object obj){
	
	Array fields;
	for(int i=0; i<layout.getFieldCount(); i++){
		FormField* fld = layout.getField(i);
		fields.push_back(fld->getName());
	}

	obj["fields"] = fields;

	obj["tabs"] = detail.serializeUIElements();
	obj["cinch_type"] = "template";
	return obj;
}

void Form::save(wchar_t* filename){
	Object o;
	o = serializeFormToObject(o);
	
	char cfilename[80];
	size_t t;
	Value v(o);
	wcstombs_s(&t, cfilename, filename, 80);
	v.writeToFile(cfilename);
}

void Form::NewDocument(string type){
	Object * obj = new Object();
	LoadDocument("", *obj, false);
}

void Form::setDelegate(FormDelegate *_delegate){
	delegate = _delegate;
}

FormDelegate* Form::getDelegate(){
	return delegate;
}

void Form::RefreshValues(){
	if ( hasDocument ){
		

		for(int i=0; i<layout.getFieldCount(); i++){
			FormField* field = layout.getField(i);
			field->clearValue();
			field->loadValue(obj);
		}
	
		detail.LoadDocument(obj);

	}
}

void Form::LoadDocument(string _id, Object _obj, bool skipRefresh){
	id = _id;
	obj = _obj;

	hasDocument = true;

	if ( !skipRefresh ){
		RefreshValues();
	}
}

void Form::SaveDocument(int changedFieldId){
	if ( hasDocument == false ) return; 
	
	for(int i=0; i<layout.getFieldCount(); i++){
		FormField* field = layout.getField(i);
		if ( field->controlChildId == changedFieldId ){
			obj = field->storeValue(obj);
		}
	}

	obj = detail.StoreValuesToDocument(changedFieldId, obj);
	
	
	Document updatedDoc = db.createDocument(Value(obj), id);

	Value v = updatedDoc.getData();
	
	LoadDocument(id, v.getObject(), true);

}



void Form::ReloadDocument(){
	Document d = db.getDocument(id);
	LoadDocument(id, d.getData().getObject(),  false);
}


#include "stdafx.h"

wstring s2ws(const string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    wstring r(buf);
    delete[] buf;
    return r;
}

std::string ws2s(const std::wstring& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0); 
    char* buf = new char[len];
    WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0); 
    std::string r(buf);
    delete[] buf;
    return r;
}

void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
     // Note that LONGLONG is a 64-bit value
     LONGLONG ll;

     ll = Int32x32To64(t, 10000000) + 116444736000000000;
     pft->dwLowDateTime = (DWORD)ll;
     pft->dwHighDateTime = ll >> 32;
}



string CreateUUID(){
	UUID uuid;
	UuidCreate(&uuid);
	char* buffer;
	UuidToStringA(&uuid, (RPC_CSTR*)&buffer);
	
	string rc = string(buffer);

	RpcStringFreeA((RPC_CSTR*)&buffer);

	return rc;
}


vector<string> collectAttributesSingle(Database& db, string field){
	vector<string> attributes;
	Object results = db.viewResults("all-attributes", "by-type", Value(field), Value(field), true);
	if ( results["rows"].isArray() ){
		Array rows = results["rows"].getArray();
		for(unsigned int i=0; i<rows.size(); i++){
			Object row = rows[i].getObject();
			Object doc = row["doc"].getObject();
			attributes.push_back(doc["_id"].getString());
		}
	}
	return attributes;
}



vector<string> collectAttributes(string field){
	;
	

	vector<string> attributes;

	string baseType = "";
	string type = field;

	Object o;

	vector<string> singleAttrs = collectAttributesSingle(db, type);
	attributes.insert(attributes.end(), singleAttrs.begin(), singleAttrs.end()); 


	do {
		Object o = db.getDocument(type).getData().getObject();
		baseType = o["type"].getString();
		type = baseType;
		
		vector<string> singleAttrs = collectAttributesSingle(db, type);
		attributes.insert(attributes.end(), singleAttrs.begin(), singleAttrs.end()); 

	} while ( type.compare(baseType) != 0 );
	

	return attributes;
}

map<string,string> baseTypes;
map<string,string> superTypes;
map<string, Object> typeDefinitions;
map<string,Object> defaultViewDefinitions;

string getSuperType(string type){
	if ( superTypes.find(type) != superTypes.end() ){
		return superTypes[type];
	}
	Object o = db.getDocument(type).getData().getObject();
	string superType = o["type"].getString();
	superTypes[type] = superType;
	return superType;
}

string getBaseType(string type){

	if ( baseTypes.find(type) != baseTypes.end() ){
		return baseTypes[type];
	}

	string baseType = getSuperType(type);
	while ( baseType.compare(type) != 0 ){
		type = baseType;
		baseType = getSuperType(baseType);
	}
	/*Object o = db.getDocument(type).getData().getObject();
	while ( o["type"].getString().compare(baseType) != 0 ){
		baseType = o["type"].getString();
		o = db.getDocument(type).getData().getObject();
	}*/

	baseTypes[type] = baseType;

	return baseType;
}

Object getTypeDefinition(string type){
	if ( typeDefinitions.find(type) != typeDefinitions.end() ){
		return typeDefinitions[type];
	}
	Object def = db.getDocument(type).getData().getObject();
	typeDefinitions[type] = def;
	return def;
}

Object getDefaultViewDefinition(string t){
	if ( defaultViewDefinitions.find(t) != defaultViewDefinitions.end() ){
		return defaultViewDefinitions[t];
	}

	Array rows;
	string type = t;
	do { 
		Object results = db.viewResults("all-default-view-definitions", "by-document-type", Value(type), Value(type), true);
		rows = results["rows"].getArray();
		type = getSuperType(type);
	} while ( rows.size() == 0 );

	if ( rows[0].isObject() ){
		Object result = rows[0].getObject();
		if ( result["doc"].isObject() ) {
			defaultViewDefinitions[t] = result["doc"].getObject();
		}
	}

	return defaultViewDefinitions[t];
}

FormField* createFieldForType(HWND parent, string enclosingType, string id, bool bare){
	Object field = getTypeDefinition(id);
		
	string label;
	if( field["label"].isString() ){
		label = field["label"].getString();
	} else {
		label = field["_id"].getString();
	}
	string name = field["_id"].getString();

	wstring wlabel =s2ws(label);
	string type = field["type"].getString();
	Value config;
	if ( field["config"].isObject() ){
		config = field["config"].getObject();
	}
		
	bool autocomplete = field["autocomplete"].getBoolean();

	/* Find the base type and the first default view */
	//Object defaultView;
	//bool foundDefaultView = false;
	string baseType= getBaseType(type);

	/*while ( o["type"].getString().compare(baseType) != 0 ){
		baseType = o["type"].getString();
		o = db.getDocument(type).getData().getObject();

		Object results = db.viewResults("all-default-view-definitions", "by-document-type", Value(type), Value(type), true);
		Array rows = results["rows"].getArray();
		if ( rows.size() > 0 ){
			defaultView = rows[0].getObject();
			foundDefaultView = true;
		}
	}*/

	wchar_t* wclabel = new wchar_t[label.length()+sizeof(wchar_t)];
	memset(wclabel, 0, label.length() + sizeof(wchar_t));
	wcscpy_s(wclabel, label.length()+sizeof(wchar_t), wlabel.c_str());

	FormField* formField;

	if ( baseType.compare(STRING) == 0){
		if ( autocomplete ) {
			formField = FormField::createAutocompletingEditField(parent, GetModuleHandle(0), enclosingType, name, wclabel, bare);
		} else {
			formField = FormField::createEditField(parent, GetModuleHandle(0), name, wclabel, bare);
		}
	} else if (baseType.compare(ID) == 0 ){
		formField = FormField::createIdentifierField(parent, GetModuleHandle(0), name, wclabel, bare);
	} else if ( baseType.compare(MULTILINE) == 0 ){
		formField = FormField::createMultilineText(parent, GetModuleHandle(0), name, wclabel, bare);
	} else if ( baseType.compare(DATETYPE) == 0 ) {
		formField = FormField::createDatePicker(parent, GetModuleHandle(0), name, wclabel, bare);
	} else if ( baseType.compare(CODEDVALUE) == 0 ){
		formField = FormField::createComboBox(parent, GetModuleHandle(0), name, wclabel, name, bare);
	} else if ( baseType.compare(NUMBERTYPE) == 0 ){
		formField = FormField::createNumberField(parent, GetModuleHandle(0), name, wclabel, bare);
	} else if ( baseType.compare(BOOLEAN) == 0 ){
		formField = FormField::createYesNoField(parent, GetModuleHandle(0), name, wclabel, bare);
	} else if ( baseType.compare(DOCUMENT) == 0 ){
	
		Object conf;
		Object doc = getDefaultViewDefinition(name);
		string design = doc["design_name"].getString();
		string view = doc["view_name"].getString();

		Object pick;
		pick["design"] = design;
		pick["view"] = view;
		conf["pick_from"] = pick;
	

		formField = FormField::createReferenceField(parent, GetModuleHandle(0), name, wclabel, Value(conf), bare);
		
	} else {
		formField = FormField::createEditField(parent, GetModuleHandle(0), name, wclabel);
	}
	return formField;
}

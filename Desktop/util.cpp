

#include "stdafx.h"


wstring s2ws(const string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, buf, len);
    wstring r(buf);
    delete[] buf;
    return r;
}

std::string ws2s(const std::wstring& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), slength, 0, 0, 0, 0); 
    char* buf = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, s.c_str(), slength, buf, len, 0, 0); 
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
	
	QueryOptions options;
	options.startKey = JsonBox::Value(field);
	options.endKey = JsonBox::Value(field);
	options.includeDocs = true;
	Object results = db.viewResults("all-attributes", "by-type", options);

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

string getSuperType(const string& type){
	if ( superTypes.find(type) != superTypes.end() ){
		return superTypes[type];
	}
	Object o = db.getDocument(type).getData().getObject();
	string superType = o["type"].getString();
	superTypes[type] = superType;
	return superType;
}

string getBaseType(const string& type){

	if ( baseTypes.find(type) != baseTypes.end() ){
		return baseTypes[type];
	}
	string o = type;
	string baseType = getSuperType(type);
	while ( baseType.compare(o) != 0 ){
		o = baseType;
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

string convertToString(double val){
	int decimal, sign;
	char* buffer = new char[_CVTBUFSIZE];
	int precision = 2;
	_fcvt_s(buffer, _CVTBUFSIZE, val, precision, &decimal, &sign);
	string s = string(buffer);
	s.insert(decimal, ".");
	delete buffer;
	return s;
}

string convertToString(int val){
	char* buf = new char[_CVTBUFSIZE];
	memset(buf, 0, _CVTBUFSIZE);
	_itoa_s(val, buf, _CVTBUFSIZE, 10);
	string s = string(buf);
	delete buf;
	return s;
}

string serializeForDisplay(const Value& v, const string& type){
	string base = getBaseType(type);
	if( base.compare("string") == 0 ){
		return v.getString();
	} else if ( base.compare("multiline") == 0 ){
		return v.getString();
	} else if ( base.compare("date") == 0 ){
		string date = v.getString();
		return date;
	} else if( base.compare("integer") == 0 ){
		if( v.isInteger() ) return convertToString(v.getInt());
	} else if ( base.compare("decimal") == 0 ){
		if ( v.isDouble() ) return convertToString(v.getDouble());
	} else if ( base.compare("coded_value") == 0 ){
		Object def = typeDefinitions[type];
		if ( def["allowed_codes"].isArray() ){
			Array codes = def["allowed_codes"].getArray();
			for(int i=0; i<codes.size(); i++){
				Object code = codes[i].getObject();
				if ( code["value"].getString().compare(v.getString()) == 0 ){
					return code["label"].getString();
				}
			}
		}
	} else if ( base.compare("document") == 0 ){
		/* This is a document reference */
		Object referencedDoc = db.getDocument(v.getString()).getData().getObject();
		Object def = typeDefinitions[type];
		
		stringstream script;
		script << "var obj = " << referencedDoc << ";";
		script << def["functions"].getString();
		script << "toString(obj);";


		return EvaluateScript(script.str());
	} else if ( base.compare("reference") == 0 ){
		if ( v.getString().length() > 0){
			Object referencedDoc = db.getDocument(v.getString()).getData().getObject();
		
			Object def = typeDefinitions[type];
			string references = def["referenced_type"].getString();
			Object referencedDef = typeDefinitions[references];

			stringstream script;
			script << "var obj = " << referencedDoc << ";";
			script << referencedDef["functions"].getString();
			script << "toString(obj);";


			return EvaluateScript(script.str());
		}
	}
	return "";
}

Object getTypeDefinition(const string& type){
	if ( typeDefinitions.find(type) != typeDefinitions.end() ){
		return typeDefinitions[type];
	}
	Object def = db.getDocument(type).getData().getObject();
	typeDefinitions[type] = def;
	return def;
}

void preloadTypeDefinitions(){
	if ( typeDefinitions.size() > 0 ){
		return;
	}

	QueryOptions options;
	options.includeDocs = true;
	Object results = db.viewResults("all-data-definitions", "by-id", options);

	if ( results["rows"].isArray() ){
		Array rows = results["rows"].getArray();
		for(unsigned int i=0; i<rows.size(); i++){
			Object row = rows[i].getObject();
			Object doc = row["doc"].getObject();
			string type = doc["_id"].getString();
			string super = doc["type"].getString();
			typeDefinitions[type] = doc;

			superTypes[type] = super;
		}
	}
	/*
	for(unsigned int i=0; i<definitions.size(); i++){
		Object o = definitions[i].getObject();
		typeDefinitions[o["_id"].getString()] = o;
	}
	*/
}

Object getDefaultViewDefinition(const string& t){
	if ( defaultViewDefinitions.find(t) != defaultViewDefinitions.end() ){
		return defaultViewDefinitions[t];
	}

	Array rows;
	string type = t;
	do { 
		
		QueryOptions options;
		options.startKey = JsonBox::Value(type);
		options.endKey = JsonBox::Value(type);
		Object results = db.viewResults("all-default-view-definitions", "by-document-type", options);

		rows = results["rows"].getArray();
		type = getSuperType(type);
	} while ( rows.size() == 0 && type.compare("document") != 0 );

	if ( rows.size() > 0 && rows[0].isObject() ){
		Object result = rows[0].getObject();
		if ( result["value"].isObject() ) {
			defaultViewDefinitions[t] = result["value"].getObject();
		}
	}

	if ( defaultViewDefinitions.find(t) != defaultViewDefinitions.end() ){
		return defaultViewDefinitions[t];
	}
	return Object();
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
	JsonBox::Value config;
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

	if ( baseType.compare(CINCH_STRING) == 0){
		if ( autocomplete ) {
			formField = FormField::createAutocompletingEditField(parent, GetModuleHandle(0), enclosingType, name, wclabel, bare);
		} else {
			formField = FormField::createEditField(parent, GetModuleHandle(0), name, wclabel, bare);
		}
	} else if (baseType.compare(CINCH_ID) == 0 ){
		formField = FormField::createIdentifierField(parent, GetModuleHandle(0), name, wclabel, bare);
	} else if ( baseType.compare(MULTILINE) == 0 ){
		formField = FormField::createMultilineText(parent, GetModuleHandle(0), name, wclabel, bare);
	} else if ( baseType.compare(DATETYPE) == 0 ) {
		formField = FormField::createDatePicker(parent, GetModuleHandle(0), name, wclabel, bare);
	} else if ( baseType.compare(CODEDVALUE) == 0 ){
		formField = FormField::createComboBox(parent, GetModuleHandle(0), name, wclabel, name, bare);
	} else if ( baseType.compare(INTEGERTYPE) == 0 ){
		formField = FormField::createIntegerField(parent, GetModuleHandle(0), name, wclabel, bare);
	} else if ( baseType.compare(DECIMALTYPE) == 0 ){
		formField = FormField::createDecimalField(parent, GetModuleHandle(0), name, wclabel, bare);
	} else if ( baseType.compare(BOOLEAN) == 0 ){
		formField = FormField::createYesNoField(parent, GetModuleHandle(0), name, wclabel, bare);
	} else if ( baseType.compare(CINCH_REFERENCE) == 0 || baseType.compare(DOCUMENT) == 0 ){
	
		string type;
		if ( field["referenced_type"].isString() ){
			type = field["referenced_type"].getString();
		} else {
			type = id;
		}
		Object conf;
		
		Object doc = getDefaultViewDefinition(type);
		string design = doc["design"].getString();
		string view = doc["view"].getString();

		Object pick;
		pick["design"] = design;
		pick["view"] = view;
		conf["pick_from"] = pick;
	
		formField = FormField::createReferenceField(parent, GetModuleHandle(0), name, wclabel, JsonBox::Value(conf), bare);
		
	} else {
		formField = FormField::createEditField(parent, GetModuleHandle(0), name, wclabel);
	}

	delete wclabel;

	return formField;
}

Array getAllConcreteDocumentTypes(){

	Array docTypes;

	Object r = db.viewResults("all-data-definitions", "by-label");
	Array rows = r["rows"].getArray();
	unsigned int i = 0;
	for(; i<rows.size(); i++){
		Object row = rows[i].getObject();
				
		const string& id = row["id"].getString();
		if ( getBaseType(id) == "document")  {
			Document doc = db.getDocument(row["id"].getString());
			Object def = doc.getData().getObject();
			if ( !def["abstract"].isBoolean() || def["abstract"].getBoolean() == false ){
				docTypes.push_back(doc.getData().getObject());
			}
		}
	}
	return docTypes;
}

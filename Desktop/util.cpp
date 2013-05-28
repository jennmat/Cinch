

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
	Connection conn;
	Database db = conn.getDatabase(DATABASE);

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

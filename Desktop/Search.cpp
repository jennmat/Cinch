
#include "stdafx.h"

void createIndexingDesignDoc(const Array& fields, const string& revision = ""){
	Object doc;
	Object indexes;
	Object all;
	stringstream func;
	func << "function(doc){";
	for(int i=0; i<fields.size(); i++){
		string field = fields[i].getString();
		func << "if ( '" << field << "' in doc ) index(\"default\", doc." << field << ");";
	}
	func << "}";
	all["index"] = func.str();
	indexes["all"] = all;
	doc["indexes"] = indexes;
	doc["fields_indexed"] = fields;
	doc["_rev"] = revision;
	db.createDocument(Value(doc), "_design/fulltext/default");
}

void SetupSearching(){
	Object results = db.viewResults("all-searchable-types", "by-id");
	if ( results["rows"].isArray()){
		Array rows = results["rows"].getArray();
		Array fields;

		for(int i=0; i<rows.size(); i++){
			Object row = rows[i].getObject();
			fields.push_back(row["key"].getString());
		}

		/* Find the design document responsible for setting up the default index */
		try {
			Document doc = db.getDocument("_design/fulltext/default");
			Object data = doc.getData().getObject();
			Array previouslyIndexedFields = data["fields_indexed"].getArray();
			if ( previouslyIndexedFields != fields ){
				createIndexingDesignDoc(fields, doc.getRevision());
			}
		}catch(DocumentNotFoundException){
			createIndexingDesignDoc(fields);
		}
	}
}

void Search(const string& term){

}
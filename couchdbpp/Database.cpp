/**
 * Copyright 2009 Tragicphantom Productions
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/
#include <sstream>

#include "couchdb/Database.hpp"
#include "couchdb/Exception.hpp"

#include <JsonBox.h>

using namespace std;
using namespace CouchDB;
using namespace JsonBox;

Database::Database(Communication &_comm, const string &_name)
   : comm(_comm)
   , name(_name)
{
}

Database::Database(const Database &db)
   : comm(db.comm)
   , name(db.name)
{
}

Database::~Database(){
}

Database& Database::operator=(Database &db){
   name = db.getName();
   return *this;
}

const string& Database::getName() const{
   return name;
}

vector<Document> Database::documentsVectorFromValue(const Value& var){
	Object  obj = var.getObject();

   int numRows = obj["total_rows"].getInt();

   vector<Document> docs;

   if(numRows > 0){
	   Array rows = obj["rows"].getArray();

      Array::iterator        row     = rows.begin();
      const Array::iterator &row_end = rows.end();
      for(; row != row_end; ++row){
		  Object docObj = (*row).getObject();
		  Object values = docObj["value"].getObject();

         Document doc(comm, Object(), name,
			 docObj["id"].getString(),
			 docObj["key"].getString(),
			 values["rev"].getString());

         docs.push_back(doc);
      }
   }

   return docs;
}


Object Database::listViews(){
	stringstream s;
	s << "/" << name << "/_all_docs";
	s << "?startkey=%22_design%2f%22&endkey=%22_design0%22&include_docs=true";
	
	Value var = comm.getData(s.str());
	return var.getObject();
}



Object Database::viewResults(const string& design, const string& view, QueryOptions options){
	stringstream url;
	url << "/" << name << "/_design/";
	url << design;
	url << "/_view/";
	url << view;
	stringstream params;
	params << "?";
	if ( options.limit != UINT_MAX ){
		params << "limit=" << options.limit << "&";
	}
	params << "descending=" << (options.descending ? "true" : "false") << "&";
	params << "skip=";
	params << options.skip << "&";
	
	if ( options.startKey != NULL ){
		params << "startkey=";
		stringstream key;
		key << "\"" << options.startKey.getString() << "\"";
		char* escapedKey = curl_easy_escape(comm.curl, key.str().c_str(), 0);
		params << escapedKey;
		params << "&";
		curl_free(escapedKey);
	}

	if ( options.startKeyDocId.length() > 0 ){
		params << "startkey_docid=";
		char* escaped = curl_easy_escape(comm.curl, options.startKeyDocId.c_str(), 0);
		params << escaped;
		params << "&";
		curl_free(escaped);
	}

	if ( options.endKey != NULL ){
		params << "endkey=";
		stringstream key;
		key << "\"" << options.endKey.getString() << "\"";
		char* escaped = curl_easy_escape(comm.curl, key.str().c_str(), 0);
		params << escaped;
		params << "&";
		curl_free(escaped);
	}

	if ( options.endKeyDocId.length() > 0 ){
		params << "endkey_docid=";
		char* escaped = curl_easy_escape(comm.curl, options.endKeyDocId.c_str(), 0);
		params << escaped;
		params << "&";
		curl_free(escaped);
	}

	if ( options.includeDocs == true ){
		params << "include_docs=true&";
	}
	if ( options.reduce == true ){
		params << "reduce=true&";
		params << "group_level=" << options.group_level << "&";
	} else {
		params << "reduce=false&";
	}

	string p = params.str();
	p = p.substr(0, p.length()-1);
	url << p;
	Value var = comm.getData(url.str());

	return var.getObject();
}

vector<Document> Database::listDocuments(){
   Value var = comm.getData("/" + name + "/_all_docs");
   return documentsVectorFromValue(var);
}

Document Database::getDocument(const string &id, const string &rev){
   char * escaped = curl_easy_escape(comm.curl, id.c_str(), id.size());
   string url = "/" + name + "/" + escaped;
   if(rev.size() > 0)
      url += "?rev=" + rev;

   Value var = comm.getData(url);
   Object  obj = var.getObject();

   if(obj.find("error") != obj.end())
	   throw Exception("Document " + id + " (v" + rev + ") not found: " + obj["error"].getString());

   curl_free(escaped);
   Document doc(comm,  obj, name,
				obj["_id"].getString(),
                "", // no key returned here
				obj["_rev"].getString()
               );

   return doc;
}


Array Database::getDocuments(const Array& ids){
   string url = "/" + name + "/_all_docs?include_docs=true";
   Communication::HeaderMap headers;
   stringstream data;
   Object o;
   o["keys"] = ids;
   data << o;
   string d = data.str();
   Value var = comm.getData(url, headers, "POST", d);
   Object obj = var.getObject();
   Array result;
   if ( obj["rows"].isArray() ){
	   Array rows = obj["rows"].getArray();
	   for(unsigned int i=0; i<rows.size(); i++){
		   Object row = rows[i].getObject();
		   if ( row["doc"].isObject() ){
			   result.push_back(row["doc"].getObject());
		   }
	   }
   }
   return result;
}

static string createJSON(const Value &data){
   ostringstream ostr;
   ostr << data;
   return ostr.str();
}

Document Database::createDocument(const Value &data, const string &id){
   vector<Attachment> attachments;
   return createDocument(data, attachments, id);
}

Document Database::createDocument(Value data,
                                  vector<Attachment> attachments,
                                  const string &id){
   if(attachments.size() > 0){
      Object attachmentObj;

      vector<Attachment>::iterator attachment = attachments.begin();
      const vector<Attachment>::iterator &attachmentEnd = attachments.end();
      for(; attachment != attachmentEnd; ++attachment){
         Object attachmentData;
         attachmentData["content_type"] = attachment->getContentType();
         attachmentData["data"        ] = attachment->getData();
         attachmentObj[attachment->getID()] = attachmentData;
      }

	  Object obj = data.getObject();
	  obj["_attachments"] = attachmentObj;
   }

   string json = createJSON(data);
   char* escapedId = curl_easy_escape(comm.curl, id.c_str(), id.length());

   Value var;
   if(id.size() > 0)
      var = comm.getData("/" + name + "/" + escapedId, "PUT", json);
   else
      var = comm.getData("/" + name + "/", "POST", json);

   Object  obj = var.getObject();

   if(obj.find("error") != obj.end())
	   throw Exception("Document could not be created: " + obj["error"].getString());

   Document doc(comm, obj, name,
				obj["id"].getString(),
                "", // no key returned here
				obj["rev"].getString()
               );

   return doc;
}


void Database::listenForChanges(void (*changesArrivedFunc)()){
	comm.readChangesFeed(name, changesArrivedFunc);
}

void Database::stopListening(){
	comm.listenFlag = false;
}

void Database::startReplication(const string& destinationHost, const string& destinationDatabase, const string& username, const string& password)
{
	Object obj;
	obj["source"] = name;
	stringstream target;
	target << "https://" << username << ":" << password << "@" << destinationHost << "/" << destinationDatabase;
	obj["target"] = target.str();
	obj["continuous"] = Value(true);
	obj["create_target"] = Value(true);

	string data = createJSON(Value(obj));
    Value response = comm.getData("/_replicate", "POST", data);

	Object pull;
	stringstream source;
	source << "https://" << username << ":" << password << "@" << destinationHost << "/" << destinationDatabase;
	pull["target"] = name;
	pull["continuous"] = Value(true);
	pull["source"] = source.str();

	data = createJSON(Value(pull));
    response = comm.getData("/_replicate", "POST", data);

	stringstream r;
	r << response;

	string q = r.str();

	int a  =1;
}

Communication& Database::getCommunication(){
	return comm;
}


ostream& operator<<(ostream &out, const CouchDB::Database &db){
   return out << "<Database: " << db.getName() << ">";
}

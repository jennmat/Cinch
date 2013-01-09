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

         Document doc(comm, name,
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

Object Database::viewResults(const string& design, const string& view, Value& startKey, int limit=25){
	stringstream s;
	s << "/" << name << "/_design/";
	s << design;
	s << "/_view/";
	s << view;
	s << "?limit=";
	s << limit;

	Value var = comm.getData(s.str());

	return var.getObject();
}

Object Database::viewResulsFromStartDocId(const string& design, const string& view, Value& startKey, const string& startKeyDocId, int limit)
{
	stringstream s;
	s << "/" << name << "/_design/";
	s << design;
	s << "/_view/";
	s << view;
	s << "?startkey=";
	stringstream key;
	key << "\"" << startKey.getString() << "\"";
	char* escapedKey = curl_easy_escape(comm.curl, key.str().c_str(), 0);
	s << escapedKey;
	s << "&startkey_docid=";
	s << startKeyDocId;
	s << "&limit=";
	s << limit;

	Value var = comm.getData(s.str());

	return var.getObject();


}

vector<Document> Database::listDocuments(){
   Value var = comm.getData("/" + name + "/_all_docs");
   return documentsVectorFromValue(var);
}

Document Database::getDocument(const string &id, const string &rev){
   string url = "/" + name + "/" + id;
   if(rev.size() > 0)
      url += "?rev=" + rev;

   Value var = comm.getData(url);
   Object  obj = var.getObject();

   if(obj.find("error") != obj.end())
	   throw Exception("Document " + id + " (v" + rev + ") not found: " + obj["error"].getString());

   Document doc(comm, name,
				obj["id"].getString(),
                "", // no key returned here
				obj["_rev"].getString()
               );

   return doc;
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

   Value var;
   if(id.size() > 0)
      var = comm.getData("/" + name + "/" + id, "PUT", json);
   else
      var = comm.getData("/" + name + "/", "POST", json);

   Object  obj = var.getObject();

   if(obj.find("error") != obj.end())
	   throw Exception("Document could not be created: " + obj["error"].getString());

   Document doc(comm, name,
				obj["id"].getString(),
                "", // no key returned here
				obj["rev"].getString()
               );

   return doc;
}

ostream& operator<<(ostream &out, const CouchDB::Database &db){
   return out << "<Database: " << db.getName() << ">";
}

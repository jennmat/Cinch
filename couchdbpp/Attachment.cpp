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
#include "couchdb/Attachment.hpp"
#include "couchdb/Exception.hpp"
#include "JsonBox.h"

#include <curl/curl.h>

using namespace JsonBox;

using namespace std;
using namespace CouchDB;

Attachment::Attachment(Communication &_comm, const string &_db,
                       const string &_document, const string &_id,
                       const string &_revision, const string &_contentType)
   : comm(_comm)
   , db(_db)
   , document(_document)
   , id(_id)
   , revision(_revision)
   , contentType(_contentType)
{
}

Attachment::Attachment(const Attachment &attachment)
   : comm(attachment.comm)
   , db(attachment.db)
   , document(attachment.document)
   , id(attachment.id)
   , revision(attachment.revision)
{
}

Attachment::~Attachment(){
}

Attachment& Attachment::operator=(Attachment &attachment){
   comm     = attachment.comm;
   db       = attachment.db;
   document = attachment.document;
   id       = attachment.id;
   revision = attachment.revision;

   return *this;
}

const string& Attachment::getID() const{
   return id;
}

const string& Attachment::getRevision() const{
   return revision;
}

const string& Attachment::getContentType() const{
   return contentType;
}

string Attachment::getContentMD5(){
	char * ed = curl_easy_escape(comm.curl, document.c_str(), document.length());
	char * eid = curl_easy_escape(comm.curl, id.c_str(), id.length());
	string url = "/" + db + "/" + ed + "/" + eid;
    if(revision.size() > 0)
		url += "?rev=" + revision;
    
	string headers = comm.getHead(url);

	int pos = headers.find("Content-MD5: ");
	if ( pos >= 0 ){
		pos += 13;
		int end = headers.find("\n", pos);
		return headers.substr(pos, end-pos-1);
	}

	return "";
	
}



string Attachment::getData(){
   string data;

   if(rawData.size() > 0)
      data = rawData;
   else{
      string url = "/" + db + "/" + document + "/" + id;
      if(revision.size() > 0)
         url += "?rev=" + revision;
      data = comm.getRawData(url);

      if(data.size() > 0 && data[0] == '{'){
         // check to make sure we did not receive an error
		 Value val = comm.getData(url);
		 if ( val.isObject() ){
			Object obj = val.getObject();
		
			if(obj.find("error") != obj.end() && obj.find("reason") != obj.end())
				 throw Exception("Could not retrieve data for attachment '" + id + "': " + obj["reason"].getString());
		 }
      }
   }

   return data;
}


void Attachment::saveToDirectory(string directory){
	char* escaped = curl_easy_escape(comm.curl, id.c_str(), id.length());
	string url = "/" + db + "/" + document + "/" + string(escaped);
	if(revision.size() > 0)
		url += "?rev=" + revision;
	comm.saveRawData(url, directory + "\\" + id);
}

ostream& operator<<(ostream &out, const CouchDB::Attachment &attachment){
   return out << "{id: " << attachment.getID()
              << ", rev: " << attachment.getRevision()
              << ", content-type: " << attachment.getContentType()
              << "}";
}



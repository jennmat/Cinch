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
#include "couchdb/Document.hpp"
#include "couchdb/Exception.hpp"

using namespace std;
using namespace CouchDB;

Document::Document(Communication &_comm, const Object& data, const string &_db, const string &_id,
                   const string &_key, const string &_rev)
   : comm(_comm)
   , data(data)
   , db(_db)
   , id(_id)
   , key(_key)
   , revision(_rev)
{
}

Document::Document(const Document &doc)
   : comm(doc.comm)
   , data(doc.data)
   , db(doc.db)
   , id(doc.id)
   , key(doc.key)
   , revision(doc.revision)
{

}

Document::~Document(){
}

Document& Document::operator=(Document& doc){
   db       = doc.getDatabase();
   id       = doc.getID();
   key      = doc.getKey();
   revision = doc.getRevision();
   data     = doc.getData().getObject();
   return *this;
}

bool Document::operator==(const Document &doc){
   return (id == doc.getID());
}

Communication& Document::getCommunication(){
   return comm;
}

const string& Document::getDatabase() const{
   return db;
}

const string& Document::getID() const{
   return id;
}

const string& Document::getKey() const{
   return key;
}

const string& Document::getRevision() const{
   return revision;
}


string Document::getURL(bool withRevision) const{
	char* escaped = curl_easy_escape(comm.curl, getID().c_str(), getID().length());
	string url = "/" + getDatabase() + "/" + escaped;
	if(withRevision && revision.size() > 0)
		url += "?rev=" + revision;	

	curl_free(escaped);
	return url;
}

vector<Revision> Document::getAllRevisions(){
   vector<Revision> revisions;

   Value var = comm.getData(getURL(false) + "?revs_info=true");
   Object  obj = var.getObject();

   Array revInfo = obj["_revs_info"].getArray();

   Array::iterator        revInfoItr = revInfo.begin();
   const Array::iterator &revInfoEnd = revInfo.end();
   for(; revInfoItr != revInfoEnd; ++revInfoItr){
	   Object revObj    =  (*revInfoItr).getObject();
	   revisions.push_back(Revision(revObj["rev"].getString(),
		   revObj["status"].getString()));
   }

   return revisions;
}

Value Document::getData(){
	if( data["_id"].isString() ){
		return data;
	}
   Value var = comm.getData(getURL(false));
   Object  obj = var.getObject();

   if(obj.find("_id") == obj.end() && obj.find("_rev") == obj.end() &&
      obj.find("error") != obj.end() && obj.find("reason") != obj.end())
	  throw Exception("Document '" + getID() + "' not found: " + obj["reason"].getString());

   return var;
}

bool Document::addAttachment(const string &attachmentId,
                             const string &contentType,
                             const string &data){

	string url = getURL(false) + "/" + attachmentId;
	if(revision.size() > 0)
		url += "?rev=" + revision;

	Communication::HeaderMap headers;
	headers["Content-Type"] = contentType;

	Value var = comm.getData(url, headers, "PUT", data);
	Object  obj = var.getObject();

	if(obj.find("error") != obj.end() && obj.find("reason") != obj.end())
		throw Exception("Could not create attachment '" + attachmentId + "': " + obj["reason"].getString());

	revision = obj["rev"].getString();

	return obj["ok"].getBoolean();
}

bool Document::updateAttachmentFromFile(std::string name, std::string filename) {
	char* escaped = curl_easy_escape(comm.curl, name.c_str(), name.length());
	string url = getURL(false) + "/" + escaped;
	if(revision.size() > 0)
		url += "?rev=" + revision;
	
	Communication::HeaderMap headers;
	comm.uploadData(url, headers, filename);
	return true;
}

bool Document::addAttachmentFromFile(std::string name, std::string contentType, std::string filename) {
	char* escaped = curl_easy_escape(comm.curl, name.c_str(), name.length());
	string url = getURL(false) + "/" + escaped;
	if(revision.size() > 0)
		url += "?rev=" + revision;
	
	Communication::HeaderMap headers;
	comm.uploadData(url, headers, filename);
	return true;
}




Attachment Document::getAttachment(const string &attachmentId){
	Object data = getData().getObject();

   if(data.find("_attachments") == data.end())
      throw AttachmentNotFoundException("Attachment not found");

   Object attachments = data["_attachments"].getObject();
   if(attachments.find(attachmentId) == attachments.end())
	   throw AttachmentNotFoundException("Attachment not found");

   Object attachment = attachments[attachmentId].getObject();

   return Attachment(comm, db, id, attachmentId, "", attachment["content_type"].getString());
}

vector<Attachment> Document::getAllAttachments(){
	Object data = getData().getObject();

   if(data.find("_attachments") == data.end())
      throw Exception("No attachments");

   vector<Attachment> vAttachments;

   Object attachments = data["_attachments"].getObject();

   Object::iterator attachmentItr = attachments.begin();
   const Object::iterator &attachmentEnd = attachments.end();
   for(; attachmentItr != attachmentEnd; ++attachmentItr){
      const string &attachmentId = attachmentItr->first;
	  Object attachment = (attachmentItr->second).getObject();

	  vAttachments.push_back(Attachment(comm, db, id, attachmentId, "", attachment["content_type"].getString()));
   }

   return vAttachments;
}

bool Document::removeAttachment(const string &attachmentId){
   string url = getURL(false) + "/" + attachmentId;
   if(revision.size() > 0)
      url += "?rev=" + revision;

   Value var = comm.getData(url, "DELETE");
   Object  obj = var.getObject();

   if(obj.find("error") != obj.end() && obj.find("reason") != obj.end())
	   throw Exception("Could not delete attachment '" + attachmentId + "': " + obj["reason"].getString());

   revision = obj["rev"].getString();

   return obj["ok"].getBoolean();
}

bool Document::remove(){
   Value var = comm.getData(getURL(true), "DELETE");
   Object  obj = var.getObject();

   return obj["ok"].getBoolean();
}

ostream& operator<<(ostream &out, const CouchDB::Document &doc){
   return out << "{id: " << doc.getID() << ", key: " << doc.getKey() << ", rev: " << doc.getRevision() << "}";
}

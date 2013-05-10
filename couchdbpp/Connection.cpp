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
#include <iostream>

#include "couchdb/Connection.hpp"

using namespace std;
using namespace CouchDB;

Connection::Connection(){
   getInfo();
}

Connection::Connection(const string &url) : comm(url){
   //getInfo();
}

void Connection::getInfo(){
   //Value val = comm.getData("");
   //Object obj = val.getObject();

   //couchDBVersion = obj["version"].getString();
}

Connection::~Connection(){
}

string Connection::getCouchDBVersion() const{
   return couchDBVersion;
}

vector<string> Connection::listDatabases(){
   Value val = comm.getData("/_all_dbs");
   Array   arr = val.getArray();

   vector<string> dbs;

   Array::iterator        db     = arr.begin();
   const Array::iterator &db_end = arr.end();
   for(; db != db_end; ++db)
      dbs.push_back((*db).getString());

   return dbs;
}

Database Connection::getDatabase(const string &db){
   return Database(comm, db);
}

bool Connection::createDatabase(const string &db){
   Value val = comm.getData("/" + db, "PUT");
   Object  obj = val.getObject();

   if(obj.find("error") != obj.end())
	   throw Exception("Unable to create database '" + db + "': " + obj["reason"].getString());

   return obj["ok"].getBoolean();
}

bool Connection::deleteDatabase(const string &db){
   Value val = comm.getData("/" + db, "DELETE");
   Object obj = val.getObject();

   if(obj.find("error") != obj.end())
	   throw Exception("Unable to create database '" + db + "': " + obj["reason"].getString());

   return obj["ok"].getBoolean();
}

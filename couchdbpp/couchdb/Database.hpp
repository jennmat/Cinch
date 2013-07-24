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
#ifndef __COUCH_DB_DATABASE_HPP__
#define __COUCH_DB_DATABASE_HPP__

#include <string>
#include <iostream>

#include "couchdb/Document.hpp"
#include "JsonBox.h"

using namespace JsonBox;
using namespace std;

namespace CouchDB{

class Database{
   friend class Connection;

   protected:
      Database(Communication&, const std::string&);

   public:
      Database(const Database&);
      ~Database();

      Database& operator=(Database&);

      const string& getName() const;

      vector<Document> listDocuments();
	  Object listViews();
	  
	  Object viewResults(const string& design, const string& view, int limit=25, bool includeDocs=false, bool reduce=false, int group_level=0);
      Object viewResults(const string& design, const string& view, int limit=25, int skip=0, bool includeDocs=false, bool reduce=false, int group_level=0);
	  Object viewResults(const string& design, const string& view, Value& startKey, Value& endKey, bool includeDocs=false, bool reduce=false);
      Object viewResultsFromStartDocId(const string& design, const string& view, Value& startKey, const string& startKeyDocId, int limit, int skip);

	  Document getDocument(const string&, const string &rev="");
	  Array getDocuments(const Array& ids);
      Document createDocument(const Value&, const string &id="");
      Document createDocument(Value, vector<Attachment>,
                              const string &id="");

	  vector<Document> documentsVectorFromValue(const Value& value);

	  void listenForChanges(void (*)());
	  void startReplication(const string& destinationHost, const string& destinationDatabase, const string& username, const string& password);
      
	  Communication& getCommunication();

   private:
      Communication &comm;
      std::string   name;
};

}

std::ostream& operator<<(std::ostream&, const CouchDB::Database&);

#endif

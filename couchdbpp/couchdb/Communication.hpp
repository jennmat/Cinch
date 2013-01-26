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
#ifndef __COUCH_DB_COMM_HPP__
#define __COUCH_DB_COMM_HPP__


#include <iostream>
#include <string>
#include <deque>
#include <map>

#include <curl/curl.h>
#include "JsonBox.h"

using namespace JsonBox;

namespace CouchDB{

// some data helpers aligned with TinyJSON implementation
//typedef boost::shared_ptr<boost::any>  Variant;
//typedef std::deque<Variant>            Array;
//typedef std::map<std::string, Variant> Object;

// convenience template
template<typename T>
Value createValue(T value){
   return Value(value);
}

class Communication{
	friend class Database;
   public:
      typedef std::map<std::string, std::string> HeaderMap;

      Communication();
      Communication(const std::string&);
      ~Communication();

      Value getData(const std::string&, const std::string &method = "GET",
                      const std::string &data = "");
      Value getData(const std::string&, const HeaderMap&,
                      const std::string &method = "GET",
                      const std::string &data = "");

	  void readChangesFeed(const std::string& database, void (*newDataArrived)());

      std::string getRawData(const std::string&);
   private:
      void init(const std::string&);
      Value getData(const std::string&, const std::string&,
                      std::string, const HeaderMap&);
      void getRawData(const std::string&, const std::string&,
                      std::string, const HeaderMap&);

      CURL        *curl;
      std::string baseURL;
      std::string buffer;
};

}

#endif

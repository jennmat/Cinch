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
#include "couchdb/Communication.hpp"
#include "couchdb/Exception.hpp"
#include "JsonBox.h"

#include <sstream>

using namespace std;
using namespace CouchDB;
using namespace JsonBox;

#define DEFAULT_COUCHDB_URL "http://localhost:5984"


static Value parseData(const string &buffer){
	Value value;
	value.loadFromString(buffer);

#ifdef COUCH_DB_DEBUG
   cout << "Data:" << endl;
   value.writeToStream(std::cout);
#endif

   return value;
}

static int writer(char *data, size_t size, size_t nmemb, string *dest){
   int written = 0;
   if(dest != NULL){
      written = size * nmemb;
      dest->append(data, written);
   }

   return written;
}

static size_t reader(void *ptr, size_t size, size_t nmemb, string *stream){
   int actual  = (int)stream->size();
   int written = size * nmemb;
   if(written > actual)
      written = actual;
   memcpy(ptr, stream->c_str(), written);
   stream->erase(0, written);
   return written;
}

Communication::Communication(){
   init(DEFAULT_COUCHDB_URL);
}

Communication::Communication(const string &url){
   init(url);
}

void Communication::init(const string &url){
   curl_global_init(CURL_GLOBAL_DEFAULT);

   curl = curl_easy_init();
   if(!curl)
      throw Exception("Unable to create CURL object");

   //curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);

   if(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer) != CURLE_OK)
      throw Exception("Unable to set writer function");

   if(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer) != CURLE_OK)
      throw Exception("Unable to set write buffer");

   baseURL = url;
}

Communication::~Communication(){
   if(curl)
      curl_easy_cleanup(curl);
   curl_global_cleanup();
}

Value Communication::getData(const string &url, const string &method,
                               const string &data){
   HeaderMap headers;
   return getData(url, method, data, headers);
}

Value Communication::getData(const string &url, const HeaderMap &headers,
                               const string &method, const string &data){
   return getData(url, method, data, headers);
}

string Communication::getRawData(const string &url){
   HeaderMap headers;
   getRawData(url, "GET", "", headers);
   return buffer;
}

void Communication::readChangesFeed(const std::string& database, void (*newDataArrived)()){
	int last_seq = 0;
	while(true){
		std::stringstream s;
		s << "/" << database << "/_changes?feed=longpoll&since=" << last_seq;
		string url = s.str();
		string data = getRawData(s.str());
		Value value = parseData(data);
		if ( value.isObject() ){
			newDataArrived();
	
			Object obj = value.getObject();
			if ( obj["last_seq"].isInteger() ){
				last_seq = obj["last_seq"].getInt();
			}
		}

		if ( last_seq == 0 ){
			return;
		}
	}
}

Value Communication::getData(const string &url, const string &method,
                               string data, const HeaderMap &headers){
   getRawData(url, method, data, headers);
   return parseData(buffer);
}

void Communication::getRawData(const string &_url, const string &method,
                               string data, const HeaderMap &headers){
   string url = baseURL + _url;

#ifdef COUCH_DB_DEBUG
   cout << "Getting data: " << url << " [" << method << "]" << endl;
#endif

   buffer.clear();

   if(curl_easy_setopt(curl, CURLOPT_URL, url.c_str()) != CURLE_OK)
      throw Exception("Unable to set URL: " + url);

   if(data.size() > 0){
#ifdef COUCH_DB_DEBUG
      cout << "Sending data: " << data << endl;
#endif

      if(curl_easy_setopt(curl, CURLOPT_READFUNCTION, reader) != CURLE_OK)
         throw Exception("Unable to set read function");

      if(curl_easy_setopt(curl, CURLOPT_READDATA, &data) != CURLE_OK)
         throw Exception("Unable to set data: " + data);

      if(curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L) != CURLE_OK)
         throw Exception("Unable to set upload request");

      if(curl_easy_setopt(curl, CURLOPT_INFILESIZE, (long)data.size()) != CURLE_OK)
         throw Exception("Unable to set content size: " + data.size());
   }

   if(headers.size() > 0 || data.size() > 0){
      struct curl_slist *chunk = NULL;

      HeaderMap::const_iterator header = headers.begin();
      const HeaderMap::const_iterator &headerEnd = headers.end();
      for(; header != headerEnd; ++header){
         string headerStr = header->first + ": " + header->second;
         chunk = curl_slist_append(chunk, headerStr.c_str());
      }

      if(data.size() > 0 && headers.find("Content-Type") == headers.end())
         chunk = curl_slist_append(chunk, "Content-Type: application/json");

      if(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk) != CURLE_OK)
         throw Exception("Unable to set custom headers");
   }

   if(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str()) != CURLE_OK)
      throw Exception("Unable to set HTTP method: " + method);

   if(curl_easy_perform(curl) != CURLE_OK)
      throw Exception("Unable to load URL: " + url);

   if(data.size() > 0 || headers.size() > 0){
      if(curl_easy_setopt(curl, CURLOPT_UPLOAD, 0L) != CURLE_OK)
         throw Exception("Unable to reset upload request");

      if(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL) != CURLE_OK)
         throw Exception("Unable to reset custom headers");
   }

#ifdef COUCH_DB_DEBUG
   long responseCode;
   if(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode) != CURLE_OK)
      throw Exception("Unable to get response code");

   cout << "Response code: " << responseCode << endl;
   cout << "Raw buffer: " << buffer;
#endif
}

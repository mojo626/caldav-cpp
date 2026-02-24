#include "caldav/client.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include "curl/curl.h"
#include "curl/easy.h"
#include "dotenv.h"
#include <pugixml.hpp>

//https://decovar.dev/blog/2021/03/08/cmake-cpp-library/

namespace caldav {

	size_t Client::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
	{
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	Client::Client(std::string base_url) : env("../.env") {
		std::cout << "Base URL: " << base_url << std::endl;

		std::string data = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> \
							<D:propfind xmlns:D=\"DAV:\"> \
							<D:prop> \
							<D:current-user-principal/> \
							</D:prop> \
							</D:propfind>";


		CalDAVRequest(base_url, "", 0, data);

		/*CURL *curl;
		CURLcode res;
		std::string readBuffer;
		
		curl = curl_easy_init();

		if (curl) {

			curl_easy_setopt(curl, CURLOPT_URL, base_url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			std::string user_pass = "ben:" + env.get("PASSWORD");


			curl_easy_setopt(curl, CURLOPT_USERPWD, user_pass.c_str()); 
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PROPFIND");
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			
			struct curl_slist *headers = NULL;
			headers = curl_slist_append(headers, "Depth: 0");
			headers = curl_slist_append(headers, "Content-Type: application/xml");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			std::string data = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> \
    <D:propfind xmlns:D=\"DAV:\"> \
      <D:prop> \
        <D:current-user-principal/> \
      </D:prop> \
    </D:propfind>";

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

			res = curl_easy_perform(curl);
			
			if (res != CURLE_OK) {
				std::cerr << "curl error: " << curl_easy_strerror(res) << std::endl;
			}



			std::cout << "Success: " << readBuffer << std::endl;

			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load_string(readBuffer.c_str());

			if (!result) {
				throw std::runtime_error("Failed to parse XML");
			}

			std::string user_root = doc.child("multistatus").child("response").child("propstat").child("prop").child("current-user-principal").child("href").child_value();
			

			std::string newData = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> \
								   <D:propfind xmlns:D=\"DAV:\" xmlns:C=\"urn:ietf:params:xml:ns:caldav\"> \
								   <D:prop> \
								   <C:calendar-home-set/> \
								   </D:prop> \
								   </D:propfind>";

			std::string user_url = base_url + user_root; 

			curl_easy_setopt(curl, CURLOPT_URL, user_url.c_str());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, newData.c_str());

			res = curl_easy_perform(curl);


			curl_easy_cleanup(curl);
		}*/

	}

	std::string Client::CalDAVRequest(std::string url, std::string user_pass, int depth, std::string data) {
		CURL *curl;
		CURLcode res;
		std::string readBuffer;
		
		curl = curl_easy_init();

		if (curl) {

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			std::string user_pass2 = "ben:" + env.get("PASSWORD");


			curl_easy_setopt(curl, CURLOPT_USERPWD, user_pass2.c_str()); 
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PROPFIND");
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

			struct curl_slist *headers = NULL;
			std::string depth_str = "Depth: " + std::to_string(depth);
			headers = curl_slist_append(headers, depth_str.c_str());
			headers = curl_slist_append(headers, "Content-Type: application/xml");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			/*std::string data = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> \
								<D:propfind xmlns:D=\"DAV:\"> \
								<D:prop> \
								<D:current-user-principal/> \
								</D:prop> \
								</D:propfind>";*/

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

			res = curl_easy_perform(curl);

			if (res != CURLE_OK) {
				std::cerr << "curl error: " << curl_easy_strerror(res) << std::endl;
				return "";
			}



			std::cout << "Success: " << readBuffer << std::endl;

			return readBuffer;
		}

		return "";

	}
}

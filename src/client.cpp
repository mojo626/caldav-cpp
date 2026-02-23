#include "caldav/client.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include "curl/curl.h"
#include "curl/easy.h"
#include "dotenv.h"

//https://decovar.dev/blog/2021/03/08/cmake-cpp-library/

namespace caldav {

	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
	{
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	Client::Client(std::string base_url) {
		std::cout << "Base URL: " << base_url << std::endl;

		dotenv env(".env");

		CURL *curl;
		CURLcode res;
		std::string readBuffer;
		
		curl = curl_easy_init();

		if (curl) {

			curl_easy_setopt(curl, CURLOPT_URL, "https://calendar.benjaynes.com");
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			std::string user_pass = "ben:" + env.get("PASSWORD");
			curl_easy_setopt(curl, CURLOPT_USERPWD, user_pass.c_str()); 
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

			res = curl_easy_perform(curl);
			
			if (res != CURLE_OK) {
				std::cerr << "curl error: " << curl_easy_strerror(res) << std::endl;
			}


			curl_easy_cleanup(curl);

			std::cout << "Success: " << readBuffer << std::endl;

		}

	}
}

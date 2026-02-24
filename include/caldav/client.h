#pragma once
#include <string>
#include "dotenv.h"

namespace caldav {
	class Client {
		public:
			Client(std::string base_url);
			std::string CalDAVRequest(std::string url, std::string user_pass, int depth, std::string data);

		private:
			static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
			dotenv env;	
	};
}

#pragma once
#include <string>
#include "dotenv.h"

namespace caldav {
	class Client {
		public:
			Client(std::string base_url);
			std::string CalDAVRequest(std::string url, std::string user_pass, int depth, std::string data, bool verbose = false);

		private:
			static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
			std::string GetUserRoot(std::string base_url, std::string user_pass, bool verbose = false);
			std::string GetUserCalendarPath(std::string base_url, std::string user_root, std::string user_pass, bool verbose = false);
			void GetCalendars(std::string base_url, std::string calendar_path, std::string user_pass, bool verbose = false);
			dotenv env;	
	};
}

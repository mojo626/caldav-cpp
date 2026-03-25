#pragma once
#include <string>
#include <vector>
#include "caldav/calendar.h"
#include "caldav/event.h"
#include "caldav/todo.h"

namespace caldav {
	class Client {
		public:
			Client(std::string base_url, std::string user_pass, std::string prod_id = "-//caldav-cpp.//CalDAV Client//EN");
			std::string CalDAVRequest(std::string url, std::string user_pass, int depth, std::string data, std::string method = "PROPFIND", bool verbose = false);
			std::vector<caldav::Calendar> GetCalendars();
			std::vector<caldav::Todo> GetTodos(Calendar cal, bool verbose = false);
			std::vector<caldav::Event> GetEvents(Calendar cal, bool verbose = false);
			int CreateNewTodo(caldav::Todo newTodo, Calendar cal, bool verbose = false);

		private:
			static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
			void CheckPaths();
			std::string GetUserRoot(std::string base_url, std::string user_pass, bool verbose = false);
			std::string GetUserCalendarPath(std::string base_url, std::string user_root, std::string user_pass, bool verbose = false);
			std::vector<caldav::Calendar> GetCalendars(std::string base_url, std::string calendar_path, std::string user_pass, bool verbose = false);

			//Local variables
			std::string base_url;
			std::string user_pass;
			std::string user_root;
			std::string calendar_path;
			std::string prod_id;
			std::vector<caldav::Calendar> calendars;
	};
}

#pragma once
#include <string>
#include <vector>
#include "caldav/calendar.h"
#include "caldav/event.h"
#include "caldav/todo.h"
#include "ical.h"

namespace caldav {
	class Client {
		public:
			Client(std::string base_url, std::string user_pass, std::string prod_id = "-//caldav-cpp.//CalDAV Client//EN");
			std::string CalDAVRequest(std::string url, std::string user_pass, int depth, std::string data, std::string method = "PROPFIND", bool verbose = false);
			std::string PutRequestNew(std::string url, std::string user_pass, std::string data, bool verbose = false);
			std::string PutRequestUpdate(std::string url, std::string user_pass, std::string data, std::string etag, bool verbose = false);
			std::vector<caldav::Calendar> GetCalendars();
			std::vector<caldav::Todo> GetTodos(Calendar cal, bool verbose = false);
			std::vector<caldav::Event> GetEvents(Calendar cal, bool verbose = false);
			caldav::Todo CreateNewTodo(std::string summary, icalproperty_status status, Calendar cal, bool verbose = false);
			int CreateNewTodo(caldav::Todo todo, Calendar cal, bool verbose = false);
			int UpdateTodo(caldav::Todo todo, Calendar cal, bool verbose);

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

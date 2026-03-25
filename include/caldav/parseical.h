#pragma once
#include <string>
#include "caldav/event.h"
#include "caldav/todo.h"

namespace caldav {
	class ParseIcal {
		public:
			static caldav::Todo ParseTodo(std::string data, std::string etag);
			static caldav::Event ParseEvent(std::string data, std::string etag);
			static std::string TodoToIcal(caldav::Todo todo, std::string prod_id);
		private:
			static bool IsPrefix(std::string shortStr, std::string longStr);
	};
}

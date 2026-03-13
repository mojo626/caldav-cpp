#pragma once
#include <string>
#include "caldav/event.h"
#include "caldav/todo.h"

namespace caldav {
	class ParseIcal {
		public:
			static caldav::Todo ParseTodo(std::string data);
			static caldav::Event ParseEvent(std::string data);
	};
}

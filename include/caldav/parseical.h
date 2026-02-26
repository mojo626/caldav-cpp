#pragma once
#include <string>
#include "caldav/todo.h"

namespace caldav {
	class ParseIcal {
		public:
			static caldav::Todo ParseTodo(std::string data);
	};
}

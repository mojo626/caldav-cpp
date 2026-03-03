#pragma once
#include <string>
#include <vector>
#include "todo.h"

namespace caldav {
	class Calendar {
		public:
			Calendar(std::string display_name, std::string url, std::string ctag, std::vector<std::string> supported_components, std::string color);

			std::string display_name;
			std::string url;
			std::string ctag;
			std::vector<std::string> supported_components;
			std::string color;

	};
}

#include "caldav/calendar.h"
#include <string>

namespace caldav {
	
	Calendar::Calendar() {

	}

	Calendar::Calendar(std::string display_name, std::string url, std::string ctag, std::vector<std::string> supported_components, std::string color) {
		this->display_name = display_name;
		this->url = url;
		this->ctag = ctag;
		this->supported_components = supported_components;
		this->color = color;
	}

}

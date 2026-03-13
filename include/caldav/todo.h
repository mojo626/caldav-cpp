#pragma once
#include "caldav/calendar.h"
#include "caldav/timeutils.hpp"
#include <string>
#include <chrono>
#include <sstream>
#include <date/date.h>
#include <iostream>
#include <ctime>

namespace caldav {
	enum TodoStatus {
		COMPLETED,
		NEEDS_ACTION,
		IN_PROGRESS,
		CANCELLED
	};


	struct Todo {
		std::string uid;
		std::string summary;
		TodoStatus status;	
		std::string completed;
		std::string created;
		std::string dtstamp;
		std::string last_modified;
		int percent_completed;
			
		std::tm getCompletedDateLocal() {	
			auto tp = TimeUtils::tm_from_format(completed, "%Y%m%dT%H%M%SZ");

			std::tm tm = TimeUtils::to_local_tm(tp);

			return tm;

		}
	};

	

}

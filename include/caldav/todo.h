#pragma once
#include "caldav/timeutils.hpp"
#include <string>
#include <chrono>
#include <date/date.h>
#include <ctime>
#include <libical/ical.h>

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
			auto tp = TimeUtils::tp_from_format(completed, "%Y%m%dT%H%M%SZ");

			std::tm tm = TimeUtils::to_local_tm(tp);

			return tm;

		}
	};

	

}

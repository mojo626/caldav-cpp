#pragma once
#include "caldav/timeutils.hpp"
#include <string>
#include <chrono>
#include <date/date.h>
#include <ctime>
#include <libical/ical.h>

namespace caldav {

	struct Todo {
		std::string uid;
		std::string etag;
		std::string summary;
		icalproperty_status status;	
		icaltimetype completed;
		icaltimetype created;
		icaltimetype dtstamp;
		icaltimetype last_modified;
		int percent_completed;
			
		// std::tm getCompletedDateLocal() {	
		// 	auto tp = TimeUtils::tp_from_format(completed, "%Y%m%dT%H%M%SZ");

		// 	std::tm tm = TimeUtils::to_local_tm(tp);

		// 	return tm;

		// }
	};

	

}

#pragma once
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

	static std::tm to_utc_tm(std::chrono::system_clock::time_point tp) {
		std::time_t tt = std::chrono::system_clock::to_time_t(tp);

		std::tm tm{};
		gmtime_r(&tt, &tm); // thread-safe UTC (macOS & Linux)

		return tm;
	}

	static std::tm to_local_tm(std::chrono::system_clock::time_point tp) {
		std::time_t tt = std::chrono::system_clock::to_time_t(tp);

		std::tm tm{};
		localtime_r(&tt, &tm); // local timezone
		return tm;
	}

	struct Todo {
		std::string uid;
		std::string summary;
		TodoStatus status;	
		std::string completed;
		std::string created;
		std::string dtstamp;
		std::string last_modified;
		int percent_completed;
			
		void get_day() {
			std::istringstream in{completed};
			std::chrono::system_clock::time_point tp;

			in >> date::parse("%Y%m%dT%H%M%SZ", tp);

			if (in.fail()) {
				throw std::runtime_error("Failed to parse time: " + completed);
			}		

			std::tm tm = to_utc_tm(tp);

			std::cout << tm.tm_year << std::endl;

			//std::cout << tp << std::endl;
		}
	};

	

}

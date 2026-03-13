#include "caldav/parseical.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <sstream>
#include "caldav/event.h"
#include "caldav/todo.h"

namespace caldav {
	
	bool ParseIcal::IsPrefix(std::string shortStr, std::string longStr) {
		auto res = std::mismatch(shortStr.begin(), shortStr.end(), longStr.begin());

		return res.first == shortStr.end();
	}

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

	caldav::Todo ParseIcal::ParseTodo(std::string data) {
		Todo todo;

		std::istringstream iss(data);
		std::string line;

		bool in_todo = false;

		while(std::getline(iss, line)) {
			std::string key = line.substr(0, line.find_first_of(":"));
			std::string value = line.substr(line.find_first_of(":") + 1, line.length() - 1);

			if (key == "BEGIN" && value == "VTODO") {
				in_todo = true;
			}

			if (key == "END" && value == "VTODO") {
				in_todo = false;
			}

			if (!in_todo) {
				continue;
			}


			if (key == "SUMMARY") {
				todo.summary = value;	
			} else if (key == "STATUS") {
				if (value == "COMPLETED") {
					todo.status = TodoStatus::COMPLETED;
				} else if (value == "IN-PROGRESS") {
					todo.status = TodoStatus::IN_PROGRESS;
				} else if (value == "NEEDS-ACTION") {
					todo.status = TodoStatus::NEEDS_ACTION;
				} else if (value == "CANCELLED") {
					todo.status = TodoStatus::CANCELLED;
				}
			} else if (key == "COMPLETED") {
				todo.completed = value;
			} else if (key == "CREATED") {
				todo.created = value;
			} else if (key == "DTSTAMP") {
				todo.dtstamp = value;
			} else if (key == "LAST-MODIFIED") {
				todo.last_modified = value;
			} else if (key == "UID") {
				todo.uid = value;
			} else if (key == "PERCENT-COMPLETE") {
				todo.percent_completed = std::stoi(value);
			}
		}

		return todo;

	}

	caldav::Event ParseIcal::ParseEvent(std::string data) {
		Event event;

		//std::cout << data << std::endl << std::endl;

		std::istringstream iss(data);
		std::string line;

		bool in_event = false;

		while(std::getline(iss, line)) {
			std::string key = line.substr(0, line.find_first_of(":"));
			std::string value = line.substr(line.find_first_of(":") + 1, line.length() - 1);

			if (key == "BEGIN" && value == "VEVENT") {
				in_event = true;
			}

			if (key == "END" && value == "VEVENT") {
				in_event = false;
			}

			if (!in_event) {
				continue;
			}


			if (key == "SUMMARY") {
				event.summary = value;	
			} else if (key == "UID") {
				event.uid = value;
			} else if (key == "CREATED") {
				event.created = value;
			} else if (key == "DTSTAMP") {
				event.dtstamp = value;
			} else if (key == "LAST-MODIFIED") {
				event.last_modified = value;
			} else if (IsPrefix("DTSTART", key)) {

				//Whether the start of the event is a time or date (is the event full-day)
				//Can either be something like "TZID=America/Los_Angeles" for time or "VALUE=DATE" for a full-day event
				std::string start_data = key.substr(key.find_first_of(";") + 1, key.length() - 1);

				std::string start_type = start_data.substr(0, start_data.find_first_of("="));
				std::string start_value = start_data.substr(start_data.find_first_of("=") + 1, start_data.length() - 1);

				if (start_type == "VALUE" && start_value == "DATE") {
					event.dtstart = value;
				} else if (start_type == "TZID") {
					event.dtstart = value;
					event.time_zone = start_value;
				}
			} else if (IsPrefix("DTEND", key)) {

				//Whether the end of the event is a time or date (is the event full-day)
				//Can either be something like "TZID=America/Los_Angeles" for time or "VALUE=DATE" for a full-day event
				std::string end_data = key.substr(key.find_first_of(";") + 1, key.length() - 1);

				std::string end_type = end_data.substr(0, end_data.find_first_of("="));
				std::string end_value = end_data.substr(end_data.find_first_of("=") + 1, end_data.length() - 1);

				if (end_type == "VALUE" && end_value == "DATE") {
					event.dtend = value;
				} else if (end_type == "TZID") {
					event.dtend = value;
					event.time_zone = end_value;
				}
			}
		}
		
		return event;
	}
}

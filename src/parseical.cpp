#include "caldav/parseical.h"
#include <algorithm>
#include <string>
#include <sstream>
#include "caldav/event.h"
#include "caldav/todo.h"
#include "icalcomponent.h"
#include "icalderivedproperty.h"
#include <libical/ical.h>

namespace caldav {
	
	bool ParseIcal::IsPrefix(std::string shortStr, std::string longStr) {
		auto res = std::mismatch(shortStr.begin(), shortStr.end(), longStr.begin());

		return res.first == shortStr.end();
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

	caldav::Event ParseIcal::ParseEvent(std::string data, std::string etag) {
		Event event;

		icalcomponent* component = icalparser_parse_string(data.c_str());

		event.uid = icalcomponent_get_uid(component);
		event.summary = icalcomponent_get_summary(component);
		event.dtstamp = icalcomponent_get_dtstamp(component);
		event.dtstart = icalcomponent_get_dtstart(component);
		event.dtend = icalcomponent_get_dtend(component);
		event.etag = etag;
		
		return event;
	}

	std::string ParseIcal::TodoToIcal(caldav::Todo todo, std::string prod_id) {
		icalcomponent* vcal = icalcomponent_new(ICAL_VCALENDAR_COMPONENT);

		icalcomponent_add_property(vcal, 
			icalproperty_new_version("2.0"));
		
		icalcomponent_add_property(vcal, 
			icalproperty_new_calscale("GREGORIAN"));

		icalcomponent_add_property(vcal, 
			icalproperty_new_prodid(prod_id.c_str()));

		icalcomponent* vtodo = icalcomponent_new(ICAL_VTODO_COMPONENT);

		icalcomponent_add_property(vtodo,
			icalproperty_new_summary(todo.summary.c_str()));

		icalcomponent_add_property(vtodo,
			icalproperty_new_uid(todo.uid.c_str()));

		//icalcomponent_add_property(vtodo, icalproperty_new_completed(todo.completed));

		icalcomponent_add_component(vcal, vtodo);

		char* ics = icalcomponent_as_ical_string(vcal);

		return ics;
	}

}

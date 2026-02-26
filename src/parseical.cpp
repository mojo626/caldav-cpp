#include "caldav/parseical.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <sstream>
#include "caldav/todo.h"

namespace caldav {

	caldav::Todo ParseIcal::ParseTodo(std::string data) {
		Todo todo;

		std::istringstream iss(data);
		std::string line;

		bool in_todo = false;

		while(std::getline(iss, line)) {
			std::cout << line << std::endl;
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
}

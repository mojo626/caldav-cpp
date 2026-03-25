#include "caldav/parseical.h"
#include <algorithm>
#include <cstddef>
#include <string>
#include <sstream>
#include "caldav/event.h"
#include "caldav/todo.h"
#include "icalcomponent.h"
#include "icalderivedproperty.h"
#include "icalparser.h"
#include <libical/ical.h>

namespace caldav {
	
	bool ParseIcal::IsPrefix(std::string shortStr, std::string longStr) {
		auto res = std::mismatch(shortStr.begin(), shortStr.end(), longStr.begin());

		return res.first == shortStr.end();
	}



	caldav::Todo ParseIcal::ParseTodo(std::string data, std::string etag) {
		std::cout << data << std::endl;
		Todo todo;

		icalcomponent* component = icalparser_parse_string(data.c_str());

		icalcomponent* vtodo = icalcomponent_get_first_component(component, ICAL_VTODO_COMPONENT);

		todo.uid = icalcomponent_get_uid(vtodo);
		todo.etag = etag;
		todo.summary = icalcomponent_get_summary(vtodo);
		todo.status = icalcomponent_get_status(vtodo);
		todo.completed = icalproperty_get_completed(icalcomponent_get_first_property(vtodo, ICAL_COMPLETED_PROPERTY));
		todo.created = icalproperty_get_created(icalcomponent_get_first_property(vtodo, ICAL_CREATED_PROPERTY));
		todo.dtstamp = icalproperty_get_dtstamp(icalcomponent_get_first_property(vtodo, ICAL_DTSTAMP_PROPERTY));
		todo.last_modified = icalproperty_get_lastmodified(icalcomponent_get_first_property(vtodo, ICAL_LASTMODIFIED_PROPERTY));
		todo.percent_completed = icalproperty_get_percentcomplete(icalcomponent_get_first_property(vtodo, ICAL_PERCENTCOMPLETE_PROPERTY));

		icalcomponent_free(vtodo);
		icalcomponent_free(component);

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

		icalcomponent_free(component);
		
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

		if (todo.status == icalproperty_status::ICAL_STATUS_COMPLETED) {
			icalcomponent_add_property(vtodo, 
				icalproperty_new_completed(todo.completed));
		}
			

		icalcomponent_add_property(vtodo, 
			icalproperty_new_created(todo.created));

		icalcomponent_add_property(vtodo, 
			icalproperty_new_dtstamp(todo.dtstamp));

		icalcomponent_add_property(vtodo, 
			icalproperty_new_lastmodified(todo.last_modified));

		icalcomponent_add_property(vtodo, 
			icalproperty_new_status(todo.status));

		icalcomponent_add_property(vtodo, 
			icalproperty_new_percentcomplete(todo.percent_completed));

		icalcomponent_add_component(vcal, vtodo);

		char* ics = icalcomponent_as_ical_string(vcal);

		icalcomponent_free(vtodo);
		icalcomponent_free(vcal);

		return ics;
	}

}

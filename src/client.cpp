#include "caldav/client.h"
#include "caldav/calendar.h"
#include "caldav/event.h"
#include "caldav/parseical.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include "caldav/todo.h"
#include "curl/curl.h"
#include "curl/easy.h"
#include <pugixml.hpp>
#include <vector>
#include <libical/ical.h>
#include <uuid/uuid.h>


namespace caldav {

	size_t Client::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
	{
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}


	Client::Client(std::string base_url, std::string user_pass, std::string prod_id) {

		this->base_url = base_url;

		this->user_pass = user_pass;

		this->prod_id = prod_id;

		this->user_root = GetUserRoot(base_url, user_pass);

		this->calendar_path = GetUserCalendarPath(base_url, user_root, user_pass);

		this->calendars = GetCalendars(base_url, calendar_path, user_pass);

	}

	void Client::CheckPaths() {
		if (user_root.empty()) {
			user_root = GetUserRoot(base_url, user_pass);
		}

		if (calendar_path.empty()) {
			calendar_path = GetUserCalendarPath(base_url, user_root, user_pass);
		}
	}

	caldav::Todo Client::CreateNewTodo(std::string summary, icalproperty_status status, Calendar cal, bool verbose) {
		caldav::Todo todo;

		todo.summary = summary;
		if (status == icalproperty_status::ICAL_STATUS_COMPLETED) {
			todo.completed = icaltime_current_time_with_zone(icaltimezone_get_utc_timezone());
		}
		todo.created = icaltime_current_time_with_zone(icaltimezone_get_utc_timezone());
		todo.dtstamp = icaltime_current_time_with_zone(icaltimezone_get_utc_timezone());
		todo.last_modified = icaltime_current_time_with_zone(icaltimezone_get_utc_timezone());
		todo.percent_completed = status == icalproperty_status::ICAL_STATUS_COMPLETED ? 100 : 0;
		todo.status = status;
		
		
		uuid_t uuid;
		uuid_generate_random(uuid); 

		char uuid_str[37]; 
		uuid_unparse_lower(uuid, uuid_str);

		std::string cpp_uuid(uuid_str);


		todo.uid = uuid_str;

		std::string ics = ParseIcal::TodoToIcal(todo, this->prod_id);

		std::string response = PutRequestNew(this->base_url + cal.url + uuid_str + ".ics", this->user_pass, ics, verbose);

		if (response == "") {
			std::cout << "Error when creating new todo" << std::endl;
			return todo;
		}

		return todo;
	}

	int Client::CreateNewTodo(caldav::Todo todo, Calendar cal, bool verbose) {
		uuid_t uuid;
		uuid_generate_random(uuid); 

		char uuid_str[37]; 
		uuid_unparse_lower(uuid, uuid_str);

		std::string cpp_uuid(uuid_str);


		todo.uid = uuid_str;

		std::string ics = ParseIcal::TodoToIcal(todo, this->prod_id);

		std::string response = PutRequestNew(this->base_url + cal.url + uuid_str + ".ics", this->user_pass, ics, verbose);

		if (response == "") {
			return 0;
		}

		return 1;
	}

	int Client::UpdateTodo(caldav::Todo todo, Calendar cal, bool verbose) {
		uuid_t uuid;
		uuid_generate_random(uuid); 

		char uuid_str[37]; 
		uuid_unparse_lower(uuid, uuid_str);

		std::string cpp_uuid(uuid_str);


		todo.uid = uuid_str;

		std::string ics = ParseIcal::TodoToIcal(todo, this->prod_id);

		std::string response = PutRequestUpdate(this->base_url + cal.url + uuid_str + ".ics", this->user_pass, ics, todo.etag, verbose);

		if (response == "") {
			return 0;
		}

		return 1;
	}

	std::vector<caldav::Todo> Client::GetTodos(Calendar cal, bool verbose) {
		
		std::string data = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> \
							<c:calendar-query xmlns:d=\"DAV:\" xmlns:c=\"urn:ietf:params:xml:ns:caldav\"> \
							<d:prop> \
							<d:getetag/> \
							<c:calendar-data/> \
							</d:prop> \
							<c:filter> \
							<c:comp-filter name=\"VCALENDAR\"> \
							<c:comp-filter name=\"VTODO\" /> \
							</c:comp-filter> \
							</c:filter> \
							</c:calendar-query>";

		std::string response =	CalDAVRequest(base_url + cal.url, user_pass, 1, data, "REPORT", verbose);

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(response.c_str());

		if (!result) {
			throw std::runtime_error("Failed to parse XML");
		}

		std::vector<Todo> todos;

		for (pugi::xml_node todo_response : doc.child("multistatus").children("response")) {
			std::string todo_string = todo_response.child("propstat").child("prop").child("C:calendar-data").child_value();
			std::string etag = todo_response.child("propstat").child("prop").child("getetag").child_value();
			etag = etag.substr(1, etag.length() - 2);

			Todo todo = ParseIcal::ParseTodo(todo_string, etag);

			todos.push_back(todo);
		} 


		return todos;
	}

	std::vector<caldav::Event> Client::GetEvents(Calendar cal, bool verbose) {
		
		std::string data = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> \
							<c:calendar-query xmlns:d=\"DAV:\" xmlns:c=\"urn:ietf:params:xml:ns:caldav\"> \
							<d:prop> \
							<d:getetag/> \
							<c:calendar-data/> \
							</d:prop> \
							<c:filter> \
							<c:comp-filter name=\"VCALENDAR\"> \
							<c:comp-filter name=\"VEVENT\" /> \
							</c:comp-filter> \
							</c:filter> \
							</c:calendar-query>";

		std::string response =	CalDAVRequest(base_url + cal.url, user_pass, 1, data, "REPORT", verbose);

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(response.c_str());

		if (!result) {
			throw std::runtime_error("Failed to parse XML");
		}

		std::vector<caldav::Event> events;

		for (pugi::xml_node event_response : doc.child("multistatus").children("response")) {
			std::string event_string = event_response.child("propstat").child("prop").child("C:calendar-data").child_value();
			std::string etag = event_response.child("propstat").child("prop").child("getetag").child_value();
			etag = etag.substr(1, etag.length() - 2);

			caldav::Event event = ParseIcal::ParseEvent(event_string, etag);

			events.push_back(event);
		} 


		return events;
	}

	std::string Client::GetUserRoot(std::string base_url, std::string user_pass, bool verbose) {
		std::string data = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> \
							<D:propfind xmlns:D=\"DAV:\"> \
							<D:prop> \
							<D:current-user-principal/> \
							</D:prop> \
							</D:propfind>";

		
		std::string response = CalDAVRequest(base_url, user_pass, 0, data, "PROPFIND", verbose);
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(response.c_str());

		if (!result) {
			throw std::runtime_error("Failed to parse XML");
		}

		std::string user_root = doc.child("multistatus").child("response").child("propstat").child("prop").child("current-user-principal").child("href").child_value();
		
		return user_root;
	}

	std::string Client::GetUserCalendarPath(std::string base_url, std::string user_root, std::string user_pass, bool verbose) {
		std::string data = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> \
							<D:propfind xmlns:D=\"DAV:\" xmlns:C=\"urn:ietf:params:xml:ns:caldav\"> \
							<D:prop> \
							<C:calendar-home-set/> \
							</D:prop> \
							</D:propfind>";


		std::string response = CalDAVRequest(base_url + user_root, user_pass, 0, data, "PROPFIND", verbose);

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(response.c_str());

		if (!result) {
			throw std::runtime_error("Failed to parse XML");
		}


		std::string user_calendar_path = doc.child("multistatus").child("response").child("propstat").child("prop").child("calendar-home-set").child("href").child_value();

		return user_root;
	}


	/**
	 * @brief Gets a list of calendars avilable for the user on the client 
	 * 
	 * @return std::vector<caldav::Calendar> list of calendars on server
	 */
	std::vector<caldav::Calendar> Client::GetCalendars() {
		if (calendars.empty()) {
			this->calendars = GetCalendars(base_url, calendar_path, user_pass);
		}

		return this->calendars;
	}


	std::vector<caldav::Calendar> Client::GetCalendars(std::string base_url, std::string calendar_path, std::string user_pass, bool verbose) {
		std::string data = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> \
							<D:propfind xmlns:D=\"DAV:\" xmlns:cs=\"http://calendarserver.org/ns/\" xmlns:C=\"urn:ietf:params:xml:ns:caldav\" xmlns:apple=\"http://apple.com/ns/ical/\"> \
							<D:prop> \
							<D:resourcetype/> \
							<D:displayname/> \
							<cs:getctag/> \
							<C:supported-calendar-component-set/> \
							<apple:calendar-color/> \
							</D:prop> \
							</D:propfind>";


		std::string response = CalDAVRequest(base_url + calendar_path, user_pass, 1, data, "PROPFIND", verbose);

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(response.c_str());

		if (!result) {
			throw std::runtime_error("Failed to parse XML");
		}
		


		std::vector<caldav::Calendar> calendars;

		for (pugi::xml_node calendar : doc.child("multistatus").children("response")) {
			//skip the base calendar collection (I don't think we want this, check other services that are not radicale to make sure)
			if (calendar.child("propstat").child("prop").child("resourcetype").child("principal")) {
				continue;
			}

			std::string display_name = calendar.child("propstat").child("prop").child("displayname").child_value();
			std::string url = calendar.child("href").child_value();
			std::string ctag = calendar.child("propstat").child("prop").child("CS:getctag").child_value();
			ctag = ctag.substr(1, ctag.length() - 2); //remove quotes on either side
			std::string color = calendar.child("propstat").child("prop").child("ICAL:calendar-color").child_value();

			std::vector<std::string> supported_components;

			for (pugi::xml_node supported_component : calendar.child("propstat").child("prop").child("C:supported-calendar-component-set").children("C:comp")) {
				std::string name = supported_component.attribute("name").value(); 
				supported_components.push_back(name);
			}
			
			caldav::Calendar cal{
					display_name = display_name,
					url = url,
					ctag = ctag,
					supported_components = supported_components,
					color = color};	
			
			calendars.push_back(cal);
		}


		return calendars;
	}

	std::string Client::PutRequestNew(std::string url, std::string user_pass, std::string data, bool verbose) {
		CURL *curl;
		CURLcode res;
		std::string readBuffer;

		curl = curl_easy_init();

		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
			curl_easy_setopt(curl, CURLOPT_USERPWD, user_pass.c_str()); 
			curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose ? 1L : 0L);

			struct curl_slist* headers = nullptr;
			headers = curl_slist_append(headers, "Content-Type: text/calendar");
			headers = curl_slist_append(headers, "If-None-Match: *");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			res = curl_easy_perform(curl);

			if (res != CURLE_OK) {
				std::cerr << "curl error: " << curl_easy_strerror(res) << std::endl;
				return "";
			}


			return readBuffer;
		}


		return "";
	}

	std::string Client::PutRequestUpdate(std::string url, std::string user_pass, std::string data, std::string etag, bool verbose) {
		CURL *curl;
		CURLcode res;
		std::string readBuffer;

		curl = curl_easy_init();

		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
			curl_easy_setopt(curl, CURLOPT_USERPWD, user_pass.c_str()); 
			curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose ? 1L : 0L);

			struct curl_slist* headers = nullptr;
			headers = curl_slist_append(headers, "Content-Type: text/calendar");
			headers = curl_slist_append(headers, ("If-Match: " + etag).c_str());

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			res = curl_easy_perform(curl);

			if (res != CURLE_OK) {
				std::cerr << "curl error: " << curl_easy_strerror(res) << std::endl;
				return "";
			}


			return readBuffer;
		}


		return "";
	}

	/**
	 * @brief Sends a PROPFIND request to CalDAV server using libcurl 
	 * 
	 * 
	 * @param  url: base url for server (http://localhost:5232) 
	 * @param  user_pass: username and password for basic authentication in a "<user>:<password" format
	 * @param depth: depth of the PROPFIND request
	 * @param data: xml string of data to send in PROPFIND request
	 * @param verbose: (optional) whether the curl request should be verbose
	 * 
	 * @return std::string of response from server
	 */
	std::string Client::CalDAVRequest(std::string url, std::string user_pass, int depth, std::string data, std::string method, bool verbose) {
		CURL *curl;
		CURLcode res;
		std::string readBuffer;
		
		curl = curl_easy_init();

		if (curl) {

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); //maybe not neccesary in the future?


			curl_easy_setopt(curl, CURLOPT_USERPWD, user_pass.c_str()); 
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
			curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose ? 1L : 0L);

			struct curl_slist *headers = NULL;
			std::string depth_str = "Depth: " + std::to_string(depth);
			headers = curl_slist_append(headers, depth_str.c_str());
			headers = curl_slist_append(headers, "Content-Type: application/xml");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

			res = curl_easy_perform(curl);

			if (res != CURLE_OK) {
				std::cerr << "curl error: " << curl_easy_strerror(res) << std::endl;
				return "";
			}


			return readBuffer;
		}

		return "";

	}
}

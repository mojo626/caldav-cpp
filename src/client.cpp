#include "caldav/client.h"
#include "caldav/calendar.h"
#include "caldav/parseical.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include "curl/curl.h"
#include "curl/easy.h"
#include "dotenv.h"
#include <pugixml.hpp>
#include <vector>
//#include <libical/ical.h>
#include <uICAL.h>

//https://decovar.dev/blog/2021/03/08/cmake-cpp-library/

namespace caldav {

	size_t Client::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
	{
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	Client::Client(std::string base_url) : env("../.env") {
		std::cout << "Base URL: " << base_url << std::endl;

		std::string user_pass = "ben:" + env.get("PASSWORD");

		std::string user_root = GetUserRoot(base_url, user_pass, true);

		std::string calendar_path = GetUserCalendarPath(base_url, user_root, user_pass);


		std::vector<caldav::Calendar> calendars = GetCalendars(base_url, calendar_path, user_pass);

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

		std::cout << base_url + calendars[0].url << std::endl;

		std::string events = CalDAVRequest(base_url + calendars[0].url, user_pass, 1, data, "REPORT", true);

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(events.c_str());

		if (!result) {
			throw std::runtime_error("Failed to parse XML");
		}

		std::string event_data = doc.child("multistatus").first_child().child("propstat").child("prop").child("C:calendar-data").child_value();

		Todo todo = ParseIcal::ParseTodo(event_data);

		std::cout << todo.completed << std::endl;	
		todo.get_day();
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
			
			caldav::Calendar cal(
					display_name,
					url,
					ctag,
					supported_components,
					color);	
			
			calendars.push_back(cal);
		}


		return calendars;
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

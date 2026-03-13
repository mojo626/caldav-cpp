#include "caldav/calendar.h"
#include "caldav/client.h"
#include "caldav/todo.h"
#include "dotenv.h"
#include "caldav/parseical.h"
#include <iostream>
#include <vector>

int main() {
	std::cout << "Hello, World!" << std::endl;

	dotenv env("../.env");

	std::string user_pass = "ben:" + env.get("PASSWORD");

	caldav::Client client("https://calendar.benjaynes.com", user_pass);
	//caldav::Client client("http://10.0.0.82:5232");
	//
	std::vector<caldav::Calendar> calendars = client.GetCalendars();

	std::cout << calendars[1].display_name << std::endl;

	// std::vector<caldav::Todo> todos = client.GetTodos(calendars[1]);

	// for (caldav::Todo todo : todos) {
	// 	std::cout << "Local completed hour: " << todo.getCompletedDateLocal().tm_hour << std::endl;
	// }

	std::vector<std::string> events = client.GetEvents(calendars[1]);

	std::cout << events[12] << std::endl;

	return 0;
}

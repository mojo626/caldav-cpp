#include "caldav/calendar.h"
#include "caldav/client.h"
#include "caldav/event.h"
#include "dotenv.h"
#include "ical.h"
#include <iostream>
#include <vector>

int main() {
	std::cout << "Hello, World!" << std::endl;

	dotenv env("../.env");

	std::string user_pass = "ben:" + env.get("PASSWORD");

	caldav::Client client("https://calendar.benjaynes.com", user_pass);
	//caldav::Client client("http://10.0.0.82:5232");
	
	std::vector<caldav::Calendar> calendars = client.GetCalendars();

	

	std::vector<caldav::Todo> todos = client.GetTodos(calendars[2]);

	// for (caldav::Todo todo : todos) {
	// 	std::cout << "Local completed hour: " << todo.completed.hour << std::endl;
	// }

	// std::vector<caldav::Event> events = client.GetEvents(calendars[1]);

	// for (caldav::Event event : events) {
	// 	std::cout << event.summary << ": " <<  event.dtstart.day << std::endl;
	// }

	caldav::Todo todo = todos[0];

	//caldav::Todo todo = client.CreateNewTodo("will the todo update?", icalproperty_status::ICAL_STATUS_NEEDSACTION, calendars[2]);

	todo.summary = "updated todo";
	// todo.summary = "new?";
	//std::cout << todo.summary << std::endl;
	client.UpdateTodo(todo, calendars[2], false);


	return 0; 
}

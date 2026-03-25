#include "caldav/calendar.h"
#include "caldav/client.h"
#include "caldav/event.h"
#include "dotenv.h"
#include <iostream>
#include <vector>

int main() {
	std::cout << "Hello, World!" << std::endl;

	dotenv env("../.env");

	std::string user_pass = "ben:" + env.get("PASSWORD");

	caldav::Client client("https://calendar.benjaynes.com", user_pass);
	//caldav::Client client("http://10.0.0.82:5232");
	
	std::vector<caldav::Calendar> calendars = client.GetCalendars();

	std::cout << calendars[2].display_name << std::endl;
	

	std::vector<caldav::Todo> todos = client.GetTodos(calendars[1]);

	// for (caldav::Todo todo : todos) {
	// 	std::cout << "Local completed hour: " << todo.created << std::endl;
	// }

	// std::vector<caldav::Event> events = client.GetEvents(calendars[1]);

	// for (caldav::Event event : events) {
	// 	std::cout << event.summary << ": " <<  event.dtstart.day << std::endl;
	// }

	caldav::Todo todo;

	client.CreateNewTodo(todos[0], calendars[2]);


	return 0; 
}

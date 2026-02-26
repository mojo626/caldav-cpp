#include "caldav/client.h"
#include "caldav/parseical.h"
#include <iostream>

int main() {
	std::cout << "Hello, World!" << std::endl;

	caldav::Client client("https://calendar.benjaynes.com");
	//caldav::Client client("http://10.0.0.82:5232");

	return 0;
}

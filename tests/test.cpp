#include "caldav/client.h"
#include <iostream>

int main() {
	std::cout << "Hello, World!" << std::endl;

	caldav::Client client("https://calendar.benjaynes.com");
	return 0;
}

#include <string>
#include <iostream>

//https://decovar.dev/blog/2021/03/08/cmake-cpp-library/

namespace caldav {

	class Client {
		public:
			Client(std::string base_url) {
				std::cout << "Base URL: " << base_url << std::endl;
			}
	};
}

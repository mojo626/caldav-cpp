#pragma once
#include <string>
#include <chrono>
#include <sstream>
#include <date/date.h>
#include <iostream>
#include <ctime>

namespace caldav {
    
    struct Event {
        std::string uid;
        std::string summary;
        std::string created;
    };
}
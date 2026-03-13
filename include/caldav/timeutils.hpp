#pragma once
#include "date/date.h"
#include <chrono>
#include <ctime>
#include <iostream>

namespace caldav {

    struct TimeUtils {
        static std::tm to_utc_tm(std::chrono::system_clock::time_point tp) {
            std::time_t tt = std::chrono::system_clock::to_time_t(tp);

            std::tm tm{};
            gmtime_r(&tt, &tm); // thread-safe UTC (macOS & Linux)

            return tm;
        }

        static std::tm to_local_tm(std::chrono::system_clock::time_point tp) {
            std::time_t tt = std::chrono::system_clock::to_time_t(tp);

            std::tm tm{};
            localtime_r(&tt, &tm); // local timezone
            return tm;
        }

        /**
        * @brief Parses a string into a time point using a format string
        * 
        * 
        * @param  data: string to be parsed
        * @param format: format that time is in (see https://en.cppreference.com/w/cpp/chrono/system_clock/from_stream.html for reference on this format string)
        * 
        * @return std::chrono::system_clock::time_point parsed time point
        */
        static std::chrono::system_clock::time_point tm_from_format(std::string data, std::string format) {
            std::istringstream in{data};
			std::chrono::system_clock::time_point tp;

			date::from_stream(in, "%Y%m%dT%H%M%SZ", tp);

			if (in.fail()) {
				std::cerr << ("Failed to parse time: " + data) << std::endl;
			}	
            
            return tp;
        }
    };
	
}
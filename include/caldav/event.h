#pragma once
#include "caldav/timeutils.hpp"
#include "ical.h"
#include <string>
#include <date/date.h>

namespace caldav {
    
    struct Event {
        std::string uid;
        std::string summary;
        icaltimetype dtstamp;
        icaltimetype dtstart;
        icaltimetype dtend;

        // std::tm getStartLocal() {
        //     std::chrono::system_clock::time_point tp;

        //     if (!time_zone.empty()) {
        //         tp = TimeUtils::tp_from_format(dtstart, "%Y%m%dT%H%M%S");
        //     } else {
        //         tp = TimeUtils::tp_from_format(dtstart, "%Y%m%d");
        //     }

        //     std::tm tm = TimeUtils::to_local_tm(tp);
            
        //     return tm;
        // }
    };
}
cmake_minimum_required(VERSION 3.20)
project(caldav-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(caldav
	src/client.cpp
)

target_include_directories(caldav
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)


add_library(caldav::caldav ALIAS caldav)


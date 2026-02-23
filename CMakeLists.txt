cmake_minimum_required(VERSION 3.20)
project(caldav-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(caldav
	src/client.cpp
)

target_include_directories(caldav
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)


add_library(caldav::caldav ALIAS caldav)

include(FetchContent)

FetchContent_Declare(
    pugixml
    GIT_REPOSITORY https://github.com/zeux/pugixml.git
    GIT_TAG v1.15
)

FetchContent_MakeAvailable(pugixml)

FetchContent_Declare(
	libcurl
	GIT_REPOSITORY https://github.com/curl/curl.git
	GIT_TAG curl-8_18_0
)

# Must install openSSL-devel and libpsl-devel packages
FetchContent_MakeAvailable(libcurl)

target_link_libraries(caldav PRIVATE pugixml)
target_link_libraries(caldav PRIVATE libcurl)

# Testing
add_executable(test
    tests/test.cpp
)

target_link_libraries(test
    PRIVATE caldav
)

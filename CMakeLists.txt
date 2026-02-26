cmake_minimum_required(VERSION 3.20)
project(caldav-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(caldav
	src/client.cpp
	src/calendar.cpp
	src/parseical.cpp
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

FetchContent_Declare(
  date_src
  GIT_REPOSITORY https://github.com/HowardHinnant/date.git
  GIT_TAG v3.0.4
)

FetchContent_MakeAvailable(date_src)

file(GLOB UICAL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib/uICAL/src/*.cpp)

add_library(uICAL STATIC
	${UICAL_SRC_FILES}
)

target_include_directories(uICAL 
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/lib/uICAL/src
		#${CMAKE_CURRENT_SOURCE_DIR}/lib/uICAL/src/uICAL
)



target_link_libraries(caldav 
	PRIVATE 
		pugixml 
		libcurl 
		uICAL
		date::date
)

target_include_directories(caldav
    PUBLIC
        ${date_src_SOURCE_DIR}/include
)



# Testing
add_executable(test
    tests/test.cpp
)

target_link_libraries(test
    PRIVATE caldav
)

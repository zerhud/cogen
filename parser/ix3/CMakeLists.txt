cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(ix3 VERSION 0.0.0.0 LANGUAGES C CXX)

option(enable_tests "enable tests" ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)

file(
    GLOB_RECURSE ic_src
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(
    GLOB_RECURSE ic_hdr
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
list(FILTER ic_src EXCLUDE REGEX "tests/.*")
list(FILTER ic_hdr EXCLUDE REGEX "tests/.*")
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/" FILES ${ic_src} ${ic_hdr})

add_library(${PROJECT_NAME} STATIC ${ic_src} ${ic_hdr})
target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs PUBLIC generation_utils)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${Boost_INCLUDE_DIRS}
		"${CMAKE_CURRENT_SOURCE_DIR}"
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/.."
	)

if(enable_tests)
	include(cmake/tests.inc.cmake)
endif()

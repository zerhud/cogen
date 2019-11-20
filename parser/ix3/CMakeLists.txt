cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(ix3 VERSION 0.0.0.0 LANGUAGES C CXX)

option(enable_tests "enable tests" ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(Boost 1.69 REQUIRED)

add_library(${PROJECT_NAME} SHARED parse.hpp)
target_include_directories(
	${PROJECT_NAME}
	PRIVATE ${Boost_INCLUDE_DIRS}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
	)
target_sources(${PROJECT_NAME} PRIVATE ast/common.cpp ast/common.hpp)
target_sources(${PROJECT_NAME} PRIVATE fusion/common.hpp)
target_sources(${PROJECT_NAME} PRIVATE grammar/common.hpp)
target_sources(${PROJECT_NAME} PRIVATE operators/common.cpp operators/common.hpp)
target_sources(${PROJECT_NAME} PRIVATE grammar_def/common.cpp grammar_def/common.hpp)


if(enable_tests)
	include(cmake/tests.inc.cmake)
endif()

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(generation_utils VERSION 0.1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} naming.cpp naming.hpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

if(ENABLE_TESTS)
	find_package(Boost COMPONENTS unit_test_framework REQUIRED)
	add_executable(naming_test "tests/naming.cpp")
	target_link_libraries(naming_test PRIVATE ${PROJECT_NAME} ${Boost_LIBRARIES})
	target_include_directories(naming_test PRIVATE ${turtle} ${Boost_INCLUDE_DIR})
	add_test(NAME naming COMMAND naming_test)
endif()

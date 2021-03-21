cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(builders VERSION 0.0.0.0 LANGUAGES C CXX)

option(enable_tests "enable tests" ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost REQUIRED)

file(
    GLOB_RECURSE src_list
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(
    GLOB_RECURSE hdr_list
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
list(FILTER src_list EXCLUDE REGEX "tests/.*")
list(FILTER hdr_list EXCLUDE REGEX "tests/.*")
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/" FILES ${src_list} ${hdr_list})

add_library(${PROJECT_NAME} STATIC ${src_list} ${hdr_list})
target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs PUBLIC generation_utils)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${Boost_INCLUDE_DIRS}
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/src"
	)


if(enable_tests)
	enable_testing()
	macro(add_unit_test tname path lib)
		set(name ${PROJECT_NAME}_${tname}_test)
		add_executable(${name} "${path}${tname}.cpp")
		target_link_libraries(${name} PRIVATE ${lib} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} generation_utils)
		target_include_directories(${name} SYSTEM PRIVATE
			"${Boost_INCLUDE_DIR}"
			"${turtle}"
			"${CMAKE_CURRENT_SOURCE_DIR}"
			"${CMAKE_CURRENT_SOURCE_DIR}/${path}"
			)
		add_test(NAME ${tname} COMMAND ${name})
		unset(name)
	endmacro()

	add_unit_test(loader "tests/" "${PROJECT_NAME}")
endif()


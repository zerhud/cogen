cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(mdg2 VERSION 0.0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cppjinja REQUIRED)

file(
    GLOB_RECURSE mdg_src
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(
    GLOB_RECURSE mdg_hdr
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
list(FILTER mdg_src EXCLUDE REGEX "tests/.*")
list(FILTER mdg_hdr EXCLUDE REGEX "tests/.*")
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/" FILES ${mdg_src} ${mdg_hdr})

add_executable(${PROJECT_NAME} ${mdg_src} ${mdg_hdr})
if("${CMAKE_BUILD_TYPE} " EQUAL "Debug ")
	target_compile_definitions(${PROJECT_NAME} PUBLIC -DDEBUG)
endif()

target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		input_configurator
		#cppjinja::evaluator
	)


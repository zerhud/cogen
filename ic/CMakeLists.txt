cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(input_configurator VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

add_library("${PROJECT_NAME}" STATIC ${ic_src} ${ic_hdr} ${lohman_json_file})

add_dependencies("${PROJECT_NAME}" generate_nlohman)
set_source_files_properties(${lohman_json_file} PROPERTIES GENERATED TRUE)

target_include_directories("${PROJECT_NAME}" PUBLIC "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries("${PROJECT_NAME}" PUBLIC generation_utils)

if(ENABLE_TESTS)
	find_package(Boost COMPONENTS unit_test_framework REQUIRED)

	macro(add_unit_test tname path lib)
		add_executable(ic_${tname}_test "${path}${tname}_test.cpp")
		target_link_libraries(ic_${tname}_test PRIVATE ${PROJECT_NAME} ${lib} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
		target_include_directories(ic_${tname}_test SYSTEM PRIVATE ${turtle} ${Boost_INCLUDE_DIR} "${CMAKE_CURRENT_LIST_DIR}/${path}")
		add_test(NAME ic_${tname} COMMAND ic_${tname}_test)
	endmacro()

	add_unit_test(core "tests/" "")
endif()

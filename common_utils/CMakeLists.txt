cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(generation_utils VERSION 0.1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(boost_json REQUIRED)

file(
    GLOB_RECURSE src
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(
    GLOB_RECURSE hdr
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
list(FILTER src EXCLUDE REGEX "tests/.*")
list(FILTER hdr EXCLUDE REGEX "tests/.*")
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/" FILES ${src} ${hdr})

add_library(${PROJECT_NAME} ${src} ${hdr})
target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::json)

if(ENABLE_TESTS)
	find_package(Boost COMPONENTS unit_test_framework REQUIRED)

	macro(add_unit_test tname path lib)
		set(src_list "${path}${tname}.cpp" tests/mocks.hpp)
		add_executable(gu_${tname}_test ${src_list})
		source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/${path}" FILES ${src_list})
		target_link_libraries(gu_${tname}_test PRIVATE ${PROJECT_NAME} ${Boost_LIBRARIES} )
		target_include_directories(gu_${tname}_test PRIVATE ${turtle} ${Boost_INCLUDE_DIR})
		add_test(NAME gu_${tname} COMMAND gu_${tname}_test)
	endmacro()

	add_unit_test(naming "tests/" "")
	add_unit_test(input "tests/" "")
	add_unit_test(imports_manager "tests/" "")
endif()

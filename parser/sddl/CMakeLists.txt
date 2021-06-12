cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(sddl
	VERSION 0.0.0.0
	LANGUAGES C CXX
	DESCRIPTION "Structured Data Description Language"
	)

file(
    GLOB_RECURSE sddl_src
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(
    GLOB_RECURSE sddl_hdr
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
list(FILTER sddl_src EXCLUDE REGEX "tests/.*")
list(FILTER sddl_hdr EXCLUDE REGEX "tests/.*")
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/" FILES ${sddl_src} ${sddl_hdr})

include(GNUInstallDirs)
include(GenerateExportHeader)

find_package(Boost 1.75 REQUIRED)
add_library(${PROJECT_NAME} SHARED ${sddl_src} ${sddl_hndl})
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC generation_utils)
#set_target_properties(${PROJECT_NAME} PROPERTIES
    #CXX_VISIBILITY_PRESET hidden
    #VISIBILITY_INLINES_HIDDEN 1
    #)
generate_export_header(${PROJECT_NAME})

if(ENABLE_TESTS)
	find_package(Boost 1.75 COMPONENTS unit_test_framework REQUIRED)
	macro(add_unit_test name)
		add_executable(${PROJECT_NAME}_${name}_test "tests/${name}.cpp")
		target_link_libraries(${PROJECT_NAME}_${name}_test PRIVATE ${PROJECT_NAME} ${Boost_LIBRARIES})
		add_test(NAME ${PROJECT_NAME}-${name} COMMAND ${PROJECT_NAME}_${name}_test)
	endmacro()
	add_unit_test(loader)
endif()

install(TARGETS ${PROJECT_NAME}
	CONFIGURATIONS Release ""
	RUNTIME DESTINATION "${CMAKE_INSTALL_LIBEXECDIR}"
	COMPONENT ${PROJECT_NAME})

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(modegen_parser VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TODO "enable todo warnings in debug mode" ON)

find_package(Boost COMPONENTS program_options unit_test_framework system REQUIRED)
find_package(cppjson REQUIRED)
find_package(Threads REQUIRED)
find_file(turtle turtle/mock.hpp)
get_filename_component(turtle "${turtle}" DIRECTORY)
get_filename_component(turtle "${turtle}" DIRECTORY)

set(config_files_dir "${CMAKE_CURRENT_BINARY_DIR}/cfg")
set(main_config_file "${config_files_dir}/config.hpp")
set(templates_dir "share/modegen")
set(dbg_templates_dir "${config_files_dir}/${templates_dir}")
set(rls_templates_dir "${CMAKE_INSTALL_PREFIX}/${templates_dir}")
set(dbg_cpp_templates_dir "generation/cpp")
set(dbg_cpp_templates_dir "${dbg_templates_dir}/cpp")
set(rls_cpp_templates_dir "${rls_templates_dir}/cpp")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	set(templates_dir_path "${rls_templates_dir}")
	set(cpp_templates_dir "${rls_cpp_templates_dir}")
else()
	set(templates_dir_path "${dbg_templates_dir}")
	set(cpp_templates_dir "${dbg_cpp_templates_dir}")
endif()

configure_file("config.hpp.in" "${main_config_file}")

include(cmake/target_sources_local.cmake)

# interface library: parse interface definitions and generate code by templates
add_library(modegen_interfaces ${main_config_file})
target_link_libraries(modegen_interfaces PRIVATE stdc++fs PUBLIC ${Boost_LIBRARIES} ${CPPJSON_LIBRARIES})
target_include_directories(modegen_interfaces PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${config_files_dir}")
target_sources_local(modegen_interfaces errors.h errors.cpp "")
# interface declaration
target_sources_local(modegen_interfaces parser/loader.hpp parser/loader.cpp "")
target_sources_local(modegen_interfaces grammar.hpp         grammar.cpp         "parser/interface/")
target_sources_local(modegen_interfaces modegen.hpp         modegen.cpp         "parser/interface/")
target_sources_local(modegen_interfaces meta_parameters.hpp meta_parameters.cpp "parser/interface/")
target_sources_local(modegen_interfaces helpers.hpp         helpers.cpp         "parser/interface/")
target_sources_local(modegen_interfaces loader.hpp          loader.cpp          "parser/interface/")
target_sources_local(modegen_interfaces check.hpp           check.cpp           "parser/interface/")
# generations
target_sources_local(modegen_interfaces pythongen.hpp pythongen.cpp "generation/")
target_sources_local(modegen_interfaces provider.hpp  provider.cpp  "generation/")
target_sources_local(modegen_interfaces common.hpp    common.cpp    "generation/")
target_sources_local(modegen_interfaces file_data.hpp file_data.cpp "generation/")
target_sources_local(modegen_interfaces cpp.hpp       cpp.cpp       "generation/")
target_sources(modegen_interfaces PRIVATE
	generation/pythongen
	generation/cpp/declarations.info generation/cpp/declarations.jinja
	generation/cpp/header.jinja generation/cpp/definitions.jinja
	)
# generator part for interface
target_sources_local(modegen_interfaces to_json.hpp to_json.cpp "generation/interface/")

# main programm: run selected generator and sets options
add_executable(modegen modegen/main.cpp)
target_link_libraries(modegen modegen_interfaces ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

# unit tests...
enable_testing()
set(tests_gcommon common cpp to_json)
set(tests_interface
	mdp
	checker
	#name_conversion
	#split_by_version
	#filter
	#common_generators
	)
foreach(tname ${tests_interface})
	add_executable(${tname}_test "tests/interface/${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE modegen_interfaces ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
	target_include_directories(${tname}_test SYSTEM PRIVATE "${turtle}")
	add_test(NAME ${tname} COMMAND ${tname}_test)
endforeach()
foreach(tname ${tests_gcommon})
	add_executable(${tname}_test "tests/generation/${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE modegen_interfaces ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
	target_include_directories(${tname}_test SYSTEM PRIVATE "${turtle}")
	add_test(NAME ${tname} COMMAND ${tname}_test)
endforeach()


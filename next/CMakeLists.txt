cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(modegen_parser VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TODO "enable todo warnings in debug mode" ON)

find_package(Boost COMPONENTS program_options unit_test_framework system REQUIRED)
find_package(cppjson REQUIRED)
find_package(Threads REQUIRED)
#find_package(LibDl  REQUIRED)

set(config_files_dir "${CMAKE_CURRENT_BINARY_DIR}/cfg")
set(main_config_file "${config_files_dir}/config.hpp")
set(templates_dir "share/modegen")
set(dbg_templates_dir "${config_files_dir}/${templates_dir}")
set(rls_templates_dir "${CMAKE_INSTALL_PREFIX}/${config_files_dir}/${templates_dir}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	set(templates_dir_path "${rls_templates_dir}")
else()
	set(templates_dir_path "${dbg_templates_dir}")
endif()

configure_file("config.hpp.in" "${main_config_file}")

# interface library: parse interface definitions and generate code by templates
add_library(modegen_interfaces ${main_config_file})
target_link_libraries(modegen_interfaces PRIVATE stdc++fs PUBLIC ${Boost_LIBRARIES} ${CPPJSON_LIBRARIES})
target_include_directories(modegen_interfaces PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${config_files_dir}")
target_sources(modegen_interfaces PRIVATE errors.h errors.cpp)
# interface declaration
target_sources(modegen_interfaces PRIVATE parser/loader.hpp parser/loader.cpp)
target_sources(modegen_interfaces PRIVATE parser/interface/grammar.hpp parser/interface/grammar.cpp)
target_sources(modegen_interfaces PRIVATE parser/interface/modegen.hpp parser/interface/modegen.cpp)
target_sources(modegen_interfaces PRIVATE parser/interface/meta_parameters.hpp parser/interface/meta_parameters.cpp)
target_sources(modegen_interfaces PRIVATE parser/interface/helpers.hpp parser/interface/helpers.cpp)
target_sources(modegen_interfaces PRIVATE parser/interface/loader.hpp parser/interface/loader.cpp)
target_sources(modegen_interfaces PRIVATE parser/interface/check.hpp parser/interface/check.cpp)
# interface generator
target_sources(modegen_interfaces PRIVATE generation/pythongen.cpp generation/pythongen.hpp generation/pythongen)
target_sources(modegen_interfaces PRIVATE generation/common.hpp generation/common.cpp)
target_sources(modegen_interfaces PRIVATE generation/cpp.hpp generation/cpp.cpp)
target_sources(modegen_interfaces PRIVATE
	generation/cpp/declarations.info generation/cpp/declarations.jinja
	generation/cpp/header.jinja generation/cpp/definitions.jinja
	)

# main programm: run selected generator and sets options
add_executable(modegen modegen/main.cpp)
target_link_libraries(modegen modegen_interfaces ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

# unit tests...
enable_testing()
set(tests_interface mdp checker to_json name_conversion split_by_version filter common_generators)
foreach(tname ${tests_interface})
	#add_executable(${tname}_test "tests/interface/${tname}_test.cpp")
	#target_link_libraries(${tname}_test PRIVATE modegen_interfaces ${Boost_LIBRARIES})
	#add_test(NAME ${tname} COMMAND ${tname}_test)
endforeach()


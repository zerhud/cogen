cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(modegen_parser VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TODO "enable todo warnings in debug mode" ON)

find_package(Boost COMPONENTS program_options unit_test_framework system REQUIRED)
find_package(cppjson REQUIRED)
find_package(pybind11 REQUIRED)
find_package(Threads REQUIRED)
find_file(turtle turtle/mock.hpp)
include(CheckIncludeFile)
get_filename_component(turtle "${turtle}" DIRECTORY)
get_filename_component(turtle "${turtle}" DIRECTORY)

check_include_file("filesystem" HAVE_FILESYSTEM)
set(config_files_dir "${CMAKE_CURRENT_BINARY_DIR}/cfg")
set(main_config_file "${config_files_dir}/config.hpp")
set(templates_dir "share/modegen")
set(dbg_templates_dir "${config_files_dir}/${templates_dir}")
set(rls_templates_dir "${CMAKE_INSTALL_PREFIX}/${templates_dir}")
set(dbg_cpp_templates_dir "generation/cpp")
set(dbg_cpp_templates_dir "${dbg_templates_dir}/cpp")
set(rls_cpp_templates_dir "${rls_templates_dir}/cpp")
set(dbg_cmake_templates_dir "${dbg_templates_dir}/cmake")
set(rls_cmake_templates_dir "${rls_templates_dir}/cmake")

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
set(pythongen_src "${CMAKE_CURRENT_LIST_DIR}/modegen/pythongen")
add_library(modegen_interfaces ${main_config_file})
target_link_libraries(modegen_interfaces PRIVATE stdc++fs PUBLIC ${Boost_LIBRARIES} ${CPPJSON_LIBRARIES})
target_include_directories(modegen_interfaces PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${config_files_dir}")
target_sources_local(modegen_interfaces errors.h errors.cpp "")
# interface declaration
target_sources_local(modegen_interfaces loader.hpp          loader.cpp          "parser/")
target_sources_local(modegen_interfaces grammar.hpp         grammar.cpp         "parser/interface/")
target_sources_local(modegen_interfaces modegen.hpp         modegen.cpp         "parser/interface/")
target_sources_local(modegen_interfaces meta_parameters.hpp meta_parameters.cpp "parser/interface/")
target_sources_local(modegen_interfaces helpers.hpp         helpers.cpp         "parser/interface/")
target_sources_local(modegen_interfaces loader.hpp          loader.cpp          "parser/interface/")
target_sources_local(modegen_interfaces check.hpp           check.cpp           "parser/interface/")
# data_tree
target_sources_local(modegen_interfaces loader.hpp          loader.cpp          "parser/data_tree/")
# generations
target_sources_local(modegen_interfaces provider.hpp  provider.cpp  "generation/")
target_sources_local(modegen_interfaces common.hpp    common.cpp    "generation/")
target_sources_local(modegen_interfaces file_data.hpp file_data.cpp "generation/")
target_sources_local(modegen_interfaces cpp.hpp       cpp.cpp       "generation/")
target_sources_local(modegen_interfaces cmake.hpp     cmake.cpp     "generation/")
target_sources_copy(modegen_interfaces "${dbg_cpp_templates_dir}"
	generation/cpp/declarations.info generation/cpp/render.jinja
	generation/cpp/declarations.jinja generation/cpp/header.jinja generation/cpp/definitions.jinja
	)
target_sources_copy(modegen_interfaces "${dbg_cmake_templates_dir}"
	generation/cmake/plain.jinja
	)
# generation part for interface
target_sources_local(modegen_interfaces to_json.hpp to_json.cpp "generation/interface/")
target_sources_local(modegen_interfaces naming.hpp naming.cpp "generation/interface/")
target_sources_local(modegen_interfaces filter.hpp filter.cpp "generation/interface/")
target_sources_local(modegen_interfaces split_version.hpp split_version.cpp "generation/interface/")
# generation part for cpp
target_sources_local(modegen_interfaces type_converter.hpp type_converter.cpp "generation/cpp/")

# main programm: run selected generator and sets options
add_executable(modegen modegen/main.cpp modegen/pythongen.cpp modegen/pythongen.hpp)
target_sources(modegen PRIVATE "${pythongen_src}" "${CMAKE_CURRENT_BINARY_DIR}/pythongen")
target_sources(modegen PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/pythongen_data.cpp")
target_link_libraries(modegen PRIVATE
	modegen_interfaces
	${Boost_LIBRARIES} pybind11::embed
	${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS}
	)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/pythongen"
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different
		"${pythongen_src}"
		"${CMAKE_CURRENT_BINARY_DIR}/pythongen"
	DEPENDS "${pythongen_src}"
	)
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/pythongen_data.cpp"
	COMMAND "${CMAKE_COMMAND}"
		-Dcnt_list="tscript;jinja_env"
		-Dtscript_file="${CMAKE_CURRENT_LIST_DIR}/modegen/scripts/tmpl_script.py"
		-Djinja_env_file="${CMAKE_CURRENT_LIST_DIR}/modegen/scripts/jinja_env.py"
		-Dinput="${CMAKE_CURRENT_LIST_DIR}/modegen/pythongen_data.cpp.in"
		-Doutput="${CMAKE_CURRENT_BINARY_DIR}/pythongen_data.cpp"
		-P "${CMAKE_CURRENT_LIST_DIR}/cmake/configure_file.cmake"
	DEPENDS
		"${CMAKE_CURRENT_LIST_DIR}/modegen/pythongen_data.cpp.in"
		"${CMAKE_CURRENT_LIST_DIR}/cmake/configure_file.cmake"
		"${CMAKE_CURRENT_LIST_DIR}/modegen/scripts/tmpl_script.py"
		"${CMAKE_CURRENT_LIST_DIR}/modegen/scripts/jinja_env.py"
	)

# unit tests...
enable_testing()
macro(add_unit_test tname path lib)
	add_executable(${tname}_test "${path}${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE ${lib} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
	target_include_directories(${tname}_test SYSTEM PRIVATE "${turtle}")
	add_test(NAME ${tname} COMMAND ${tname}_test)
endmacro()
set(tests_gcommon
	common
	to_json
	cpp cpp_type_cvt
	name_conversion
	filter
	split_version
	cmake
	)
set(tests_interface mdp checker )
foreach(tname ${tests_interface})
	add_unit_test(${tname} "tests/interface/" "modegen_interfaces")
endforeach()
foreach(tname ${tests_gcommon})
	add_unit_test(${tname} "tests/generation/" "modegen_interfaces")
endforeach()
add_unit_test("dt_loader" "tests/data_tree/" "modegen_interfaces")


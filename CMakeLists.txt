cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(modegen_parser VERSION 0.1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TODO "enable todo warnings in debug mode" ON)
option(ENABLE_TESTS "enable tests.. it depends on turtle library" ON)
option(LOW_MEMORY "assume we don't have efough memroty for complie grammars (gcc optimizations)" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Boost COMPONENTS program_options unit_test_framework system REQUIRED)
find_package(pybind11 REQUIRED)
find_package(Threads REQUIRED)
include(CheckCXXSourceCompiles)

CHECK_CXX_SOURCE_COMPILES("#include <filesystem>\nint main(int,char**){return 0;}" HAVE_FILESYSTEM)
if(NOT HAVE_FILESYSTEM)
	message(WARNING "we don't have std::filesystem.. try to use expiremental.\nthis mode is supported only for clang, for build coverage report")
endif()

set(config_files_dir "${CMAKE_CURRENT_BINARY_DIR}/cfg")
set(main_config_file "${config_files_dir}/config.hpp")
set(templates_dir "share/modegen")
set(dbg_templates_dir "${config_files_dir}/${templates_dir}")
set(rls_templates_dir "${CMAKE_INSTALL_PREFIX}/${templates_dir}")
set(dbg_cpp_templates_dir "${dbg_templates_dir}/cpp")
set(rls_cpp_templates_dir "${rls_templates_dir}/cpp")
set(dbg_cmake_templates_dir "${dbg_templates_dir}/cmake")
set(rls_cmake_templates_dir "${rls_templates_dir}/cmake")
set(dbg_python_templates_dir "${dbg_templates_dir}/python")
set(rls_python_templates_dir "${rls_templates_dir}/python")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	set(templates_dir_path "${rls_templates_dir}")
	set(cpp_templates_dir "${rls_cpp_templates_dir}")
	set(python_templates_dir "${rls_python_templates_dir}")
else()
	set(templates_dir_path "${dbg_templates_dir}")
	set(cpp_templates_dir "${dbg_cpp_templates_dir}")
	set(python_templates_dir "${dbg_python_templates_dir}")
endif()

configure_file("config.hpp.in" "${main_config_file}")

include(cmake/target_sources_local.cmake)

add_library(modegen_parsers ${main_config_file})
target_link_libraries(modegen_parsers PRIVATE stdc++fs PUBLIC ${Boost_LIBRARIES})
target_include_directories(modegen_parsers PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/parser"
	"${CMAKE_CURRENT_BINARY_DIR}" "${config_files_dir}")
target_include_directories(modegen_parsers SYSTEM PUBLIC "${Boost_INCLUDE_DIR}")
# grammar need to start compiling first
target_sources_local(modegen_parsers grammar.hpp         grammar.cpp         "parser/interface/")
target_sources_local(modegen_parsers errors.h            errors.cpp          "parser/")
# interface declaration
target_sources_local(modegen_parsers loader.hpp          loader.cpp          "parser/")
target_sources_local(modegen_parsers modegen.hpp         modegen.cpp         "parser/interface/")
target_sources_local(modegen_parsers meta_parameters.hpp meta_parameters.cpp "parser/interface/")
target_sources_local(modegen_parsers helpers.hpp         helpers.cpp         "parser/interface/")
target_sources_local(modegen_parsers loader.hpp          loader.cpp          "parser/interface/")
target_sources_local(modegen_parsers check.hpp           check.cpp           "parser/interface/")
# data_tree
target_sources_local(modegen_parsers loader.hpp          loader.cpp          "parser/data_tree/")
target_sources_local(modegen_parsers ptree_merge.hpp     ""                  "parser/data_tree/")

add_library(part_generation "pg/declarations.hpp" ${main_config_file})
target_include_directories(part_generation PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/pg" "${CMAKE_CURRENT_BINARY_DIR}" "${config_files_dir}")
target_include_directories(part_generation SYSTEM PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(part_generation PRIVATE stdc++fs)

target_sources_local(part_generation "generator.hpp" "generator.cpp" "pg/")
target_sources_local(part_generation "part_descriptor.hpp" "part_descriptor.cpp" "pg/")
target_sources_local(part_generation "info_part.hpp" "info_part.cpp" "pg/")
target_sources_local(part_generation "part_algos.hpp" "part_algos.cpp" "pg/")
target_sources_local(part_generation "module.hpp" "module.cpp" "pg/part_algos/")
target_sources_local(part_generation "data.hpp" "data.cpp" "pg/part_algos/")
target_sources_local(part_generation "to_json.hpp" "to_json.cpp" "pg/part_algos/interface/")
target_sources_local(part_generation "filter.hpp" "filter.cpp" "pg/part_algos/interface/")
target_sources_local(part_generation "naming.hpp" "naming.cpp" "pg/part_algos/interface/")
target_sources_local(part_generation "split_version.hpp" "split_version.cpp" "pg/part_algos/interface/")
target_sources_local(part_generation "output_descriptor.hpp" "output_descriptor.cpp" "pg/")
target_sources_local(part_generation "final_generator.hpp" "final_generator.cpp" "pg/")
target_sources_local(part_generation "generator.hpp" "generator.cpp" "pg/")
target_sources_local(part_generation "part_manager.hpp" "part_manager.cpp" "pg/")
target_sources_local(part_generation "options.hpp" "options.cpp" "pg/")
target_sources_local(part_generation "provider.hpp" "provider.cpp" "pg/")
target_sources_local(part_generation "exceptions.hpp" "exceptions.cpp" "pg/")
target_sources_local(part_generation "cpp.hpp" "cpp.cpp" "pg/langs/")
target_sources_local(part_generation "type_converter.hpp" "type_converter.cpp" "pg/langs/cpp/")
target_sources_local(part_generation "cmake.hpp" "cmake.cpp" "pg/langs/")
target_sources_local(part_generation "python.hpp" "python.cpp" "pg/langs/")
target_sources_copy(part_generation "${dbg_cpp_templates_dir}"
	pg/tmpls/cpp_decl.info pg/tmpls/cpppy.info
	pg/tmpls/cpp/render.jinja pg/tmpls/cpp/declarations.jinja
	pg/tmpls/cpp/header.jinja pg/tmpls/cpp/definitions.jinja
	pg/tmpls/cpp/cpppy.jinja
	)
target_sources_copy(part_generation "${dbg_cmake_templates_dir}"
	pg/tmpls/cmake/plain.jinja
	)
#target_sources_copy(part_generation "${dbg_python_templates_dir}"
	#pg/tmpls/
	#)

# lohman json
set(lohman_json_file "${CMAKE_CURRENT_BINARY_DIR}/nlohman/json.hpp")
set(lohman_json_url "https://github.com/nlohmann/json/raw/develop/single_include/nlohmann/json.hpp")
add_custom_command(OUTPUT "${lohman_json_file}"
	COMMAND "${CMAKE_COMMAND}"
		-Durl="${lohman_json_url}" -Dout="${lohman_json_file}"
		-Denv_var="nlohman_json_header"
		-P "${CMAKE_CURRENT_LIST_DIR}/cmake/download_or_get.cmake"
	DEPENDS
		"${CMAKE_CURRENT_LIST_DIR}/cmake/download_or_get.cmake"
)

#target_sources(modegen_interfaces PRIVATE "${lohman_json_file}" )
target_sources(part_generation PRIVATE "${lohman_json_file}" )

# main programm: run selected generator and sets options
add_executable(modegen modegen/main.cpp)
target_sources(modegen PRIVATE modegen/pythongen.cpp modegen/pythongen.hpp)
target_sources(modegen PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/pythongen_data.cpp")
target_include_directories(modegen SYSTEM PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(modegen PRIVATE
	modegen_parsers part_generation
	${Boost_LIBRARIES} pybind11::embed
	${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS}
	)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/pythongen_data.cpp"
	COMMAND "${CMAKE_COMMAND}"
		-Dcnt_list="tscript;jinja_env"
		-Dtscript_file="${CMAKE_CURRENT_LIST_DIR}/modegen/scripts/tmpl_script.py"
		-Djinja_env_file="${CMAKE_CURRENT_LIST_DIR}/modegen/scripts/jinja_env.py"
		-Dinput="${CMAKE_CURRENT_LIST_DIR}/modegen/pythongen_data.cpp.in"
		-Doutput="${CMAKE_CURRENT_BINARY_DIR}/pythongen_data.cpp"
		-Dtemplates_dir_path="${templates_dir_path}"
		-P "${CMAKE_CURRENT_LIST_DIR}/cmake/configure_file.cmake"
	DEPENDS
		"${CMAKE_CURRENT_LIST_DIR}/modegen/pythongen_data.cpp.in"
		"${CMAKE_CURRENT_LIST_DIR}/cmake/configure_file.cmake"
		"${CMAKE_CURRENT_LIST_DIR}/modegen/scripts/tmpl_script.py"
		"${CMAKE_CURRENT_LIST_DIR}/modegen/scripts/jinja_env.py"
	)

# some files may slow down system when compiling..
if(LOW_MEMORY AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set_source_files_properties("parser/interface/grammar.cpp" PROPERTIES COMPILE_FLAGS "--param ggc-min-expand=1024 --param ggc-min-heapsize=4096")
endif()

if(ENABLE_TESTS)
	include(cmake/tests.inc.cmake)
endif()


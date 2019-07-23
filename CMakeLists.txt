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
else()
	set(templates_dir_path "${dbg_templates_dir}")
	set(cpp_templates_dir "${dbg_cpp_templates_dir}")
endif()

configure_file("config.hpp.in" "${main_config_file}")

include(cmake/target_sources_local.cmake)

# interface library: parse interface definitions and generate code by templates
add_library(modegen_interfaces ${main_config_file})
target_link_libraries(modegen_interfaces PRIVATE stdc++fs PUBLIC ${Boost_LIBRARIES})
target_include_directories(modegen_interfaces PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" "${config_files_dir}")
# grammar need to start compiling first
target_sources_local(modegen_interfaces grammar.hpp         grammar.cpp         "parser/interface/")
target_sources_local(modegen_interfaces errors.h errors.cpp "")
# interface declaration
target_sources_local(modegen_interfaces loader.hpp          loader.cpp          "parser/")
target_sources_local(modegen_interfaces modegen.hpp         modegen.cpp         "parser/interface/")
target_sources_local(modegen_interfaces meta_parameters.hpp meta_parameters.cpp "parser/interface/")
target_sources_local(modegen_interfaces helpers.hpp         helpers.cpp         "parser/interface/")
target_sources_local(modegen_interfaces loader.hpp          loader.cpp          "parser/interface/")
target_sources_local(modegen_interfaces check.hpp           check.cpp           "parser/interface/")
# data_tree
target_sources_local(modegen_interfaces loader.hpp          loader.cpp          "parser/data_tree/")
target_sources_local(modegen_interfaces ptree_merge.hpp     ""                  "parser/data_tree/")
# generations
target_sources_local(modegen_interfaces "generation/declaration.hpp" "" "")
target_sources_local(modegen_interfaces options.hpp   options.cpp   "generation/")
target_sources_local(modegen_interfaces provider.hpp  provider.cpp  "generation/")
target_sources_local(modegen_interfaces common.hpp    common.cpp    "generation/")
target_sources_local(modegen_interfaces file_data.hpp file_data.cpp "generation/")
target_sources_local(modegen_interfaces cpp.hpp       cpp.cpp       "generation/")
target_sources_local(modegen_interfaces python.hpp    python.cpp    "generation/")
target_sources_local(modegen_interfaces cmake.hpp     cmake.cpp     "generation/")
target_sources_local(modegen_interfaces output_info.hpp        output_info.cpp        "generation/")
target_sources_local(modegen_interfaces part_descriptor.hpp    part_descriptor.cpp    "generation/")
target_sources_copy(modegen_interfaces "${dbg_cmake_templates_dir}"
	generation/cmake/plain.jinja
	)
target_sources_copy(modegen_interfaces "${dbg_python_templates_dir}"
	generation/bridges/cpppy.info
	generation/bridges/pycpp.info
	generation/bridges/cpppy.cpp.jinja
	generation/bridges/pycpp.py.jinja
	)
# generation part for interface
target_sources_local(modegen_interfaces to_json.hpp to_json.cpp "generation/interface/")
target_sources_local(modegen_interfaces naming.hpp naming.cpp "generation/interface/")
target_sources_local(modegen_interfaces filter.hpp filter.cpp "generation/interface/")
target_sources_local(modegen_interfaces split_version.hpp split_version.cpp "generation/interface/")
target_sources_local(modegen_interfaces part_descriptor.hpp part_descriptor.cpp "generation/interface/")
# generation part for cpp
target_sources_local(modegen_interfaces type_converter.hpp type_converter.cpp "generation/cpp/")

add_library(part_generation "pg/declarations.hpp" ${main_config_file})
target_include_directories(part_generation PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/pg" "${CMAKE_CURRENT_BINARY_DIR}" "${config_files_dir}")
target_link_libraries(part_generation PRIVATE stdc++fs)

target_sources_local(part_generation "generator.hpp" "generator.cpp" "pg/")
target_sources_local(part_generation "part_descriptor.hpp" "part_descriptor.cpp" "pg/")
target_sources_local(part_generation "info_part.hpp" "info_part.cpp" "pg/")
target_sources_local(part_generation "part_algos.hpp" "part_algos.cpp" "pg/")
target_sources_local(part_generation "module.hpp" "module.cpp" "pg/part_algos/")
target_sources_local(part_generation "to_json.hpp" "to_json.cpp" "pg/part_algos/interface/")
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
	pg/tmpls/cpp_decl.info
	pg/tmpls/cpp/render.jinja pg/tmpls/cpp/declarations.jinja
	pg/tmpls/cpp/header.jinja pg/tmpls/cpp/definitions.jinja
	)

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

target_sources(modegen_interfaces PRIVATE "${lohman_json_file}" )
target_sources(part_generation PRIVATE "${lohman_json_file}" )

# main programm: run selected generator and sets options
add_executable(modegen modegen/main.cpp)
target_sources(modegen PRIVATE modegen/pythongen.cpp modegen/pythongen.hpp)
target_sources(modegen PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/pythongen_data.cpp")
target_link_libraries(modegen PRIVATE
	modegen_interfaces part_generation
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


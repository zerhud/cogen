cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(modegen VERSION 0.1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_TODO "enable todo warnings in debug mode" ON)
option(ENABLE_TESTS "enable tests.. it depends on turtle library" ON)
option(LOW_MEMORY "assume we don't have efough memroty for complie grammars (gcc optimizations)" ON)

# lohman json
set(lohman_json_file "${CMAKE_BINARY_DIR}/nlohman/json.hpp")
set(lohman_json_url "https://github.com/nlohmann/json/raw/develop/single_include/nlohmann/json.hpp")
add_custom_command(OUTPUT ${lohman_json_file}
	COMMAND "${CMAKE_COMMAND}"
		-Durl="${lohman_json_url}" -Dout="${lohman_json_file}"
		-Denv_var="nlohman_json_header"
		-P "${CMAKE_CURRENT_LIST_DIR}/cmake/download_or_get.cmake"
	DEPENDS
		"${CMAKE_CURRENT_LIST_DIR}/cmake/download_or_get.cmake")
add_custom_target(generate_nlohman DEPENDS ${lohman_json_file})

if(ENABLE_TESTS)
	find_file(turtle turtle/mock.hpp)
	get_filename_component(turtle "${turtle}" DIRECTORY)
	get_filename_component(turtle "${turtle}" DIRECTORY)
	enable_testing()
endif()

# turtle uses auto_ptr if not defined
add_compile_definitions(BOOST_NO_AUTO_PTR)
add_subdirectory(common_utils)
add_subdirectory(ic)

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(modegen VERSION 0.1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_TODO "enable todo warnings in debug mode" ON)
option(ENABLE_TESTS "enable tests.. it depends on turtle library" ON)
option(LOW_MEMORY "assume we don't have efough memroty for complie grammars (gcc optimizations)" ON)

if(ENABLE_TESTS)
	find_file(turtle turtle/mock.hpp)
	get_filename_component(turtle "${turtle}" DIRECTORY)
	get_filename_component(turtle "${turtle}" DIRECTORY)
	enable_testing()
endif()

# turtle uses auto_ptr if not defined
# BOOST_BIND_GLOBAL_PLACEHOLDERS for boost warging about bind placehodlers
add_compile_definitions(BOOST_NO_AUTO_PTR BOOST_BIND_GLOBAL_PLACEHOLDERS)
add_subdirectory(common_utils)
add_subdirectory(ic)
add_subdirectory(parser/ix3)
add_subdirectory(parser/standard_types)
add_subdirectory(mdg2)

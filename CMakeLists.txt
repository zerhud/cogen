cmake_minimum_required(VERSION 3.9)
project(modegen_parser VERSION 0.0.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS program_options unit_test_framework REQUIRED)
find_package(cppjson REQUIRED)

set(sources parser/grammar.cpp parser/modegen.cpp parser/check.cpp parser/to_json.cpp parser/split_by_version.cpp parser/helpers.cpp)
set(headers parser/grammar.hpp parser/modegen.hpp parser/check.h   parser/to_json.h   parser/split_by_version.hpp   parser/helpers.hpp)

add_library(modegen_tree ${sources} ${headers})
target_link_libraries(modegen_tree PRIVATE ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} PUBLIC ${cppjson_LIBRARIES})
target_include_directories(modegen_tree PUBLIC parser)

add_executable("${PROJECT_NAME}" parser/main.cpp)
target_link_libraries("${PROJECT_NAME}" PRIVATE modegen_tree)

set(tests mdp checker to_json split_by_version)
enable_testing()

foreach(tname ${tests})
	add_executable(${tname}_test "parser/tests/${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE modegen_tree ${Boost_LIBRARIES})

	add_test(NAME ${tname} COMMAND ${tname}_test)
endforeach()


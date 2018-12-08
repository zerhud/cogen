cmake_minimum_required(VERSION 3.9)
project(modegen_parser VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost COMPONENTS program_options unit_test_framework REQUIRED)
find_package(cppjson REQUIRED)

set(sources parser/grammar.cpp parser/modegen.cpp parser/check.cpp parser/to_json.cpp parser/split_by_version.cpp parser/helpers.cpp parser/generator.cpp)
set(headers parser/grammar.hpp parser/modegen.hpp parser/check.h   parser/to_json.h   parser/split_by_version.hpp parser/helpers.hpp parser/generator.hpp)

add_library(modegen_tree ${sources} ${headers})
target_link_libraries(modegen_tree PRIVATE ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} PUBLIC ${cppjson_LIBRARIES})
target_include_directories(modegen_tree PUBLIC parser)

set(gen_sources generators/defaults.cpp generators/json_tree.cpp)
set(gen_headers generators/defaults.h   generators/json_tree.h)
add_library(modegen_pgens ${gen_sources} ${gen_headers})
target_link_libraries(modegen_pgens PRIVATE modegen_tree ${Boost_LIBRARIES} stdc++fs)
target_include_directories(modegen_pgens PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_sources(modegen_pgens PRIVATE generators/cpp/realization.cpp generators/cpp/realization.h)
target_sources(modegen_pgens PRIVATE generators/cpp/type_converter.cpp generators/cpp/type_converter.h)

add_executable("${PROJECT_NAME}" parser/main.cpp)
target_link_libraries("${PROJECT_NAME}" PRIVATE modegen_tree modegen_pgens)

set(tests mdp checker to_json split_by_version)
enable_testing()

foreach(tname ${tests})
	add_executable(${tname}_test "parser/tests/${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE modegen_tree ${Boost_LIBRARIES})

	add_test(NAME ${tname} COMMAND ${tname}_test)
endforeach()

#add_subdirectory(tests/srv-cpp)

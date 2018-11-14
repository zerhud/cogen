cmake_minimum_required(VERSION 3.9)
project(modegen_parser VERSION 0.0.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS program_options unit_test_framework REQUIRED)

set(sources parser/grammar.cpp parser/modegen.cpp)
set(headers parser/grammar.hpp parser/modegen.hpp)

add_library(modegen_tree ${sources} ${headers})
target_link_libraries(modegen_tree PRIVATE ${Boost_LIBRARIES})
target_include_directories(modegen_tree PUBLIC parser)

add_executable("${PROJECT_NAME}" parser/main.cpp)
target_link_libraries("${PROJECT_NAME}" PRIVATE modegen_tree)

add_executable(mdp_test "parser/tests/mdp_test.cpp")
target_link_libraries(mdp_test PRIVATE modegen_tree ${Boost_LIBRARIES})

enable_testing()
add_test(NAME mdp COMMAND mdp_test)


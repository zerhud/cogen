cmake_minimum_required(VERSION 3.9)
project(modegen_parser VERSION 0.0.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS program_options REQUIRED)

set(sources parser/main.cpp parser/grammar.cpp parser/modegen.cpp)
set(headers parser/grammar.hpp parser/modegen.hpp)

add_executable("${PROJECT_NAME}" ${sources} ${headers})
target_link_libraries("${PROJECT_NAME}" PRIVATE ${Boost_LIBRARIES})


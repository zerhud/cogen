cmake_minimum_required(VERSION 3.9)
project(md_test_cppsrv VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost COMPONENTS program_options REQUIRED)

set(srv_cpp "${CMAKE_CURRENT_BINARY_DIR}/gen/srv.cpp")
set(srv_hpp "${CMAKE_CURRENT_BINARY_DIR}/gen/srv.hpp")
set(def_hpp "${CMAKE_CURRENT_BINARY_DIR}/gen/def.hpp")
set(gen_files ${srv_cpp} ${srv_hpp} ${def_hpp})

add_custom_command(
	OUTPUT ${gen_files}
	COMMAND "modegen_parser" -i ${CMAKE_CURRENT_SOURCE_DIR}/mod.md -t server,cpp -Ogen_empty -o def="${def_hpp}" -o hpp="${srv_hpp}" -o cpp="${srv_cpp}"
	DEPENDS mod.md
	) 
set_source_files_properties(${gen_files} PROPERTIES GENERATED TRUE)

add_executable("${PROJECT_NAME}" srv.cpp  ${gen_files})
target_link_libraries("${PROJECT_NAME}" PRIVATE ${Boost_LIBRARIES})
target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/gen")

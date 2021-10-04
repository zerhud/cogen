cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(cs_test VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(gen_dir "${CMAKE_CURRENT_BINARY_DIR}/gen")
set(cs_headers "${gen_dir}/cs_1_0/module.hpp")
execute_process(
	COMMAND ../../cogen -mflist -iix3=../interface -o ${gen_dir} -gnghttp_srv
	RESULT_VARIABLE cs_sources_result
	OUTPUT_VARIABLE cs_sources_
	)
string(REPLACE "\n" ";" cs_sources_ "${cs_sources_}")
foreach(src ${cs_sources_})
	list(APPEND cs_sources "${gen_dir}/${src}")
endforeach()
add_custom_command(
	OUTPUT ${cs_sources}
	COMMAND ../../cogenwr.py -iix3=../interface -o ${gen_dir} -gnghttp_srv
	)

add_executable(server server.cpp ${cs_headers})
target_include_directories(server PRIVATE "${gen_dir}")


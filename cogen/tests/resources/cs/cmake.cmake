cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(cs_test VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(nghttp REQUIRED)
#find_package(Boost REQUIRED)

set(gen_dir "${CMAKE_CURRENT_BINARY_DIR}/gen/")
execute_process(
	COMMAND ../../cogen
		-mflist --sep=\; "--prefix=${gen_dir}"
		-iix3=../interface -o ${gen_dir} -gnghttp_srv
	RESULT_VARIABLE cs_sources_result
	OUTPUT_VARIABLE cs_sources
	)
add_custom_command(
	OUTPUT ${cs_sources}
	COMMAND ../../cogenwr.py -iix3=../interface -o ${gen_dir} -gnghttp_srv
	)

add_executable(server server.cpp ${cs_sources})
target_include_directories(server PRIVATE "${gen_dir}")


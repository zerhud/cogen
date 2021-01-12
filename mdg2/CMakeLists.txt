cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(mdg2 VERSION 0.0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cppjinja REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

file(
    GLOB_RECURSE mdg_src
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(
    GLOB_RECURSE mdg_hdr
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
list(FILTER mdg_src EXCLUDE REGEX "tests/.*")
list(FILTER mdg_hdr EXCLUDE REGEX "tests/.*")
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/" FILES ${mdg_src} ${mdg_hdr})

add_executable(${PROJECT_NAME} ${mdg_src} ${mdg_hdr})
if("${CMAKE_BUILD_TYPE} " STREQUAL "Debug ")
	message("== build in debug mode")
	target_compile_definitions(${PROJECT_NAME} PUBLIC -DDEBUG)
endif()

set(build_lib_dir "${CMAKE_CURRENT_BINARY_DIR}/etc/${PROJECT_NAME}/lib")
set(build_examples_dir "${CMAKE_CURRENT_BINARY_DIR}/etc/${PROJECT_NAME}/examples")
set(build_generators_dir "${CMAKE_CURRENT_BINARY_DIR}/etc/${PROJECT_NAME}/generators")

macro(copy_if_diff from to)
	add_custom_command(
		OUTPUT "${to}"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${from}" "${to}"
		DEPENDS "${from}" )
	target_sources(${PROJECT_NAME} PRIVATE  "${to}")
endmacro()

copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/mdgwr.py" "${CMAKE_CURRENT_BINARY_DIR}/mdgwr.py")

# generate ix3 example file
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/examples/ix3_example" "${build_examples_dir}/ix3_example")

# generators
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/generators/cpp_decl.info" "${build_generators_dir}/cpp_decl.info")

# library
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/standard_types.info" "${build_lib_dir}/standard_types.info")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cpp/declarations.jinja" "${build_lib_dir}/cpp/declarations.jinja")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cpp/header.jinja" "${build_lib_dir}/cpp/header.jinja")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cpp/render.jinja" "${build_lib_dir}/cpp/render.jinja")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cpp/definitions.jinja" "${build_lib_dir}/cpp/definitions.jinja")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cmake/plain.jinja" "${build_lib_dir}/cmake/plain.jinja")

target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		${Boost_LIBRARIES}
		input_configurator ix3 standard_types
		cppjinja::evaluator cppjinja::loader
	)

if(ENABLE_TESTS)
	enable_testing()
	add_test(NAME mdg2_cpp_decl COMMAND python3 -m pytest "${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp_decl.py")
endif()

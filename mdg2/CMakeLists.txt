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

set(build_examples_dir "${CMAKE_CURRENT_BINARY_DIR}/etc/${PROJECT_NAME}/examples")
set(build_templates_dir "${CMAKE_CURRENT_BINARY_DIR}/etc/${PROJECT_NAME}/tmpls")
set(build_generators_dir "${CMAKE_CURRENT_BINARY_DIR}/etc/${PROJECT_NAME}/generators")

# generate ix3 example file
add_custom_command(
	OUTPUT "${build_examples_dir}/ix3_example"
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different
		"${CMAKE_CURRENT_LIST_DIR}/examples/ix3_example"
		"${build_examples_dir}/ix3_example"
	DEPENDS "examples/ix3_example" )
target_sources(${PROJECT_NAME} PRIVATE  "${build_examples_dir}/ix3_example")

# generators
add_custom_command(
	OUTPUT "${build_generators_dir}/cpp_decl.info"
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different
		"${CMAKE_CURRENT_LIST_DIR}/generators/cpp_decl.info"
		"${build_generators_dir}/cpp_decl.info"
	DEPENDS "generators/cpp_decl.info" )
target_sources(${PROJECT_NAME} PRIVATE  "${build_generators_dir}/cpp_decl.info")

target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		${Boost_LIBRARIES}
		input_configurator ix3
		#cppjinja::evaluator
	)

if(ENABLE_TESTS)
	enable_testing()
	add_test(NAME mdg2_cpp_decl COMMAND python3 -m pytest "${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp_decl.py")
endif()

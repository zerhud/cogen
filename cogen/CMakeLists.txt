cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
set(build_version $ENV{build_version})
if(NOT build_version})
	set(build_version 0)
endif()
project(cogen VERSION 0.2.1.${build_version} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS   ON)
find_package(cppjinja 0.0.2 COMPONENTS evaluator loader REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

file(
    GLOB_RECURSE cogen_src
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(
    GLOB_RECURSE cogen_hdr
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
list(FILTER cogen_src EXCLUDE REGEX "tests/.*")
list(FILTER cogen_hdr EXCLUDE REGEX "tests/.*")
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/" FILES ${cogen_src} ${cogen_hdr})

configure_file("main.cpp.in" "main.cpp")
set(cogen_src_gen "${CMAKE_CURRENT_BINARY_DIR}/main.cpp")
source_group(TREE "${CMAKE_CURRENT_BINARY_DIR}/" FILES ${cogen_src_gen})

add_executable(${PROJECT_NAME} ${cogen_src} ${cogen_hdr} ${cogen_src_gen})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
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

copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/cogenwr.py" "${CMAKE_CURRENT_BINARY_DIR}/cogenwr.py")

# generate ix3 example file
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/examples/ix3_example" "${build_examples_dir}/ix3_example")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/examples/ix3_example_inc_dir" "${build_examples_dir}/incs/ix3_example_inc_dir")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/examples/ix3_errors" "${build_examples_dir}/ix3_errors")

# generators
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/generators/cpp_decl.info" "${build_generators_dir}/cpp_decl.info")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/generators/cpp_py.info" "${build_generators_dir}/cpp_py.info")

# library
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/standard_types.info" "${build_lib_dir}/standard_types.info")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cpp/declarations.jinja" "${build_lib_dir}/cpp/declarations.jinja")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cpp/header.jinja" "${build_lib_dir}/cpp/header.jinja")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cpp/render.jinja" "${build_lib_dir}/cpp/render.jinja")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cpp/definitions.jinja" "${build_lib_dir}/cpp/definitions.jinja")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cpp/cpppy.jinja" "${build_lib_dir}/cpp/cpppy.jinja")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cpp/cpppy_header.jinja" "${build_lib_dir}/cpp/cpppy_header.jinja")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cmake/plain.jinja" "${build_lib_dir}/cmake/plain.jinja")
copy_if_diff("${CMAKE_CURRENT_LIST_DIR}/lib/cmake/pybind11.jinja" "${build_lib_dir}/cmake/pybind11.jinja")

target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		${Boost_LIBRARIES} ${CMAKE_DL_LIBS}
		input_configurator ix3 standard_types builders
		cppjinja::evaluator cppjinja::loader
		-static-libgcc -static-libstdc++
	)

if(ENABLE_TESTS AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	enable_testing()
	add_test(NAME cogen_cpp_decl COMMAND python3 -m pytest "${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp_decl.py")
	add_test(NAME cogen_results COMMAND python3 -m pytest "${CMAKE_CURRENT_SOURCE_DIR}/tests/results.py")
	copy_if_diff(
		"${CMAKE_CURRENT_LIST_DIR}/tests/resources/__init__.py"
		"${CMAKE_CURRENT_BINARY_DIR}/__init__.py")
endif()

install(TARGETS ${PROJECT_NAME}
	CONFIGURATIONS Release ""
	RUNTIME DESTINATION bin
	COMPONENT cogen)
install(PROGRAMS "${CMAKE_CURRENT_LIST_DIR}/cogenwr.py"
	DESTINATION bin
	COMPONENT cogen)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/etc/"
	DESTINATION etc
	COMPONENT library)

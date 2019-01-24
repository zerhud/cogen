cmake_minimum_required(VERSION 3.9)
project(modegen_parser VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TODO "enable todo warnings in debug mode" ON)

find_package(Boost COMPONENTS program_options unit_test_framework system REQUIRED)
find_package(cppjson REQUIRED)
find_package(Threads REQUIRED)

set(config_file_dir "${CMAKE_CURRENT_BINARY_DIR}")
set(config_file "${config_file_dir}/config.hpp")
configure_file("config.hpp.in" "${config_file}")

set(sources parser/grammar.cpp parser/modegen.cpp parser/check.cpp
    parser/loader.cpp parser/helpers.cpp
    parser/errors.cpp
    )
set(headers parser/grammar.hpp parser/modegen.hpp parser/check.h
    parser/loader.h   parser/helpers.hpp
    parser/errors.h
    )

add_library(modegen_tree ${config_file} ${sources} ${headers})
target_link_libraries(modegen_tree PRIVATE ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
target_include_directories(modegen_tree PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/parser" "${config_file_dir}")

set(gen_sources generators/generator.cpp generators/defaults.cpp generators/to_json.cpp generators/json_tree.cpp)
set(gen_headers generators/generator.hpp generators/defaults.h   generators/to_json.h   generators/json_tree.h)
add_library(modegen_pgens ${config_file} ${gen_sources} ${gen_headers})
target_link_libraries(modegen_pgens PRIVATE modegen_tree ${Boost_LIBRARIES} stdc++fs PUBLIC ${cppjson_LIBRARIES})
target_include_directories(modegen_pgens PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${config_file_dir}")
target_include_directories(modegen_pgens PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/generators" "${config_file_dir}")
target_sources(modegen_pgens PRIVATE generators/pythongen.cpp generators/pythongen.cpp)
target_sources(modegen_pgens PRIVATE generators/split_by_version.cpp generators/split_by_version.hpp)
target_sources(modegen_pgens PRIVATE generators/converters/case.cpp generators/converters/case.h)
target_sources(modegen_pgens PRIVATE generators/cpp/realization.cpp generators/cpp/realization.h)
target_sources(modegen_pgens PRIVATE generators/cpp/interface.cpp generators/cpp/interface.h)
target_sources(modegen_pgens PRIVATE generators/cpp/type_converter.cpp generators/cpp/type_converter.h)
target_sources(modegen_pgens PRIVATE generators/cpp/declarations.hpp.jinja generators/cpp/realization.hpp.jinja generators/cpp/realization.cpp.jinja)

set(templates_dir "${CMAKE_CURRENT_BINARY_DIR}/tmpls")
set(cpp_templates_dir "${CMAKE_CURRENT_SOURCE_DIR}/generators/cpp")
set(binary_dir "${CMAKE_CURRENT_BINARY_DIR}")
configure_file("generators/config.hpp.in" "generators/config.hpp")
target_sources(modegen_pgens PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generators/config.hpp")
target_include_directories(modegen_pgens PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(modegen_pgens INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/generators")

add_executable("${PROJECT_NAME}" ${config_file} parser/main.cpp)
target_link_libraries("${PROJECT_NAME}" PRIVATE modegen_tree modegen_pgens ${CMAKE_THREAD_LIBS_INIT})

set(pygen_in "${CMAKE_CURRENT_SOURCE_DIR}/generators/pythongen")
set(pygen_out "${CMAKE_CURRENT_BINARY_DIR}/pythongen")
add_custom_command(
    TARGET "${PROJECT_NAME}" PRE_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${pygen_in}" "${pygen_out}"
    #DEPENDS "${pygen_in}"
    )


set(tests mdp checker)
set(generators_tests to_json name_conversion split_by_version filter common_generators)
enable_testing()
foreach(tname ${tests})
	add_executable(${tname}_test "parser/tests/${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE modegen_tree ${Boost_LIBRARIES})

	add_test(NAME ${tname} COMMAND ${tname}_test)
endforeach()
foreach(tname ${generators_tests})
	add_executable(${tname}_test "generators/tests/${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE modegen_pgens ${Boost_LIBRARIES})
	add_test(NAME ${tname} COMMAND ${tname}_test)
endforeach()

add_subdirectory(tests/srv-cpp)

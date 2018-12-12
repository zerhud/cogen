cmake_minimum_required(VERSION 3.9)
project(modegen_parser VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost COMPONENTS program_options unit_test_framework REQUIRED)
find_package(cppjson REQUIRED)

set(sources parser/grammar.cpp parser/modegen.cpp parser/check.cpp
    parser/to_json.cpp parser/split_by_version.cpp parser/helpers.cpp parser/generator.cpp
    parser/errors.cpp
    )
set(headers parser/grammar.hpp parser/modegen.hpp parser/check.h
    parser/to_json.h   parser/split_by_version.hpp parser/helpers.hpp parser/generator.hpp
    parser/errors.h
    )

add_library(modegen_tree ${sources} ${headers})
target_link_libraries(modegen_tree PRIVATE ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} PUBLIC ${cppjson_LIBRARIES})
target_include_directories(modegen_tree PUBLIC parser)

set(gen_sources generators/defaults.cpp generators/json_tree.cpp)
set(gen_headers generators/defaults.h   generators/json_tree.h)
add_library(modegen_pgens ${gen_sources} ${gen_headers})
target_link_libraries(modegen_pgens PRIVATE modegen_tree ${Boost_LIBRARIES} stdc++fs)
target_include_directories(modegen_pgens PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_sources(modegen_pgens PRIVATE generators/case_converter.cpp generators/case_converter.h)
target_sources(modegen_pgens PRIVATE generators/cpp/realization.cpp generators/cpp/realization.h)
target_sources(modegen_pgens PRIVATE generators/cpp/type_converter.cpp generators/cpp/type_converter.h)

set(templates_dir "${CMAKE_CURRENT_BINARY_DIR}/tmpls")
set(cpp_templates_dir "${CMAKE_CURRENT_SOURCE_DIR}/generators/cpp")
set(binary_dir "${CMAKE_CURRENT_BINARY_DIR}")
configure_file("generators/config.hpp.in" "generators/config.hpp")
target_sources(modegen_pgens PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generators/config.hpp")
target_include_directories(modegen_pgens PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(modegen_pgens INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/generators")

add_executable("${PROJECT_NAME}" parser/main.cpp)
target_link_libraries("${PROJECT_NAME}" PRIVATE modegen_tree modegen_pgens)

set(pygen_in "${CMAKE_CURRENT_SOURCE_DIR}/generators/pythongen")
set(pygen_out "${CMAKE_CURRENT_BINARY_DIR}/pythongen")
add_custom_command(
    TARGET "${PROJECT_NAME}" PRE_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${pygen_in}" "${pygen_out}"
    #DEPENDS "${pygen_in}"
    )


set(tests mdp checker to_json split_by_version)
set(generators_tests name_conversion)
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

if("${turtle} " STREQUAL " ")
	find_file(turtle turtle/mock.hpp)
	get_filename_component(turtle "${turtle}" DIRECTORY)
	get_filename_component(turtle "${turtle}" DIRECTORY)
	set(turtle "${turtle}" CACHE PATH "path to turtle framework")
endif()

find_package(Boost 1.69 COMPONENTS unit_test_framework REQUIRED)

# unit tests...
enable_testing()
macro(add_unit_test tname path lib)
	add_executable(${tname}_test "${path}ix3_${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE ${lib} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
	target_include_directories(${tname}_test SYSTEM PRIVATE "${Boost_INCLUDE_DIR}" "${turtle}" "${CMAKE_CURRENT_SOURCE_DIR}/${path}")
	add_test(NAME ${tname} COMMAND ${tname}_test)
endmacro()

set(tests_parser rules_common)
foreach(tname ${tests_parser})
	add_unit_test(${tname} "tests/" "${PROJECT_NAME}")
endforeach()


set(Boost_USE_STATIC_LIBS   OFF)
find_package(Boost 1.69 COMPONENTS unit_test_framework REQUIRED)

# unit tests...
enable_testing()
macro(add_unit_test tname path lib)
	add_executable(${tname}_test "${path}ix3_${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE ${lib} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} generation_utils)
	target_include_directories(${tname}_test SYSTEM PRIVATE
		"${Boost_INCLUDE_DIR}"
		"${turtle}"
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}/${path}"
		)
	add_test(NAME ${tname} COMMAND ${tname}_test)
endmacro()

set(tests_parser
	rules_common
	rules_meta
	rules_functions
	rules_entity
	rules_file
	utils_checker
	utils_to_generic_ast
	utils_parser
	)
foreach(tname ${tests_parser})
	add_unit_test(${tname} "tests/" "${PROJECT_NAME}")
endforeach()


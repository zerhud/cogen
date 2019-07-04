find_file(turtle turtle/mock.hpp)
get_filename_component(turtle "${turtle}" DIRECTORY)
get_filename_component(turtle "${turtle}" DIRECTORY)

# unit tests...
enable_testing()
macro(add_unit_test tname path lib)
	add_executable(${tname}_test "${path}${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE ${lib} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
	target_include_directories(${tname}_test SYSTEM PRIVATE "${turtle}")
	add_test(NAME ${tname} COMMAND ${tname}_test)
endmacro()
set(tests_gcommon
	options
	common
	outputs
	part_descriptors
	to_json
	cpp cpp_type_cvt
	name_conversion
	filter
	split_version
	cmake
	)
set(tests_interface mdp checker )
set(tests_pg uc)
foreach(tname ${tests_interface})
	add_unit_test(${tname} "tests/interface/" "modegen_interfaces")
endforeach()
foreach(tname ${tests_gcommon})
	add_unit_test(${tname} "tests/generation/" "modegen_interfaces")
endforeach()
foreach(tname ${tests_pg})
	add_unit_test(${tname} "tests/pg/" "part_generation")
endforeach()
add_unit_test("dt_loader" "tests/data_tree/" "modegen_interfaces")


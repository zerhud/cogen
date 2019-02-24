separate_arguments(cnt_list)
foreach(cnt ${cnt_list})
	file(READ "${${cnt}_file}" ${cnt}_cnt)
endforeach()

configure_file("${input}" "${output}" @ONLY)


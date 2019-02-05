function(target_sources_local target header source prefix)
	target_sources(${target}
		PUBLIC
			"$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${prefix}${header}>"
			"$<INSTALL_INTERFACE:${prefix}${header}>"
		PRIVATE
			"${prefix}${source}"
		)
endfunction()


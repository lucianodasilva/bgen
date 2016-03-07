

foreach (mod ${bgen_modules})
	if ( NOT ${mod} STREQUAL "bgen_core")
		string (CONCAT BGEN_LANGUAGE_PLUGIN_DEF "extern base_language_plugin * make_${mod}_plugin ();")
		string (CONCAT BGEN_LANGUAGE_PLUGIN_LIST "make_${mod}_plugin (),")
	endif()
endforeach ()

configure_file (
	${CMAKE_CURRENT_LIST_DIR}/bgen_config.h.in 
	${CMAKE_CURRENT_BINARY_DIR}/bgen_config.h
)

include_directories (${CMAKE_CURRENT_BINARY_DIR})

# include dependencies
include_directories (${CMAKE_CURRENT_LIST_DIR}/deps/cmd_args/include)

if (APPLE)
    add_custom_command (TARGET bgen
        POST_BUILD
        COMMAND install_name_tool
        "-change"
        "@rpath/libclang.dylib"
        "@loader_path/libclang.dylib"
        "$<TARGET_FILE:${PROJECT_NAME}>"
    )
endif()

if (UNIX OR APPLE)

    install(
        TARGETS bgen 
        RUNTIME DESTINATION bgen
        CONFIGURATIONS Release
    )

    # create bin symlink
    install (
        CODE "EXECUTE_PROCESS(COMMAND ln -sf ${CMAKE_INSTALL_PREFIX}/bgen/bgen /usr/local/bin/bgen)"
    )
endif ()
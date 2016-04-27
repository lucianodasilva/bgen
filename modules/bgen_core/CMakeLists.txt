
include(ExternalProject)

set (BGEN_LIBCLANG_INCLUDE_PATH "" CACHE PATH "libClang include path")
set (BGEN_LIBCLANG_LIB_PATH "" CACHE PATH "libClang lib path")

include_directories (${BGEN_LIBCLANG_INCLUDE_PATH})
link_directories(${BGEN_LIBCLANG_LIB_PATH})

if (MSVC)
	add_definitions(
		-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1
		-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1
		-D_CRT_SECURE_NO_WARNINGS=1
	)

	set (BGEN_LIBCLANG_DLL_PATH "" CACHE FILEPATH "libClang dll file")
	add_custom_command (TARGET bgen_core 
		POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BGEN_LIBCLANG_DLL_PATH} $<TARGET_FILE_DIR:bgen_core>
	)

	list(APPEND core_dependencies libClang)
else ()
	if (APPLE)
	    add_custom_command (TARGET bgen_core 
			POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BGEN_LIBCLANG_LIB_PATH}/libclang.dylib $<TARGET_FILE_DIR:bgen_core>
		)
        install (
            FILES ${BGEN_LIBCLANG_LIB_PATH}/libclang.dylib 
            DESTINATION /usr/local/bgen/
            CONFIGURATIONS Release
        )
	endif ()
	
	list (APPEND core_dependencies clang)  
endif()

add_module_dependencies (bgen_core ${core_dependencies})


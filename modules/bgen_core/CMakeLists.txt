
include_directories (
	${CMAKE_CURRENT_LIST_DIR}/deps/GSL
)

link_directories(${BGEN_LIBCLANG_LIB_PATH})

if (MSVC)
	add_definitions(
		-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1
		-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1
		-D_CRT_SECURE_NO_WARNINGS=1
	)
endif()

add_module_dependencies (bgen_core ${core_dependencies})

include(${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt)
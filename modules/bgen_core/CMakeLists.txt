
include_directories (
	${CMAKE_CURRENT_LIST_DIR}/deps/GSL
)

link_directories(${BGEN_LIBCLANG_LIB_PATH})

if (MSVC)
	# add natvis definitions
	target_sources(bgen_core PUBLIC ${bgen_extra_path}/vs/bgen.natvis)
endif()

add_module_dependencies (bgen_core ${core_dependencies})

include(${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt)
cmake_minimum_required(VERSION 3.0)

project (bgen)

set ( bgen_path ${CMAKE_CURRENT_LIST_DIR})

include (${bgen_path}/cmake/environment.cmake)
include (${bgen_path}/cmake/modules.cmake)

if (MSVC)
	add_definitions(
		-D_SCL_SECURE_NO_WARNINGS=1
	)

endif()

# add test infrastructure
set ( bgen_gtest_path ${CMAKE_CURRENT_LIST_DIR}/extra/googletest )
add_subdirectory(${bgen_gtest_path})

# setup environment
check_environment ()

#fix runtime to static in windows
set_runtime_to_static ()

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# -----------------
set (bgen_extra_path ${CMAKE_CURRENT_LIST_DIR}/extra)

add_static_module (bgen_core ${bgen_path}/modules/bgen_core)
add_static_module (bgen_clang ${bgen_path}/modules/bgen_clang bgen_core)
#add_static_module (javabgen ${bgen_path}/modules/javabgen bgen_core)
#add_static_module (casabgen ${bgen_path}/modules/casabgen bgen_core)
#add_executable_module (bgen ${bgen_path}/modules/bgen bgen_core bgen_clang casabgen)
add_executable_module (bgen ${bgen_path}/modules/bgen bgen_core bgen_clang)

project (bgen)

cmake_minimum_required(VERSION 3.0)

project (bgen)

set ( bgen_path ${CMAKE_CURRENT_LIST_DIR})

include (${bgen_path}/cmake/environment.cmake)
include (${bgen_path}/cmake/modules.cmake)

# setup environment
check_environment ()

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# -----------------

add_static_module (bgen_core ${bgen_path}/modules/bgen_core)
#add_static_module (javabgen ${bgen_path}/modules/javabgen bgen_core)
add_static_module (casabgen ${bgen_path}/modules/casabgen bgen_core)
add_executable_module (bgen ${bgen_path}/modules/bgen bgen_core casabgen)

project (bgen)

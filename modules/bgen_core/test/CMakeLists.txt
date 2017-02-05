
option(BGEN_CORE_RUN_TESTS "Run bgen core tests")

if (${BGEN_CORE_RUN_TESTS})

    glob_code (${CMAKE_CURRENT_LIST_DIR} bgen_test_sources)

    project(bgen_core_test)

    include(CTest)
    enable_testing()

    add_executable(bgen_core_test ${bgen_test_sources})
    target_link_libraries(bgen_core_test gtest gmock bgen_core ${core_dependencies})

endif()
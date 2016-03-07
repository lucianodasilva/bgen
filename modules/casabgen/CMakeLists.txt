include (ExternalProject)

add_custom_command (TARGET casabgen 
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:casabgen>/resources/casablanca
)

install (
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/resources DESTINATION bgen
)
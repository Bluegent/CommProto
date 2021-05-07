project(DiagnosticsService)

include (${CMAKE_SOURCE_DIR}/cmake/utility.cmake)

file(GLOB_RECURSE PROJECT_SRC RELATIVE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        *.cpp
        *.h
        )

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

target_link_libraries(${PROJECT_NAME} CommProtoService)

include_directories(.)

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})

SET(CONFIG_FILE diag.cfg)
installConfig(${CONFIG_FILE} ${INSTALL_DIR})
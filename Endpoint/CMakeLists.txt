project(EndpointSimulator)

include (${CMAKE_SOURCE_DIR}/cmake/utility.cmake)

add_executable(${PROJECT_NAME} 
    endpoint_main.cpp
)

target_link_libraries(${PROJECT_NAME} CommProtoService Rotary)

include_directories(.)

set(INSTALL_DIR "${CMAKE_BINARY_DIR}/../install")

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})

SET(CONFIG_FILE endpoint.cfg)
installConfig(${CONFIG_FILE} ${INSTALL_DIR})

##set_executable_path(${PROJECT_NAME}) 
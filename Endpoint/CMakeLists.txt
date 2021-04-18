project(EndpointSimulator)

include (${CMAKE_SOURCE_DIR}/cmake/utility.cmake)

add_executable(${PROJECT_NAME} 
    endpoint_main.cpp
)

target_link_libraries(${PROJECT_NAME} CommProtoService)

include_directories(.)

set(INSTALL_DIR "${CMAKE_BINARY_DIR}/../install")

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES config/endpoint.cfg DESTINATION ${INSTALL_DIR})

##set_executable_path(${PROJECT_NAME}) 
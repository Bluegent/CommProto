project(AuthService)

include (${CMAKE_SOURCE_DIR}/cmake/utility.cmake)

add_executable(${PROJECT_NAME} 
    auth_main.cpp
)

target_link_libraries(${PROJECT_NAME} CommProtoService)

include_directories(.)

set(INSTALL_DIR "${CMAKE_BINARY_DIR}/../install")

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})

install(FILES config/auth.cfg DESTINATION ${INSTALL_DIR})

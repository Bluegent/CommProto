project(Rotary)

include (${CMAKE_SOURCE_DIR}/cmake/utility.cmake)

file(GLOB_RECURSE PROJECT_SRC RELATIVE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        *.cpp
        *.h
        )

add_library(${PROJECT_NAME} SHARED ${PROJECT_SRC})

target_link_libraries(${PROJECT_NAME} CommProtoService CommProtoPlugin)
target_include_directories(${PROJECT_NAME} INTERFACE interface)

include_directories(.)
include_directories(interface)

set(INSTALL_DIR "${CMAKE_BINARY_DIR}/../install")

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})

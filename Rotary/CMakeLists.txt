project(Rotary)

find_package(Poco "1.10.1" REQUIRED COMPONENTS Net Util Crypto)
find_package(OpenSSL REQUIRED)

include (${CMAKE_SOURCE_DIR}/cmake/utility.cmake)

file(GLOB_RECURSE PROJECT_SRC RELATIVE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        *.cpp
        *.h
        )

add_library(${PROJECT_NAME} SHARED ${PROJECT_SRC})

target_link_libraries(${PROJECT_NAME} CommProtoService)
include_directories(.)

set(INSTALL_DIR "${CMAKE_BINARY_DIR}/../install")

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})

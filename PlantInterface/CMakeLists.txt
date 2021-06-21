project(PlantInterface)

include (${CMAKE_SOURCE_DIR}/cmake/utility.cmake)

file(GLOB_RECURSE PROJECT_SRC RELATIVE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        *.cpp
        *.h
        )

add_library(${PROJECT_NAME} STATIC ${PROJECT_SRC})


target_link_libraries(${PROJECT_NAME} CommProto)

target_include_directories(${PROJECT_NAME} PUBLIC .)

include_directories(.)

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})
project(PlantHealthService)

include (${CMAKE_SOURCE_DIR}/cmake/utility.cmake)

find_package(Poco "1.10.1" REQUIRED COMPONENTS Net Util)


file(GLOB_RECURSE PROJECT_SRC RELATIVE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        *.cpp
        *.h
        )

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

target_link_libraries(${PROJECT_NAME} CommProtoService PlantInterface Poco::Util Poco::Net)

include_directories(.)

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})

SET(CONFIG_FILE plant.cfg)
installConfig(${CONFIG_FILE} ${INSTALL_DIR})
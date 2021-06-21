cmake_minimum_required(VERSION 3.19)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(CommProtoProject)

set(INSTALL_DIR "${CMAKE_BINARY_DIR}/../install" CACHE PATH "Path to install location")

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_FOLDER "ThirdParty")
add_subdirectory(gtest)
add_subdirectory(ConcurrentQueue)
add_subdirectory(rapidjson)
IF(WIN32)
    add_subdirectory(poco)
endif()

set(CMAKE_FOLDER "Libs")
add_subdirectory(CommProtoLib)
add_subdirectory(CommProtoPlugin)
add_subdirectory(CommProtoService)
add_subdirectory(CommProtoDevice)
add_subdirectory(PlantInterface)

set(CMAKE_FOLDER "Plugins")
add_subdirectory(Rotary)

set(CMAKE_FOLDER "Service")
add_subdirectory(Dispatch)
add_subdirectory(DiagnosticsService)
add_subdirectory(UXService)
add_subdirectory(AuthService)
add_subdirectory(PlantHealth)

set(CMAKE_FOLDER "Endpoints")
add_subdirectory(Endpoint)
add_subdirectory(PlantTester)
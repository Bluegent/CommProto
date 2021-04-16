cmake_minimum_required(VERSION 3.19)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(CommProtoProject)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../install")
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
add_subdirectory(CommProtoService)
add_subdirectory(CommProtoDevice)

set(CMAKE_FOLDER "Runnable")
add_subdirectory(Dispatch)
add_subdirectory(Endpoint)
add_subdirectory(UXService)
add_subdirectory(AuthService)
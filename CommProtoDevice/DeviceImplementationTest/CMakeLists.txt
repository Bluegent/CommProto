project(DeviceImplementationTest)

add_executable(${PROJECT_NAME} DeviceImplementation.cpp)
target_link_libraries(${PROJECT_NAME} CommProtoDevice)
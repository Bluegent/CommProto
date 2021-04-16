project(CommProtoDevice)

file(GLOB_RECURSE PROJECT_SRC RELATIVE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        interface/*.cpp
        interface/*.h
        src/*.cpp
        src/*.h
        )

add_library(${PROJECT_NAME} STATIC ${PROJECT_SRC})

foreach(source IN LISTS PROJECT_SRC)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()  

target_link_libraries(${PROJECT_NAME} CommProto)

target_include_directories(${PROJECT_NAME} INTERFACE interface ${PD_INTERFACE_FOLDER})
include_directories(interface)

add_subdirectory(DeviceImplementationTest)
project (CommProtoPlugin)

if(UNIX)
    SET(PD_BASE "${PROJECT_SOURCE_DIR}/pd/unix")
elseif(WIN32)
    SET(PD_BASE "${PROJECT_SOURCE_DIR}/pd/windows")
endif()

if(DEFINED RPI)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    SET(PD_BASE "${PROJECT_SOURCE_DIR}/pd/unix")
    find_package(Threads REQUIRED)
endif()

SET(PD_INTERFACE_FOLDER "${PD_BASE}/interface")
SET(PD_SRC_FOLDER "${PD_BASE}/src")

file(GLOB_RECURSE PROJECT_SRC RELATIVE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${PD_INTERFACE_FOLDER}/*.cpp"
        "${PD_INTERFACE_FOLDER}/*.h"
        "${PD_SRC_FOLDER}/*.cpp"
        "${PD_SRC_FOLDER}/*.h"
        interface/*.cpp
        interface/*.h
        src/*.cpp
        src/*.h
        )

add_library(${PROJECT_NAME} STATIC ${PROJECT_SRC})
target_link_libraries(${PROJECT_NAME} PUBLIC CommProto)
foreach(source IN LISTS PROJECT_SRC)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()  

target_include_directories(${PROJECT_NAME} INTERFACE interface ${PD_INTERFACE_FOLDER})
include_directories(interface)

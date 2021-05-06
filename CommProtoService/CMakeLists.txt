project (CommProtoService)


if(DEFINED RPI)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()


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

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC CommProto ConcurrentQueue rapidjson)
elseif(DEFINED RPI)
    message("RIGHT PATH")
    target_link_libraries(${PROJECT_NAME} PUBLIC CommProto ConcurrentQueue rapidjson Threads::Threads)
endif()

target_include_directories(${PROJECT_NAME} INTERFACE interface)
include_directories(interface)

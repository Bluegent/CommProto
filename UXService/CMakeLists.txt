project(UXService)

find_package(Poco "1.10.1" REQUIRED COMPONENTS Net Util Crypto)
find_package(OpenSSL REQUIRED)

include (${CMAKE_SOURCE_DIR}/cmake/utility.cmake)

file(GLOB_RECURSE PROJECT_SRC RELATIVE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        *.cpp
        *.h
        )

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

target_link_libraries(${PROJECT_NAME} CommProtoService CommProtoPlugin Poco::Net Poco::Util Poco::Crypto OpenSSL::SSL)

include_directories(.)

if(WIN32)
target_link_directories(${PROJECT_NAME} PUBLIC ${SSL_Lib})
endif()

set(INSTALL_DIR "${CMAKE_BINARY_DIR}/../install")

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})
install (DIRECTORY "html_files" DESTINATION ${INSTALL_DIR})

SET(CONFIG_FILE uxservice.cfg)
installConfig(${CONFIG_FILE} ${INSTALL_DIR})

IF(WIN32)
message("win32 TYPE=${CMAKE_BUILD_TYPE}")
		  install(FILES 
			"${Poco_DIR}/../bin/PocoUtild.dll"   
			"${Poco_DIR}/../bin/PocoNetd.dll"   
			"${Poco_DIR}/../bin/PocoFoundationd.dll"   
			"${Poco_DIR}/../bin/PocoJSONd.dll"   
			"${Poco_DIR}/../bin/PocoXMLd.dll"   
			"${Poco_DIR}/../bin/PocoCryptod.dll"   
		  DESTINATION 
			"${INSTALL_DIR}")
else()

endif()


##set_executable_path(${PROJECT_NAME}) 

function(set_executable_path NAME)
    if(MSVC)
        set(TARGET_USER_PROPERTY_FILE "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.vcxproj.user")
        file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${NAME}.exe TARGET_EXE)
        file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} TARGET_FOLDER)
        configure_file(${CMAKE_SOURCE_DIR}/cmake/msvc_target_properties.user.in "${TARGET_USER_PROPERTY_FILE}" @ONLY)
    endif()
endfunction() 

function(installConfig NAME DIR)
if(NOT EXISTS "${DIR}/${NAME}")
	install(FILES config/${NAME} DESTINATION ${DIR})
endif()


endfunction()
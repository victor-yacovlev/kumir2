if (MSVC_IDE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
endif (MSVC_IDE)

if(NOT APPLE)

    set(CMAKE_INSTALL_RPATH
        "$ORIGIN/../${LIBS_DIR}/"
    )

    add_subdirectory(kumir2-classic)
    add_subdirectory(kumir2-highgrade)
    add_subdirectory(kumir2-ide)
    add_subdirectory(kumir2-teacher)
#    add_subdirectory(kumir2-python)

endif(NOT APPLE)

if(APPLE)
    add_subdirectory(kumir2-macx)
endif(APPLE)



#add_subdirectory(kumir2-cc)
add_subdirectory(kumir2-bc)
add_subdirectory(kumir2-xrun)


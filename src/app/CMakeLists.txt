if(NOT APPLE)

    set(CMAKE_INSTALL_RPATH
        "$ORIGIN/../${LIBS_DIR}/"
    )

    add_subdirectory(kumir2-classic)
    add_subdirectory(kumir2-ide)
    add_subdirectory(kumir2-teacher)

endif(NOT APPLE)

if(APPLE)
    add_subdirectory(kumir2-macx)
endif(APPLE)

add_subdirectory(kumir2-cc)
add_subdirectory(kumir2-bc)
add_subdirectory(kumir2-run)


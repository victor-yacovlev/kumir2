set(SRC
    main.cpp
)

add_executable(kumir2-as WIN32 ${SRC})
target_link_libraries(kumir2-as ${QT_LIBRARIES} DataFormats)
install(TARGETS kumir2-as DESTINATION ${EXEC_DIR})
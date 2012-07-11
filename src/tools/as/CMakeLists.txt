set(SRC
    main.cpp
)

add_executable(kumir2-as WIN32 ${SRC})
target_link_libraries(kumir2-as ${QT_LIBRARIES} DataFormats)
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-as PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-as DESTINATION ${EXEC_DIR})
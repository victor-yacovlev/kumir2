set(SRC
    main.cpp
)
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
add_executable(kumir2-as ${SRC})
target_link_libraries(kumir2-as)
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-as PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-as DESTINATION ${EXEC_DIR})

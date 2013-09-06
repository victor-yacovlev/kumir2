find_package(Boost REQUIRED)
find_package(ZLIB REQUIRED)

set(SOURCES
    parser.cpp
    generator.cpp
    options.cpp
    main.cpp
)

set(HEADERS
    data.h
    parser.h
    generator.h
    options.h
)

include_directories(${ZLIB_INCLUDE_DIRS})

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++03")
    #   due to bug in boost 1.49 (Ticket #6785)
endif()

add_executable(pictomir2course ${SOURCES})
target_link_libraries(pictomir2course ${ZLIB_LIBRARIES})
install(TARGETS pictomir2course DESTINATION ${EXEC_DIR})

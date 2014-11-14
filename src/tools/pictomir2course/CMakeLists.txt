project(pictomir2course)
cmake_minimum_required(VERSION 2.8.3)

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

if (MSVC)
    # Use provided ZLib sources
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib)
    set(3RDPARTY_LIBS )
    set(3RDPARTY_SOURCES
        3rdparty/zlib/uncompr.c
        3rdparty/zlib/inflate.c
        3rdparty/zlib/zutil.c
        3rdparty/zlib/inftrees.c
        3rdparty/zlib/inffast.c
        3rdparty/zlib/crc32.c
        3rdparty/zlib/adler32.c
    )
else()
    find_package(ZLIB REQUIRED)
    include_directories(${ZLIB_INCLUDE_DIRS})
    set(3RDPARTY_LIBS ${ZLIB_LIBRARIES})
    set(3RDPARTY_SOURCES )
endif()

set(SOURCES
    parser.cpp
    generator.cpp
    options.cpp
    main.cpp
)

set(HEADERS
    pictomir_data.h
    parser.h
    generator.h
    options.h
)

if(WIN32)
    add_definitions(-DWIN32)
endif()

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-fexceptions -std=c++03")
    #   1. -fexceptions:    Exceptions is widely used by Boost
    #   2. -std=c++03:      Due to bug in boost 1.49 (Ticket #6785)
endif()

add_executable(pictomir2course ${SOURCES} ${3RDPARTY_SOURCES})
target_link_libraries(pictomir2course ${3RDPARTY_LIBS} ${STDCXX_LIB})
install(TARGETS pictomir2course DESTINATION ${EXEC_DIR})

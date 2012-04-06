include(../../kumir2_library.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DSTDLIB_LIBRARY)

set(HEADERS
    kumir_core.h
    kumir_math.h
    kumir_string.h
)


set(SOURCES
    kumir_core.c
    kumir_math.c
    kumir_string.c
)

source_group(Headers FILES ${HEADERS})
source_group(Sources FILES ${SOURCES})


add_library(KumirStdLib SHARED ${SOURCES})
set_property(TARGET KumirStdLib APPEND PROPERTY COMPILE_DEFINITIONS STDLIB_LIBRARY)

handleTranslation(KumirStdLib)

install(TARGETS KumirStdLib DESTINATION ${LIBS_DIR})

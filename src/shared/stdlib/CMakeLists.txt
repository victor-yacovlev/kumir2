include(../../kumir2_library.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MOC_HEADERS
    genericinputoutput.h
)

set(HEADERS
    doubleoverflowchecher.h
    integeroverflowchecher.h
    genericinputoutput.h
    kumstdlib.h
)


set(SOURCES
    kumstdlib.cpp genericinputoutput.cpp
    integeroverflowchecker.cpp
    doubleoverflowchecker.cpp
)

source_group(Headers FILES ${HEADERS})
source_group(Sources FILES ${SOURCES})

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

add_library(KumirStdLib SHARED ${SOURCES} ${MOC_SOURCES})
target_link_libraries(KumirStdLib ${QT_LIBRARIES})
set_property(TARGET KumirStdLib APPEND PROPERTY COMPILE_DEFINITIONS STDLIB_LIBRARY)

handleTranslation(KumirStdLib)

install(TARGETS KumirStdLib DESTINATION ${LIBS_DIR})

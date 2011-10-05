include(../../kumir2_library.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
    set(CONNECTOR_MODULE shm)
    set(CONNECTOR_METHOD SHM)
else()
    set(CONNECTOR_MODULE pipe)
    set(CONNECTOR_METHOD PIPE)
endif()

add_definitions(-D${CONNECTOR_METHOD}_METHOD)

set(MOC_HEADERS
    connector.h actorwindow.h
    connector_backend.h
    connector_${CONNECTOR_MODULE}.h
)

set(HEADERS
    connector.h actorwindow.h
    connector_backend.h
    doubleoverflowchecher.h
    integeroverflowchecher.h
    genericinputoutput.h
    guirunner.h
    kumstdlib.h
    userprogramthread.h
)


set(SOURCES
    kumstdlib.cpp genericinputoutput.cpp connector.cpp guirunner.cpp actorwindow.cpp userprogramthread.cpp
    integeroverflowchecker.cpp
    doubleoverflowchecker.cpp
    connector_${CONNECTOR_MODULE}.cpp
)

source_group(Headers FILES ${HEADERS})
source_group(Sources FILES ${SOURCES})

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

add_library(KumirStdLib SHARED ${SOURCES} ${MOC_SOURCES})
target_link_libraries(KumirStdLib ${QT_LIBRARIES})
set_property(TARGET KumirStdLib APPEND PROPERTY COMPILE_DEFINITIONS STDLIB_LIBRARY)
if(WIN32)
install(TARGETS KumirStdLib DESTINATION bin)
else()
install(TARGETS KumirStdLib LIBRARY DESTINATION ${LIB_BASENAME}/kumir2)
endif()

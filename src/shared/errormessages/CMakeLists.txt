set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_library.cmake)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    errormessages.cpp
)

add_library(ErrorMessages SHARED  ${SOURCES})
target_link_libraries(ErrorMessages ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET ErrorMessages APPEND PROPERTY COMPILE_DEFINITIONS ERRORMESSAGES_LIBRARY)

handleTranslation(ErrorMessages)

install(TARGETS ErrorMessages DESTINATION ${LIBS_DIR})

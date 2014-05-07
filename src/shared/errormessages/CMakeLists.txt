project(ErrorMessages)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES})
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
    include (${QT_USE_FILE})
    find_library(QCA_LIBRARY NAMES qca HINTS ${QCA_LIBDIR})
    find_path(QCA_INCLUDE_DIR QtCrypto/qca.h ${QCA_INCDIR})
endif()

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

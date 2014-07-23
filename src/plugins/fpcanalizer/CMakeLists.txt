project(FPCAnalizer)
cmake_minimum_required(VERSION 2.8.3)
if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES})
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
    include(${QT_USE_FILE})
endif()
include(../../kumir2_plugin.cmake)

set(SOURCES
    fpcanalizerplugin.cpp
    fpcanalizer.cpp
    simplepascalsyntaxanalizer.cpp
)

set(MOC_HEADERS
    fpcanalizerplugin.h
    fpcanalizer.h
    simplepascalsyntaxanalizer.h
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()
copySpecFile(FPCAnalizer)
add_library(FPCAnalizer SHARED ${MOC_SOURCES} ${SOURCES})
#handleTranslation(FPCAnalizer)
target_link_libraries(FPCAnalizer ${QT_LIBRARIES} ExtensionSystem DataFormats  ${STDCXX_LIB} ${STDMATH_LIB})
install(TARGETS FPCAnalizer DESTINATION ${PLUGINS_DIR})
copyResources(fpcanalizer)
add_executable(as fake_as.c)
set_target_properties(as PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${LIBEXECS_PATH}/fpcanalizer")
install(TARGETS as DESTINATION ${LIBEXECS_DIR}/fpcanalizer)

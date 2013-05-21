set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtScript QtNetwork QtXml QtSvg QtWebKit REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_plugin.cmake)


set(SOURCES
    coursemanager_plugin.cpp
    task/mainwindow.cpp

)

set(MOC_HEADERS
    coursemanager_plugin.h
    task/mainwindow.h

)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(CourseManager)
add_library(CourseManager SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(CourseManager)
target_link_libraries(CourseManager ${QT_LIBRARIES} ExtensionSystem DataFormats)

install(TARGETS CourseManager DESTINATION ${PLUGINS_DIR})

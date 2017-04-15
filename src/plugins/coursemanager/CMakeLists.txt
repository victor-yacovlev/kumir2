project(CourseManager)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui Xml)

set(SOURCES
    coursemanager_plugin.cpp
    task/coursemanager_window.cpp
    course_model.cpp
)

set(FORMS
    task/editdialog.ui
    task/coursemanager_window.ui
    task/newkursdialog.ui
)

set(MOC_HEADERS
    coursemanager_plugin.h
    task/coursemanager_window.h
    course_model.h
)


kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
kumir2_wrap_ui(UI_SOURCES ${FORMS})

kumir2_add_plugin(
    NAME        CourseManager
    SOURCES     ${MOC_SOURCES} ${SOURCES} ${UI_SOURCES}
    LIBRARIES   ${QT_LIBRARIES} ExtensionSystem DataFormats Widgets
)

project(DocBookViewer)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui Svg Xml)

set(MOC_HEADERS
    docbookview.h
    docbookview_impl.h
    contentview.h
    printdialog.h
    sidepanel.h
)

set(SOURCES
    docbookview.cpp
    docbookview_impl.cpp
    docbookmodel.cpp
    docbookfactory.cpp
    document.cpp
    contentview.cpp
    printdialog.cpp
    printrenderer.cpp
    sidepanel.cpp
    mathmlrenderer.cpp
)

set(FORMS
    printdialog.ui
    sidepanel.ui
)

kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
kumir2_wrap_ui(UI_SOURCES ${FORMS})

kumir2_add_library(
    NAME        DocBookViewer
    SOURCES     ${SOURCES} ${MOC_SOURCES} ${UI_SOURCES}
    LIBRARIES   ${QT_LIBRARIES} ExtensionSystem Widgets
)

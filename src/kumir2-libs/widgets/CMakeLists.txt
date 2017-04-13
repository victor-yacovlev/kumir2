project(Widgets)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui)

set(SOURCES 
    actionproxy.cpp
    groupbox.cpp
    cyrillicmenu.cpp
    secondarywindow.cpp
    pultlogger.cpp
    draw_turtle_field.cpp
    dockwindowplace.cpp
    dockwindowplace_impl.cpp
    dockwindowplace_container.cpp
    multipagedialog.cpp
    multipagedialog_impl.cpp
    declarativesettingspage.cpp
    declarativesettingspage_impl.cpp
    secondarywindow_generic.cpp
    utf8textbrowser.cpp
    iconprovider.cpp
    flowlayout.cpp
)

set(MOC_HEADERS
    actionproxy.h
    groupbox.h
    pultlogger.h
    cyrillicmenu.h
    secondarywindow.h
    draw_turtle_field.h
    dockwindowplace.h
    dockwindowplace_container.h
    dockwindowplace_impl.h
    multipagedialog.h
    multipagedialog_impl.h
    declarativesettingspage.h
    declarativesettingspage_impl.h
    secondarywindow_generic.h
)

kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

kumir2_add_library(
    NAME        Widgets
    SOURCES     ${SOURCES} ${MOC_SOURCES}
    LIBRARIES   ${QT_LIBRARIES} ExtensionSystem
)

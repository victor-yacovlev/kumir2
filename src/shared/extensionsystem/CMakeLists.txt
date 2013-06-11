set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_library.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MOC_HEADERS
    kplugin.h
    pluginmanager.h
    settingsdialog.h
    switchworkspacedialog.h
    declarativesettingspage.h
)

set(SOURCES
    kplugin.cpp
    pluginspec.cpp
    pluginmanager.cpp
    settingsdialog.cpp
    switchworkspacedialog.cpp
    declarativesettingspage.cpp
    settings.cpp
)

set(HEADERS
    kplugin.h
    pluginmanager.h
    settingsdialog.h
    switchworkspacedialog.h
    pluginspec.h
    settings.h
)

set(FORMS
    settingsdialog.ui
    switchworkspacedialog.ui
)

qt4_wrap_ui(UI_HEADERS ${FORMS})
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

add_library(ExtensionSystem SHARED  ${UI_HEADERS} ${SOURCES} ${MOC_SOURCES})
target_link_libraries(ExtensionSystem ${QT_LIBRARIES})

set_property(TARGET ExtensionSystem APPEND PROPERTY COMPILE_DEFINITIONS EXTENSIONSYSTEM_LIBRARY)

handleTranslation(ExtensionSystem)

install(TARGETS ExtensionSystem DESTINATION ${LIBS_DIR})

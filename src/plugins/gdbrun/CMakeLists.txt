set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
include (${QT_USE_FILE})
include(../../kumir2_plugin.cmake)


set(SOURCES
    gdbrunplugin.cpp
    pascalvariablesmodel.cpp
)

set(HEADERS
)

set(MOC_HEADERS
    gdbrunplugin.h
    pascalvariablesmodel.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(GDBRun)
add_library(GDBRun SHARED ${MOC_SOURCES} ${SOURCES})
#handleTranslation(KumirCodeRun)
target_link_libraries(GDBRun ${QT_LIBRARIES} ExtensionSystem  ${STDCXX_LIB} ${STDMATH_LIB})

install(TARGETS GDBRun DESTINATION ${PLUGINS_DIR})

set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
include (${QT_USE_FILE})
include(../../kumir2_plugin.cmake)


set(SOURCES
    runplugin.cpp
    run.cpp
    consolerun.cpp
    commonrun.cpp
    util.cpp
    guirun.cpp
    kumvariablesmodel.cpp
)

set(HEADERS
    ../../shared/vm/vm.hpp
)

set(MOC_HEADERS
    runplugin.h
    run.h
    commonrun.h
    guirun.h
    kumvariablesmodel.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirCodeRun)
add_library(KumirCodeRun SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirCodeRun)
target_link_libraries(KumirCodeRun ${QT_LIBRARIES} ExtensionSystem)

install(TARGETS KumirCodeRun DESTINATION ${PLUGINS_DIR})

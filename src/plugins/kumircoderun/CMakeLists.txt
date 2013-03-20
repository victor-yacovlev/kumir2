include(../../kumir2_plugin.cmake)


set(SOURCES
    plugin.cpp
    run.cpp
    consolerun.cpp
    commonrun.cpp
    util.cpp
    guirun.cpp
)

set(HEADERS
    ../../shared/vm/vm.hpp
)

set(MOC_HEADERS
    plugin.h
    run.h
    commonrun.h
    guirun.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirCodeRun)
add_library(KumirCodeRun SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirCodeRun)
target_link_libraries(KumirCodeRun ${QT_LIBRARIES} ExtensionSystem)

install(TARGETS KumirCodeRun DESTINATION ${PLUGINS_DIR})

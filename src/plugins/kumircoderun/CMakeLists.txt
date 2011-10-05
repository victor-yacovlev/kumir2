include(../../kumir2_plugin.cmake)

set(SOURCES
    plugin.cpp
    run.cpp
    vm.cpp
    variant.cpp
    context.cpp
)

set(MOC_HEADERS
    plugin.h
    run.h
    vm.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirCodeRun)
handleTranslation(KumirCodeRun)
add_library(KumirCodeRun SHARED ${MOC_SOURCES} ${SOURCES})
target_link_libraries(KumirCodeRun ${QT_LIBRARIES} ExtensionSystem DataFormats KumirStdLib)
install(TARGETS KumirCodeRun LIBRARY DESTINATION ${LIB_BASENAME}/kumir2/plugins)

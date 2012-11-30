include(../../kumir2_plugin.cmake)

set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

set(SOURCES
    plugin.cpp
    run.cpp
)

set(HEADERS
    ../../shared/vm/vm.hpp
)

set(MOC_HEADERS
    plugin.h
    run.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirCodeRun)
add_library(KumirCodeRun SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirCodeRun)
target_link_libraries(KumirCodeRun ${QT_LIBRARIES} ExtensionSystem)

install(TARGETS KumirCodeRun DESTINATION ${PLUGINS_DIR})

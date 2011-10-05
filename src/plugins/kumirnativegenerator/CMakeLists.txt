include(../../kumir2_plugin.cmake)

if(WIN32)
    set(BACKEND llvm)
else()
    set(BACKEND gcc)
endif()

set(SOURCES
    kumirnativegeneratorplugin.cpp
    nameprovider.cpp
    ${BACKEND}backend.cpp
)

set(MOC_HEADERS
    kumirnativegeneratorplugin.h
    nameprovider.h
    ${BACKEND}backend.h
)

add_definitions(-DUSE_${BACKEND})

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirNativeGenerator SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirNativeGenerator)
add_library(KumirNativeGenerator SHARED ${MOC_SOURCES} ${SOURCES})
target_link_libraries(KumirNativeGenerator ${QT_LIBRARIES} ExtensionSystem DataFormats)
copyResources(kumirnativegenerator)

install(TARGETS KumirNativeGenerator DESTINATION ${PLUGINS_DIR})

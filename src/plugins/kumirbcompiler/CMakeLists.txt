include(../../kumir2_plugin.cmake)

set(SOURCES
    kumirbcompilerplugin.cpp
)

set(MOC_HEADERS
    kumirbcompilerplugin.h
)


qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirBCompiler)
handleTranslation(KumirBCompiler)
add_library(KumirBCompiler SHARED ${MOC_SOURCES} ${SOURCES})
target_link_libraries(KumirBCompiler ${QT_LIBRARIES} ExtensionSystem DataFormats)

if(WIN32)
	install(TARGETS KumirBCompiler DESTINATION ${LIB_BASENAME}/kumir2/plugins)
else()
	install(TARGETS KumirBCompiler LIBRARY DESTINATION ${LIB_BASENAME}/kumir2/plugins)
endif()

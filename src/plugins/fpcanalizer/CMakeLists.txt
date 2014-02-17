set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_plugin.cmake)

set(SOURCES
    fpcanalizerplugin.cpp
    fpcanalizer.cpp
    simplepascalsyntaxanalizer.cpp
)

set(MOC_HEADERS
    fpcanalizerplugin.h
    fpcanalizer.h
    simplepascalsyntaxanalizer.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(FPCAnalizer)
add_library(FPCAnalizer SHARED ${MOC_SOURCES} ${SOURCES})
#handleTranslation(FPCAnalizer)
target_link_libraries(FPCAnalizer ${QT_LIBRARIES} ExtensionSystem DataFormats  ${STDCXX_LIB} ${STDMATH_LIB})
install(TARGETS FPCAnalizer DESTINATION ${PLUGINS_DIR})

add_executable(as fake_as.c)
set_target_properties(as PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${LIBEXECS_PATH}/fpcanalizer")
install(TARGETS as DESTINATION ${LIBEXECS_DIR}/fpcanalizer)

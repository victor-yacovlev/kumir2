include(../../kumir2_plugin.cmake)
find_program(PY3CONFIG python3-config)

execute_process(COMMAND ${PY3CONFIG} --libs OUTPUT_VARIABLE PY3_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${PY3CONFIG} --includes OUTPUT_VARIABLE PY3_INCLUDES OUTPUT_STRIP_TRAILING_WHITESPACE)
include_directories(${PY3_INCLUDES})

message(STATUS "Found Python3 libraries: "${PY3_LIBRARIES})
add_definitions(-DQT_NO_KEYWORDS)

set(SOURCES
    python3languageplugin.cpp
)

set(MOC_HEADERS
    python3languageplugin.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(Python3Language)
add_library(Python3Language SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(Python3Language)
target_link_libraries(Python3Language ${QT_LIBRARIES} ${PY3_LIBRARIES} ExtensionSystem)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/share/kumir2/python3language)
install(TARGETS Python3Language DESTINATION ${PLUGINS_DIR})

include(../../kumir2_library.cmake)


include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    groupbox.cpp
)

set(MOC_HEADERS
    groupbox.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

add_library(Widgets SHARED  ${SOURCES} ${MOC_SOURCES})
target_link_libraries(Widgets ${QT_LIBRARIES})
set_property(TARGET Widgets APPEND PROPERTY COMPILE_DEFINITIONS WIDGETS_LIBRARY)

handleTranslation(Widgets)

install(TARGETS Widgets DESTINATION ${LIBS_DIR})

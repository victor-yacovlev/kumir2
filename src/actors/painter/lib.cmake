include(../../kumir2_library.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MOC_HEADERS
    painterwindow.h
    painterview.h
    painterruler.h
    painternewimagedialog.h
    painterworker.h
)

set(SOURCES
    painterwindow.cpp
    paintertools.cpp
    painterruler.cpp
    painterview.cpp
    painternewimagedialog.cpp
    painterworker.cpp
    painter_c.cpp
)

set(FORMS
    painterwindow.ui
    painternewimagedialog.ui
)

qt4_wrap_ui(UI_SOURCES ${FORMS})
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
handleTranslation(ActorPainterC)
add_library(ActorPainterC SHARED ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES})
target_link_libraries(ActorPainterC ${QT_LIBRARIES} KumirStdLib)
install(TARGETS ActorPainterC LIBRARY DESTINATION ${LIB_BASENAME}/kumir2)

find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg REQUIRED)
include (${QT_USE_FILE})

set(SOURCES
    main.cpp
    options.cpp
    generator.cpp
)

add_executable(docbook2latex ${SOURCES})
target_link_libraries(docbook2latex ${QT_LIBRARIES} DocBookViewer  ${STDCXX_LIB})

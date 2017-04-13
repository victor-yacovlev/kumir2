project(docbook2latex)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui Xml Svg)

set(SOURCES
    main.cpp
    options.cpp
    generator.cpp
)

kumir2_add_tool(
    NAME        docbook2latex
    SOURCES     ${SOURCES}
    LIBRARIES   ${QT_LIBRARIES} DocBookViewer
)

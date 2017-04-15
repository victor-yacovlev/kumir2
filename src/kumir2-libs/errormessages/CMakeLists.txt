project(ErrorMessages)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core)

set(SOURCES
    errormessages.cpp
)

kumir2_add_library(
    NAME        ErrorMessages
    SOURCES     ${SOURCES}
    LIBRARIES   ${QT_LIBRARIES} ExtensionSystem
)

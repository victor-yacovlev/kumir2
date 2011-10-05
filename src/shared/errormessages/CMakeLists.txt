include(../../kumir2_library.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    errormessages.cpp
)

add_library(ErrorMessages SHARED  ${SOURCES})
target_link_libraries(ErrorMessages ${QT_LIBRARIES})
set_property(TARGET ErrorMessages APPEND PROPERTY COMPILE_DEFINITIONS ERRORMESSAGES_LIBRARY)

if(WIN32)
install(TARGETS ErrorMessages DESTINATION bin)
else()
install(TARGETS ErrorMessages LIBRARY DESTINATION ${LIB_BASENAME}/kumir2)
endif()


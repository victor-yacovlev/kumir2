include(../../kumir2_plugin.cmake)
include(../../Fpc-Pascal.cmake)


set(SOURCES
    pasplugin.cpp
    ippc.cpp
)

set(MOC_HEADERS
    pasplugin.h
    ippc.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(PascalAnalizer)
handleTranslation(PascalAnalizer)
add_library(PascalAnalizer SHARED ${MOC_SOURCES} ${SOURCES})
target_link_libraries(PascalAnalizer ${QT_LIBRARIES} ExtensionSystem)

file(GLOB IPPC_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/*.pas
    ${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/x86/*.pas
    ${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/systems/*.pas
    ${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/${FPC_TARGET}/*.pas
    )


set(FPC_FLAGS
    ${FPC_COMMON_FLAGS}
    -Fu${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/
    -Fu${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/systems/
    -Fu${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/x86
    -Fu${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/${FPC_TARGET}
    -Fi${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/
    -Fi${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/systems/
    -Fi${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/x86
    -Fi${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/${FPC_TARGET}
    -FE${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ippc_binary
    -FU${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ippc_binary
    -Fu${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ippc_binary
)

set(LIBEXEC_DIR ${CMAKE_BINARY_DIR}/libexec)
file(MAKE_DIRECTORY ${LIBEXEC_DIR})
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ippc_binary)


add_custom_command(
    OUTPUT ${LIBEXEC_DIR}/ippc
    DEPENDS ${IPPC_SRC}
    COMMAND ${FPC_COMPILER} ${FPC_FLAGS} -o${LIBEXEC_DIR}/ippc ${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/pp.pas
)

add_custom_target(ippc_binary ALL DEPENDS ${LIBEXEC_DIR}/ippc)

install(TARGETS PascalAnalizer LIBRARY DESTINATION ${LIB_BASENAME}/kumir2/plugins)
install(
    FILES ${LIBEXEC_DIR}/ippc
    DESTINATION libexec
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                GROUP_EXECUTE GROUP_READ
                WORLD_EXECUTE WORLD_READ
)

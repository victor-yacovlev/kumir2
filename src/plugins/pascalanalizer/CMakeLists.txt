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
add_library(PascalAnalizer SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(PascalAnalizer)
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
    -FE${CMAKE_CURRENT_BINARY_DIR}/
    -FU${CMAKE_CURRENT_BINARY_DIR}/
    -Fu${CMAKE_CURRENT_BINARY_DIR}/
)

file(MAKE_DIRECTORY ${LIBEXECS_PATH})

if(WIN32)
	set(IPPC_BIN ippc.exe)
else()
	set(IPPC_BIN ippc)
endif()

add_custom_target(ippc_bin
    DEPENDS ${IPPC_SRC}
    COMMAND ${FPC_COMPILER} ${FPC_FLAGS} -o${LIBEXECS_PATH}/${IPPC_BIN} ${CMAKE_CURRENT_SOURCE_DIR}/ippc/compiler/pp.pas
)

add_dependencies(PascalAnalizer ippc_bin)

install(TARGETS PascalAnalizer DESTINATION ${PLUGINS_DIR})

install(
    FILES ${LIBEXECS_PATH}/${IPPC_BIN}
    DESTINATION ${LIBEXECS_DIR}
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                GROUP_EXECUTE GROUP_READ
                WORLD_EXECUTE WORLD_READ
)

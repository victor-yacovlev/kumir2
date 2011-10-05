find_program(FPC_COMPILER fpc
    NAMES fpc ppc386 ppcx64
    PATHS /usr/bin /usr/local/bin
)

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    set(FPC_TARGET x86_64)
else()
    set(FPC_TARGET i386)
endif()

string(TOLOWER ${CMAKE_SYSTEM_NAME} FPC_SYSTEM_NAME)

set(FPC_COMMON_FLAGS
    -T${FPC_SYSTEM_NAME}
    -d${FPC_TARGET}
)

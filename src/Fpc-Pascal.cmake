if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    set(FPC_TARGET x86_64)
else()
    set(FPC_TARGET i386)
endif()

string(TOLOWER ${CMAKE_SYSTEM_NAME} FPC_SYSTEM_NAME)

if(WIN32)
	set(FPC_SYSTEM_NAME win32)
endif(WIN32)


if(EXISTS ${CMAKE_SOURCE_DIR}/fpc/lib/fpc/2.4.4)
  find_program(FPC_COMPILER fpc
      NAMES ppc386 ppcx64
      PATHS ${CMAKE_SOURCE_DIR}/fpc/lib/fpc/2.4.4
  )
  set(FPC_COMMON_FLAGS
    -T${FPC_SYSTEM_NAME}
    -d${FPC_TARGET}
    -Fu${CMAKE_SOURCE_DIR}/fpc/lib/fpc/2.4.4/units/${FPC_TARGET}-${FPC_SYSTEM_NAME}/rtl
  )
  
else()

  find_program(FPC_COMPILER fpc
      NAMES fpc ppc386 ppcx64
      PATHS /usr/bin /usr/local/bin
  )
  set(FPC_COMMON_FLAGS
      -T${FPC_SYSTEM_NAME}
      -d${FPC_TARGET}
  )
  
endif()

if(EXISTS ${FPC_COMPILER})
  message("-- FreePascal compiler: ${FPC_COMPILER}")
else()
  message(FATAL_ERROR "FreePascal compiler (version >= 2.4.0) not found")
endif()

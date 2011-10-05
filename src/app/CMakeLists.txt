set(CMAKE_INSTALL_RPATH
    "$ORIGIN/../${LIB_BASENAME}/kumir2/"
)

include(kumir2-classic.cmake)
include(kumir2-ide.cmake)
include(kumir2-teacher.cmake)
include(kumir2-cc.cmake)
include(kumir2-bc.cmake)
include(kumir2-run.cmake)

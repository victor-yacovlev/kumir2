# NOTE: You should use MSVC 2010+ compiler

# Build filesystem layout, similar to Linux layout
set(KUMIR2_EXEC_DIR "bin")                                  # executable binaries
set(KUMIR2_LIBS_DIR "bin")                                  # common DLL's
set(KUMIR2_PLUGINS_DIR "lib/kumir2/plugins")                # libraries to be load at run time
set(KUMIR2_RESOURCES_DIR "share/kumir2")                    # non-executable resources
set(KUMIR2_LIBEXECS_DIR "bin")                              # executable supplementary binaries

if(NOT DEFINED KUMIR2_SDK_SCRIPTS_DIR)
    set(KUMIR2_SDK_SCRIPTS_DIR "include")          # devel scripts
endif(NOT DEFINED KUMIR2_SDK_SCRIPTS_DIR)

if(NOT DEFINED KUMIR2_SDK_CMAKE_DIR)
    set(KUMIR2_SDK_CMAKE_DIR "include")            # cmake files to use from others
endif(NOT DEFINED KUMIR2_SDK_CMAKE_DIR)

# Compile flags
set(KUMIR2_CXXFLAGS " -Zm300 -EHsc -GR -Y- -DWIN32 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS")
set(KUMIR2_CXXFLAGS_Release "-DNDEBUG -DQT_NO_DEBUG -MD")
set(KUMIR2_CXXFLAGS_Debug "-MDd -Zi")

if(${USE_QT} GREATER 4)
    # Qt5 uses MSVC's built-in wchar_t type
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Zc:wchar_t")
else()
    # ... but Qt4 - not, so it should be explicitly disabled
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Zc:wchar_t-")
endif(${USE_QT} GREATER 4)


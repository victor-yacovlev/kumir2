# TODO: Find a Mac and check this

# Build filesystem layout, according to LSB
set(KUMIR2_EXEC_DIR "Kumir.app/Contents/MacOS")             # executable binaries
set(KUMIR2_LIBS_DIR "Kumir.app/Contents/PlugIns")           # shared libraries
set(KUMIR2_PLUGINS_DIR "Kumir.app/Contents/PlugIns")        # libraries to be load at run time
set(KUMIR2_RESOURCES_DIR "Kumir.app/Contents/Resources")    # non-executable resources
set(KUMIR2_LIBEXECS_DIR "Kumir.app/Contents/MacOS")         # executable supplementary binaries

# clear default CMake RPATH values
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
set(CMAKE_INSTALL_RPATH "")

# Compile flags
set(KUMIR2_CXXFLAGS "-fno-exceptions -std=c++0x -fPIC -DAPPLE")
set(KUMIR2_CXXFLAGS_Release "-O2 -DNDEBUG -DQT_NO_DEBUG")
set(KUMIR2_CXXFLAGS_Debug "-g -O0 -Werror -Wreorder -Wreturn-type -Wno-error=unused-variable -Wno-error=unused-parameter")

# Linkage flags
set(KUMIR2_LIBRARY_LINKER_FLAGS "-Wl,--enable-new-dtags -Wl,-rpath,'/'")
set(KUMIR2_PLUGIN_LINKER_FLAGS "-Wl,--enable-new-dtags -Wl,-rpath,'../PlugIns'")
set(KUMIR2_LAUNCHER_LINKER_FLAGS "-Wl,-rpath,'../PlugIns'")
set(KUMIR2_TOOL_LINKER_FLAGS "-Wl,-rpath,'../PlugIns'")

# To make LSB-compliant structure all project shared libraries
# should be placed into 'lib' subdirectory of /usr [/usr/local].
# Some distributions have distinct lib64 directory for 64-bit libraries.
# LIB_BASENAME value is a correct 'lib' dir name for current distro.
if((${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64") AND NOT (EXISTS /etc/ubuntu-release) AND NOT (EXISTS /etc/debian_version))
    set(LIB_BASENAME lib64)
else()
    set(LIB_BASENAME lib)
endif()

# Build filesystem layout, according to LSB
set(KUMIR2_EXEC_DIR "bin")                                  # executable binaries
set(KUMIR2_LIBS_DIR "${LIB_BASENAME}/kumir2")               # shared libraries
set(KUMIR2_PLUGINS_DIR "${LIB_BASENAME}/kumir2/plugins")    # libraries to be load at run time
set(KUMIR2_RESOURCES_DIR "share/kumir2")                    # non-executable resources
set(KUMIR2_LIBEXECS_DIR "libexec/kumir2")                   # executable supplementary binaries

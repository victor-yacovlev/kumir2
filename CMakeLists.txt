project(Kumir2)

cmake_minimum_required(VERSION 3.0)

if(EXISTS custom_variables.cmake)
    include(custom_variables.cmake)
endif()

include(kumir2_common.cmake)
if(WIN32)
    include(kumir2_win32.cmake)
elseif(APPLE)
    include(kumir2_apple.cmake)
else()
    include(kumir2_linux.cmake)
endif()


if(WIN32)
  set(EXEC_DIR bin)
  set(LIBS_DIR bin)
  set(LIB_BASENAME lib)
  set(PLUGINS_DIR lib/kumir2/plugins)
  set(RESOURCES_DIR share/kumir2)
  set(LIBEXECS_DIR bin)
elseif(APPLE)
    if(XCODE)
        set(EXEC_DIR Debug/Kumir.app/Contents/MacOS/)
        set(LIBS_DIR Debug/Kumir.app/Contents/PlugIns)
        set(PLUGINS_DIR Debug/Kumir.app/Contents/PlugIns)
        set(RESOURCES_DIR Debug/Kumir.app/Contents/Resources)
        set(LIBEXECS_DIR Debug/Kumir.app/Contents/MacOS)
    else()
        set(EXEC_DIR Kumir.app/Contents/MacOS)
        set(LIBS_DIR Kumir.app/Contents/PlugIns)
        set(PLUGINS_DIR Kumir.app/Contents/PlugIns)
        set(RESOURCES_DIR Kumir.app/Contents/Resources)
        set(LIBEXECS_DIR Kumir.app/Contents/MacOS)
    endif(XCODE)
else()

endif()

add_definitions(-DIDE_LIBRARY_BASENAME="${LIB_BASENAME}")

set(PLUGIN_OUTPUT_PATH
    ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${PLUGINS_DIR}
)

set(SHARE_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${RESOURCES_DIR})

set(LIBRARY_OUTPUT_PATH
    ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${LIBS_DIR}
)

set(EXECUTABLE_OUTPUT_PATH
    ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXEC_DIR}
)

set(LIBEXECS_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${LIBEXECS_DIR})

add_subdirectory(src)
add_subdirectory(userdocs)
add_subdirectory(share/kumir2/courses)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/applications)
file(GLOB_RECURSE icons RELATIVE ${CMAKE_SOURCE_DIR}/app_icons/linux ${CMAKE_SOURCE_DIR}/app_icons/linux/*)
foreach(icon IN ITEMS ${icons})
    get_filename_component(subdir ${icon} PATH)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/icons/${subdir})
    file(COPY ${CMAKE_SOURCE_DIR}/app_icons/linux/${icon} DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/icons/${subdir})
    install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/icons/${icon} DESTINATION share/icons/${subdir})
endforeach(icon)

# TODO move into python subproject CMakeLists.txt
if(EXISTS ${CMAKE_SOURCE_DIR}/kumir2-python.desktop)
    file(COPY ${CMAKE_SOURCE_DIR}/kumir2-python.desktop DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/applications)
    install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/applications/kumir2-python.desktop DESTINATION share/applications)
endif()


file(MAKE_DIRECTORY ${SHARE_PATH}/icons)
file(GLOB_RECURSE icons RELATIVE ${CMAKE_SOURCE_DIR}/share/kumir2/icons ${CMAKE_SOURCE_DIR}/share/kumir2/icons/*)
foreach(icon IN ITEMS ${icons})
    get_filename_component(subdir ${icon} PATH)
    file(MAKE_DIRECTORY ${SHARE_PATH}/icons/${subdir})
    file(COPY ${CMAKE_SOURCE_DIR}/share/kumir2/icons/${icon} DESTINATION ${SHARE_PATH}/icons/${subdir})
    install(FILES ${SHARE_PATH}/icons/${icon} DESTINATION ${RESOURCES_DIR}/icons/${subdir})
endforeach(icon)

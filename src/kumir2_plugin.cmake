if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core REQUIRED)
    set(QT_LRELEASE_EXECUTABLE "${_qt5Core_install_prefix}/bin/lrelease")
else()
    # Find Qt4
    find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
    include (${QT_USE_FILE})
endif()

# -- linux
if(NOT APPLE AND NOT MSVC)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags -Wl,-rpath,'\$ORIGIN/..'")
endif(NOT APPLE AND NOT MSVC)
# -- mac
if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags -Wl,-rpath,'../PlugIns'")
endif(APPLE)

add_definitions(-DQT_PLUGIN)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/shared)
include_directories(${CMAKE_SOURCE_DIR}/src/plugins)

set(LIBRARY_OUTPUT_PATH ${PLUGIN_OUTPUT_PATH})
if (MSVC_IDE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_PATH})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_PATH})
endif (MSVC_IDE)

macro(copySpecFile pluginName)
    file(COPY ${pluginName}.pluginspec DESTINATION ${PLUGIN_OUTPUT_PATH})
    install(FILES ${PLUGIN_OUTPUT_PATH}/${pluginName}.pluginspec DESTINATION ${PLUGINS_DIR})
endmacro(copySpecFile)

macro(handleTranslation pluginName)
    file(GLOB ts_files ${CMAKE_SOURCE_DIR}/share/kumir2/translations/${pluginName}_??.ts)
    foreach(ts_file IN ITEMS ${ts_files})
        file(MAKE_DIRECTORY ${SHARE_PATH}/translations/)
        get_filename_component(basename ${ts_file} NAME_WE)
        set(qm_target ${basename}_qm)
        #add_custom_target(${qm_target} ALL ${QT_LRELEASE_EXECUTABLE} -qm ${SHARE_PATH}/translations/${basename}.qm ${ts_file})       
        #message("TS: "${ts_file})
        add_custom_target(${qm_target} ALL
            ${QT_LRELEASE_EXECUTABLE} -qm ${SHARE_PATH}/translations/${basename}.qm ${ts_file}
            DEPENDS ${ts_file}
        )
        install(FILES ${SHARE_PATH}/translations/${basename}.qm DESTINATION ${RESOURCES_DIR}/translations)
    endforeach(ts_file)
endmacro(handleTranslation)

macro(copyResources pluginDir)
    set(SRC_RES ${CMAKE_SOURCE_DIR}/share/kumir2/${pluginDir}/)
    file(GLOB_RECURSE resfiles RELATIVE ${SRC_RES} ${SRC_RES}/*)
    foreach(res IN ITEMS ${resfiles})
        if(NOT ${res} MATCHES .svn*)
            get_filename_component(subdir ${res} PATH)
            file(MAKE_DIRECTORY ${SHARE_PATH}/${pluginDir}/${subdir})
            file(COPY ${CMAKE_SOURCE_DIR}/share/kumir2/${pluginDir}/${res} DESTINATION ${SHARE_PATH}/${pluginDir}/${subdir})
            install(FILES ${SHARE_PATH}/${pluginDir}/${res} DESTINATION ${RESOURCES_DIR}/${pluginDir}/${subdir})
        endif()
    endforeach(res)
endmacro(copyResources)

macro(copyResourcesExcl pluginDir exclPattern)
    set(SRC_RES ${CMAKE_SOURCE_DIR}/share/kumir2/${pluginDir}/)
    file(GLOB_RECURSE resfiles RELATIVE ${SRC_RES} ${SRC_RES}/*)
    foreach(res IN ITEMS ${resfiles})
        if((NOT ${res} MATCHES .svn*) AND (NOT ${res} MATCHES ${exclPattern}))
            get_filename_component(subdir ${res} PATH)
            file(MAKE_DIRECTORY ${SHARE_PATH}/${pluginDir}/${subdir})
            file(COPY ${CMAKE_SOURCE_DIR}/share/kumir2/${pluginDir}/${res} DESTINATION ${SHARE_PATH}/${pluginDir}/${subdir})
            install(FILES ${SHARE_PATH}/${pluginDir}/${res} DESTINATION ${RESOURCES_DIR}/${pluginDir}/${subdir})
        endif()
    endforeach(res)
endmacro(copyResourcesExcl)



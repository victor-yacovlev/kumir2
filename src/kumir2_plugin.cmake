add_definitions(-DQT_PLUGIN)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/shared)
include_directories(${CMAKE_SOURCE_DIR}/src/plugins)

if(NOT APPLE)
    set(CMAKE_INSTALL_RPATH
        "$ORIGIN/../"
    )
endif()


set(LIBRARY_OUTPUT_PATH ${PLUGIN_OUTPUT_PATH})

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



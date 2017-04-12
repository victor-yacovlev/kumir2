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
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags -Wl,-rpath,'\$ORIGIN'")
endif(NOT APPLE AND NOT MSVC)
# -- mac
if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags -Wl,-rpath,'/'")
endif(APPLE)


if (MSVC_IDE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
endif (MSVC_IDE)

macro(kumir2_handle_translation pluginName)
    file(GLOB ts_files ${CMAKE_SOURCE_DIR}/share/kumir2/translations/${pluginName}_??.ts)
    foreach(ts_file IN ITEMS ${ts_files})
        file(MAKE_DIRECTORY ${SHARE_PATH}/translations/)
        get_filename_component(basename ${ts_file} NAME_WE)
        set(qm_target ${basename}_qm)
        #add_custom_target(${qm_target} ALL ${QT_LRELEASE_EXECUTABLE} -qm ${SHARE_PATH}/translations/${basename}.qm ${ts_file})
        add_custom_command(TARGET ${pluginName} PRE_BUILD
                COMMAND ${QT_LRELEASE_EXECUTABLE} -qm ${SHARE_PATH}/translations/${basename}.qm ${ts_file}
                #COMMAND echo aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
        )
        install(FILES ${SHARE_PATH}/translations/${basename}.qm DESTINATION ${RESOURCES_DIR}/translations/)
    endforeach(ts_file)
endmacro(kumir2_handle_translation)

macro(kumir2_copy_resources pluginDir)
    set(SRC_RES ${CMAKE_SOURCE_DIR}/share/kumir2/${pluginDir}/)
    file(GLOB_RECURSE resfiles RELATIVE ${SRC_RES} ${SRC_RES}/*)
    foreach(res IN ITEMS ${resfiles})
            get_filename_component(subdir ${res} PATH)
            file(MAKE_DIRECTORY ${SHARE_PATH}/${pluginDir}/${subdir})
            file(COPY ${CMAKE_SOURCE_DIR}/share/kumir2/${pluginDir}/${res} DESTINATION ${SHARE_PATH}/${pluginDir}/${subdir})
            install(FILES ${SHARE_PATH}/${pluginDir}/${res} DESTINATION ${RESOURCES_DIR}/${pluginDir}/${subdir})
    endforeach(res)
endmacro(kumir2_copy_resources)

function(kumir2_add_library LibraryName)
    string(TOUPPER "${LibraryName}_LIBRARY" _LIB_DEF)
    add_library(${LibraryName} SHARED ${ARGN})
    set_property(TARGET ${LibraryName} APPEND PROPERTY COMPILE_DEFINITIONS ${_LIB_DEF})
    kumir2_handle_translation(${LibraryName})
    install(TARGETS ${LibraryName} DESTINATION ${KUMIR2_LIBS_DIR})
endfunction(kumir2_add_library)

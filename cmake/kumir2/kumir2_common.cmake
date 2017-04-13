find_package(PythonInterp 3.2.0 REQUIRED)
include(CMakeParseArguments)

set(MINIMUM_QT5_VERSION 5.3.0)

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/query_version_info.py" "--mode=cmake_disabled_modules"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE DISABLED_SUBDIRS
)

if(NOT DEFINED KUMIR2_INSTALL_PREFIX)
    set(KUMIR2_INSTALL_PREFIX "/usr")
endif(NOT DEFINED KUMIR2_INSTALL_PREFIX)

function(add_opt_subdirectory SUBDIR_NAME)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR_NAME})
        string(FIND "${DISABLED_SUBDIRS}" "[${SUBDIR_NAME}]" SubdirDisableIndex)
        if(${SubdirDisableIndex} EQUAL -1)
            add_subdirectory(${SUBDIR_NAME})
        else()
            message(STATUS "Component ${SUBDIR_NAME} disabled for this version")
        endif()
    endif()
endfunction(add_opt_subdirectory)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(NOT USE_QT GREATER 4)
    message(WARNING "Support for Qt4 will be discontinued soon. Please upgrade to Qt5!")
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Find lrelease, moc and uic tools from Qt
if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 ${MINIMUM_QT5_VERSION} COMPONENTS Core Widgets REQUIRED)
    set(QT_LRELEASE_EXECUTABLE "${_qt5Core_install_prefix}/bin/lrelease")
else()
    # Find Qt4
    find_package(Qt4 4.8.0 COMPONENTS QtCore REQUIRED)
endif()


# Finds Qt modules by components name
function(kumir2_use_qt)
    set(QT_COMPONENTS)
    set(_QT_LIBRARIES)
    foreach(component ${ARGN})
        if(${USE_QT} GREATER 4)
            if(${component} MATCHES "Gui")
                list(APPEND QT_COMPONENTS "Gui" "Widgets" "PrintSupport")
            else()
                list(APPEND QT_COMPONENTS ${component})
            endif()
        else()
            list(APPEND QT_COMPONENTS "Qt${component}")
        endif()
    endforeach(component)
    if(${USE_QT} GREATER 4)
        find_package(Qt5 ${MINIMUM_QT5_VERSION} COMPONENTS ${QT_COMPONENTS} REQUIRED)
        foreach(component ${QT_COMPONENTS})
            include_directories("${Qt5${component}_INCLUDE_DIRS}")
            list(APPEND _QT_LIBRARIES "${Qt5${component}_LIBRARIES}")
        endforeach(component)
    else()
        set(QT_USE_QTMAIN 1)
        find_package(Qt4 4.8.0 COMPONENTS ${QT_COMPONENTS} REQUIRED)
        include(${QT_USE_FILE})
    endif()
    set(QT_LIBRARIES ${_QT_LIBRARIES} PARENT_SCOPE)
endfunction(kumir2_use_qt)

macro(kumir2_wrap_cpp)
    if(${USE_QT} GREATER 4)
        qt5_wrap_cpp(${ARGN})
    else()
        qt4_wrap_cpp(${ARGN})
    endif()
endmacro(kumir2_wrap_cpp)

macro(kumir2_wrap_ui)
    if(${USE_QT} GREATER 4)
        qt5_wrap_ui(${ARGN})
    else()
        qt4_wrap_ui(${ARGN})
    endif()
endmacro(kumir2_wrap_ui)

macro(kumir2_add_resources)
    if(${USE_QT} GREATER 4)
        qt5_add_resources(${ARGN})
    else()
        qt4_add_resources(${ARGN})
    endif()
endmacro(kumir2_add_resources)

function(kumir2_run_lrelease NAME FROM TO)
    add_custom_command(
        TARGET ${NAME} POST_BUILD
        COMMAND ${QT_LRELEASE_EXECUTABLE} -qm ${TO} ${FROM}
    )
    install(
        FILES ${TO}
        DESTINATION ${KUMIR2_RESOURCES_DIR}/translations/
    )
endfunction(kumir2_run_lrelease)

function(kumir2_handle_translation Name Lang)
    set(CANDIDATE_1 "${CMAKE_SOURCE_DIR}/share/kumir2/translations/${Name}_${Lang}.ts")
    set(CANDIDATE_2 "${CMAKE_CURRENT_SOURCE_DIR}/${Name}_${Lang}.ts")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/translations")
    set(QM "${CMAKE_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/translations/${Name}_${Lang}.qm")
    if(EXISTS ${CANDIDATE_1})
        kumir2_run_lrelease(${Name} ${CANDIDATE_1} ${QM})
    elseif(EXISTS ${CANDIDATE_2})
        kumir2_run_lrelease(${Name} ${CANDIDATE_2} ${QM})
    endif()
endfunction(kumir2_handle_translation)

function(kumir2_copy_resources ComponentName)
    string(TOLOWER ${ComponentName} pluginDir)
    set(SRC_RES ${CMAKE_SOURCE_DIR}/share/kumir2/${pluginDir}/)
    if(EXISTS ${SRC_RES})
        file(GLOB_RECURSE resfiles RELATIVE ${SRC_RES} ${SRC_RES}/*)
        foreach(res IN ITEMS ${resfiles})
            get_filename_component(subdir ${res} PATH)
            file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/${pluginDir}/${subdir}")
            file(COPY "${CMAKE_SOURCE_DIR}/share/kumir2/${pluginDir}/${res}" DESTINATION "${CMAKE_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/${pluginDir}/${subdir}")
            install(DIRECTORY "${CMAKE_SOURCE_DIR}/share/kumir2/${pluginDir}/${subdir}" DESTINATION "${KUMIR2_RESOURCES_DIR}/${pluginDir}/${subdir}")
        endforeach(res)
    endif()
endfunction(kumir2_copy_resources)

function(kumir2_add_library)
    cmake_parse_arguments(PARSED_ARGS "" "NAME" "SOURCES;LIBRARIES" ${ARGN})
    string(TOUPPER "${PARSED_ARGS_NAME}_LIBRARY" LIB_DEF)
    add_library(${PARSED_ARGS_NAME} SHARED ${PARSED_ARGS_SOURCES})
    if(PARSED_ARGS_LIBRARIES)
        target_link_libraries(${PARSED_ARGS_NAME} ${PARSED_ARGS_LIBRARIES})
    endif(PARSED_ARGS_LIBRARIES)
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_DEFINITIONS ${LIB_DEF})
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_FLAGS "${KUMIR2_CXXFLAGS} ${KUMIR2_CXXFLAGS_${CMAKE_BUILD_TYPE}}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY LINK_FLAGS "${KUMIR2_LIBRARY_LINKER_FLAGS}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${KUMIR2_LIBS_DIR}")
    kumir2_handle_translation(${PARSED_ARGS_NAME} "ru")
    kumir2_copy_resources(${PARSED_ARGS_NAME})
    install(TARGETS ${PARSED_ARGS_NAME} DESTINATION ${KUMIR2_LIBS_DIR})
endfunction(kumir2_add_library)

function(kumir2_add_plugin)
    cmake_parse_arguments(PARSED_ARGS "" "NAME" "SOURCES;LIBRARIES" ${ARGN})    
    add_library(${PARSED_ARGS_NAME} SHARED ${PARSED_ARGS_SOURCES})
    if(PARSED_ARGS_LIBRARIES)
        target_link_libraries(${PARSED_ARGS_NAME} ${PARSED_ARGS_LIBRARIES})
    endif(PARSED_ARGS_LIBRARIES)
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_FLAGS "${KUMIR2_CXXFLAGS} ${KUMIR2_CXXFLAGS_${CMAKE_BUILD_TYPE}}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY LINK_FLAGS "${KUMIR2_PLUGIN_LINKER_FLAGS}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${KUMIR2_PLUGINS_DIR}")
    kumir2_handle_translation(${PARSED_ARGS_NAME} "ru")
    kumir2_copy_resources(${PARSED_ARGS_NAME})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PARSED_ARGS_NAME}.pluginspec")
        file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/${PARSED_ARGS_NAME}.pluginspec" DESTINATION "${CMAKE_BINARY_DIR}/${KUMIR2_PLUGINS_DIR}")
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${PARSED_ARGS_NAME}.pluginspec" DESTINATION ${KUMIR2_PLUGINS_DIR})
    elseif(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${PARSED_ARGS_NAME}.pluginspec")
        file(COPY "${CMAKE_CURRENT_BINARY_DIR}/${PARSED_ARGS_NAME}.pluginspec" DESTINATION "${CMAKE_BINARY_DIR}/${KUMIR2_PLUGINS_DIR}")
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PARSED_ARGS_NAME}.pluginspec" DESTINATION ${KUMIR2_PLUGINS_DIR})
    endif()
    install(TARGETS ${PARSED_ARGS_NAME} DESTINATION ${KUMIR2_PLUGINS_DIR})
endfunction(kumir2_add_plugin)

function(kumir2_add_actor)
    cmake_parse_arguments(PARSED_ARGS "" "NAME" "SOURCES;LIBRARIES" ${ARGN})
    string(TOLOWER "${PARSED_ARGS_NAME}.json" JSON_FILE_NAME)
    string(TOLOWER "${PARSED_ARGS_NAME}modulebase" MODULEBASE)
    string(TOLOWER "${PARSED_ARGS_NAME}plugin" PLUGIN)
    set(PLUGIN_SPEC "Actor${PARSED_ARGS_NAME}.pluginspec")
    set(PLUGIN_NAME "Actor${PARSED_ARGS_NAME}")
    if(EXISTS "${CMAKE_SOURCE_DIR}/scripts/gen_actor_source.py")
        set(GEN_ACTOR_SOURCE_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/gen_actor_source.py")
    else()
        set(GEN_ACTOR_SOURCE_SCRIPT "${KUMIR2_INSTALL_PREFIX}/${KUMIR2_SDK_SCRIPTS_DIR}/gen_actor_source.py")
    endif()
    set(MOC_PARAMS)
    if(${USE_QT} GREATER 4)
        foreach(IDIR ${Qt5Core_INCLUDE_DIRS})
            list(APPEND MOC_PARAMS "-I${IDIR}")
        endforeach()
    endif()
    set(SOURCES_GENERATED ${PLUGIN}.cpp ${MODULEBASE}.cpp)
    set(HEADERS_GENERATED ${PLUGIN}.h ${MODULEBASE}.h)
    add_custom_command(
        OUTPUT
            ${SOURCES_GENERATED}
            ${HEADERS_GENERATED}
            ${PLUGIN_SPEC}
        COMMAND ${PYTHON_EXECUTABLE}
            ${GEN_ACTOR_SOURCE_SCRIPT}
            "--update"
            "${CMAKE_CURRENT_SOURCE_DIR}/${JSON_FILE_NAME}"
        DEPENDS
            "${GEN_ACTOR_SOURCE_SCRIPT}"
            "${CMAKE_CURRENT_SOURCE_DIR}/${JSON_FILE_NAME}"
    )
    add_custom_command(
        OUTPUT ${MODULEBASE}.moc.cpp
        COMMAND ${QT_MOC_EXECUTABLE}
            ${MOC_PARAMS}
            "-I${CMAKE_SOURCE_DIR}/include/"
            "-I${CMAKE_SOURCE_DIR}/src/"
            "-o${CMAKE_CURRENT_BINARY_DIR}/${MODULEBASE}.moc.cpp"
            "${CMAKE_CURRENT_BINARY_DIR}/${MODULEBASE}.h"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${MODULEBASE}.h"
    )
    add_custom_command(
        OUTPUT ${PLUGIN}.moc.cpp
        COMMAND ${QT_MOC_EXECUTABLE}
            ${MOC_PARAMS}
            "-I${CMAKE_SOURCE_DIR}/include/"
            "-I${CMAKE_SOURCE_DIR}/src/"
            "-o${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN}.moc.cpp"
            "${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN}.h"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN}.h"
    )

    set(MOC_SOURCES_GENERATED ${PLUGIN}.moc.cpp ${MODULEBASE}.moc.cpp)


    kumir2_add_plugin(
        NAME        "Actor${PARSED_ARGS_NAME}"
        SOURCES     ${PARSED_ARGS_SOURCES}
                    ${SOURCES_GENERATED}
                    ${MOC_SOURCES_GENERATED}
        LIBRARIES   ${PARSED_ARGS_LIBRARIES}
                    ${QT_LIBRARIES}
                    ExtensionSystem Widgets
    )

    kumir2_copy_resources("actors/${PARSED_ARGS_NAME}")

endfunction(kumir2_add_actor)

function(kumir2_add_launcher)
    cmake_parse_arguments(PARSED_ARGS "" "NAME;SPLASHSCREEN;CONFIGURATION" "" ${ARGN})
    set(LAUNCHER_SOURCES "${CMAKE_SOURCE_DIR}/src/app/main.cpp")
    if(WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PARSED_ARGS_NAME}.rc")
        list(APPEND LAUNCHER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${PARSED_ARGS_NAME}.rc")
        add_executable(${PARSED_ARGS_NAME} WIN32 ${LAUNCHER_SOURCES})
        if(${USE_QT} GREATER 4)
            list(APPEND QT_LIBRARIES ${Qt5Core_QTMAIN_LIBRARIES})
        endif()
    else()
        add_executable(${PARSED_ARGS_NAME} ${LAUNCHER_SOURCES})
    endif()
    target_link_libraries(${PARSED_ARGS_NAME} ${QT_LIBRARIES} ExtensionSystem)
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_FLAGS "${KUMIR2_CXXFLAGS} ${KUMIR2_CXXFLAGS_${CMAKE_BUILD_TYPE}}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY LINK_FLAGS ${KUMIR2_LAUNCHER_LINKER_FLAGS})
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${PARSED_ARGS_CONFIGURATION}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_DEFINITIONS SPLASHSCREEN="${PARSED_ARGS_SPLASHSCREEN}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_DEFINITIONS KUMIR2_PLUGINS_DIR="${KUMIR2_PLUGINS_DIR}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_DEFINITIONS KUMIR2_RESOURCES_DIR="${KUMIR2_RESOURCES_DIR}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_DEFINITIONS KUMIR2_EXEC_DIR="${KUMIR2_EXEC_DIR}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_DEFINITIONS KUMIR2_LIBS_DIR="${KUMIR2_LIBS_DIR}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${KUMIR2_EXEC_DIR}")
    install(TARGETS ${PARSED_ARGS_NAME} DESTINATION ${KUMIR2_EXEC_DIR})
    if(KUMIR2_XDG_APPLICATION_DIR AND EXISTS "${CMAKE_SOURCE_DIR}/${PARSED_ARGS_NAME}.desktop")
        file(COPY "${CMAKE_SOURCE_DIR}/${PARSED_ARGS_NAME}.desktop" DESTINATION "${CMAKE_BINARY_DIR}/${KUMIR2_XDG_APPLICATION_DIR}")
        install(FILES "${CMAKE_BINARY_DIR}/${KUMIR2_XDG_APPLICATION_DIR}/${PARSED_ARGS_NAME}.desktop" DESTINATION ${KUMIR2_XDG_APPLICATION_DIR})
    endif()
endfunction(kumir2_add_launcher)

function(kumir2_add_tool)
    cmake_parse_arguments(PARSED_ARGS "" "NAME" "SOURCES;LIBRARIES;CXXFLAGS" ${ARGN})
    if(WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PARSED_ARGS_NAME}.rc")
        list(APPEND PARSED_ARGS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${PARSED_ARGS_NAME}.rc")
        add_executable(${PARSED_ARGS_NAME} WIN32 ${PARSED_ARGS_SOURCES})
        if(${USE_QT} GREATER 4)
            list(APPEND PARSED_ARGS_LIBRARIES ${Qt5Core_QTMAIN_LIBRARIES})
        endif()
    else()
        add_executable(${PARSED_ARGS_NAME} ${PARSED_ARGS_SOURCES})
    endif()
    if(PARSED_ARGS_LIBRARIES)
        target_link_libraries(${PARSED_ARGS_NAME} ${PARSED_ARGS_LIBRARIES})
    endif(PARSED_ARGS_LIBRARIES)
    if(PARSED_ARGS_CXXFLAGS)
        set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_FLAGS "${PARSED_ARGS_CXXFLAGS} ${KUMIR2_CXXFLAGS_${CMAKE_BUILD_TYPE}}")
    else()
        set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY COMPILE_FLAGS "${KUMIR2_CXXFLAGS} ${KUMIR2_CXXFLAGS_${CMAKE_BUILD_TYPE}}")
    endif(PARSED_ARGS_CXXFLAGS)
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY LINK_FLAGS "${KUMIR2_TOOL_LINKER_FLAGS}")
    set_property(TARGET ${PARSED_ARGS_NAME} APPEND PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${KUMIR2_LIBEXECS_DIR}")
    kumir2_handle_translation(${PARSED_ARGS_NAME} "ru")
    kumir2_copy_resources(${PARSED_ARGS_NAME})
    install(TARGETS ${PARSED_ARGS_NAME} DESTINATION ${KUMIR2_LIBEXECS_DIR})
endfunction(kumir2_add_tool)

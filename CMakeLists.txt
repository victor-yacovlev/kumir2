project(Kumir2)
cmake_minimum_required(VERSION 3.0)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/custom_variables.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/custom_variables.cmake")
endif()

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Kumir2 REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/src")


add_subdirectory(src)


# Copy and create install targets for top-level resources
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/userdocs")
file(GLOB_RECURSE resfiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/userdocs ${CMAKE_CURRENT_SOURCE_DIR}/userdocs/*)
foreach(res IN ITEMS ${resfiles})
    if(NOT ${res} MATCHES ^[.].*)
        get_filename_component(subdir ${res} PATH)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/userdocs/${subdir}")
        file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/userdocs/${res}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/userdocs/${subdir}")
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/userdocs/${res}" DESTINATION "${KUMIR2_RESOURCES_DIR}/userdocs/${subdir}")
    endif()
endforeach(res)

# Courses in dist TODO move the code to function and reuse it
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/courses")
file(GLOB_RECURSE resfiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/courses ${CMAKE_CURRENT_SOURCE_DIR}/courses/*)
foreach(res IN ITEMS ${resfiles})
    if(NOT ${res} MATCHES ^[.].*)
        get_filename_component(subdir ${res} PATH)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/courses/${subdir}")
        file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/courses/${res}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/courses/${subdir}")
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/courses/${res}" DESTINATION "${KUMIR2_RESOURCES_DIR}/courses/${subdir}")
    endif()
endforeach(res)

# In-App icons TODO move the code to function and reuse it
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/icons")
file(GLOB_RECURSE icons RELATIVE ${CMAKE_SOURCE_DIR}/share/kumir2/icons ${CMAKE_SOURCE_DIR}/share/kumir2/icons/*)
foreach(icon IN ITEMS ${icons})
    get_filename_component(subdir ${icon} PATH)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/icons/${subdir}")
    file(COPY "${CMAKE_SOURCE_DIR}/share/kumir2/icons/${icon}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_RESOURCES_DIR}/icons/${subdir}")
    install(FILES "${CMAKE_SOURCE_DIR}/${KUMIR2_RESOURCES_DIR}/icons/${icon}" DESTINATION "${KUMIR2_RESOURCES_DIR}/icons/${subdir}")
endforeach(icon)

# Development files
install(DIRECTORY include/ DESTINATION include/ FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY src/kumir2-libs/ DESTINATION include/kumir2-libs/ FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY src/kumir2-libs/ DESTINATION include/kumir2-libs/ FILES_MATCHING PATTERN "*.table")
install(
    FILES scripts/gen_actor_source.py
    DESTINATION ${KUMIR2_SDK_SCRIPTS_DIR}
    PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_READ
        WORLD_EXECUTE WORLD_READ
)
install(DIRECTORY cmake/ DESTINATION "${KUMIR2_SDK_CMAKE_DIR}")
install(FILES src/app/kumir2-launcher.cpp DESTINATION ${KUMIR2_SDK_SRC_DIR})

# Generate and install CMake file describing current build configuration
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/distribution_build_config.cmake" "# Installed build configuration\n")
file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/distribution_build_config.cmake"
    "set(USE_QT ${USE_QT})\n"
    "set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})\n"
    "set(KUMIR2_ROOT ${CMAKE_INSTALL_PREFIX})\n"
    "set(KUMIR2_SDK_SCRIPTS_DIR ${KUMIR2_SDK_SCRIPTS_DIR})\n"
)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/distribution_build_config.cmake"
    DESTINATION "${KUMIR2_SDK_CMAKE_DIR}/kumir2"
)

# XDG mime types handling
if(NOT WIN32 AND NOT APPLE)
    set(ICONS_DIR "${CMAKE_SOURCE_DIR}/app_icons/linux/hicolor")
    message(STATUS "icons dir : ${ICONS_DIR}")
    file(GLOB_RECURSE icons RELATIVE ${ICONS_DIR} "${ICONS_DIR}/*/mimetypes/*.*")
    foreach(icon IN ITEMS ${icons})
        get_filename_component(subdir ${icon} PATH)
        install(FILES "${ICONS_DIR}/${icon}" DESTINATION "${KUMIR2_XDG_ICONS_DIR}/hicolor/${subdir}")
    endforeach(icon)
    install(FILES "${CMAKE_SOURCE_DIR}/share/mime/packages/kumir2-mimetypes.xml" DESTINATION ${KUMIR2_XDG_MIME_PACKAGES_DIR})
endif(NOT WIN32 AND NOT APPLE)

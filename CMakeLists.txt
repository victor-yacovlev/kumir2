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


if(WIN32)
    # Do nothing
elseif(APPLE)
    # TODO Make package-related stuff
else()
    # Create XDG-compliant filesystem structure for applciations and icons

    # Application icons
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_XDG_ICONS_DIR}")
    file(GLOB_RECURSE icons RELATIVE ${CMAKE_SOURCE_DIR}/app_icons/linux ${CMAKE_SOURCE_DIR}/app_icons/linux/*)
    foreach(icon IN ITEMS ${icons})
        get_filename_component(subdir ${icon} PATH)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_XDG_ICONS_DIR}/${subdir}")
        file(COPY "${CMAKE_SOURCE_DIR}/app_icons/linux/${icon}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_XDG_ICONS_DIR}/${subdir}")
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${KUMIR2_XDG_ICONS_DIR}/${icon}" DESTINATION "${KUMIR2_XDG_ICONS_DIR}/${subdir}")
    endforeach(icon)
endif()

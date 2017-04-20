project(kumir2-open)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui)

set(EXTRA_LIBS)

set(SOURCES 
    main.cpp
    messager.cpp
    dialog.cpp
    appmanager.cpp
    procmanager.cpp
    settings.cpp
)

set(HEADERS
    messager.h
    dialog.h
    appmanager.h
    procmanager.h
    settings.h
)

set(FORMS
    dialog.ui
)

set(MOC_HEADERS
    dialog.h
)


kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
kumir2_wrap_ui(UI_SOURCES ${FORMS})

if(NOT WIN32 AND NOT APPLE)
    set(SOURCES ${SOURCES} messager_unix.cpp procmanager_linux.cpp)
    set(HEADERS ${HEADERS} messager_unix.h procmanager_linux.h)
endif()

if(WIN32)
    add_definitions(-DPSAPI_VERSION=1)
    set(SOURCES ${SOURCES} procmanager_winapi.cpp messager_shm.cpp)
    set(HEADERS ${HEADERS} procmanager_winapi.h messager_shm.h)
    set(EXTRA_LIBS psapi kernel32)
endif()

kumir2_add_tool(
    NAME        kumir2-open
    SOURCES     ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES}
    LIBRARIES   ${QT_LIBRARIES} ${EXTRA_LIBS}
)

if(KUMIR2_XDG_MIME_PACKAGES_DIR)
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${KUMIR2_XDG_MIME_PACKAGES_DIR}")
    file(
        COPY "${CMAKE_SOURCE_DIR}/share/mime/packages/kumir2-mimetypes.xml"
        DESTINATION "${CMAKE_BINARY_DIR}/${KUMIR2_XDG_MIME_PACKAGES_DIR}"
    )
    install(
        FILES "${CMAKE_SOURCE_DIR}/share/mime/packages/kumir2-mimetypes.xml"
        DESTINATION ${KUMIR2_XDG_MIME_PACKAGES_DIR}
    )
endif(KUMIR2_XDG_MIME_PACKAGES_DIR)

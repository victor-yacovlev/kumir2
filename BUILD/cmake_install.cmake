# Install script for directory: /Users/denis/kumir2Master

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorComplexnumbers.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorDraw.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorGrasshopper.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorIsometricRobot.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/ActorIsometricRobot" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorIsometricRobot/coordinates.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorKeyboard.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorPainter.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/ActorPainter" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorPainter/CMYK_model.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/ActorPainter" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorPainter/CMYK_model.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/ActorPainter" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorPainter/RGB_model.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/ActorPainter" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorPainter/RGB_model.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorRobot.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorTurtle.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/ActorVodoley.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/index-classic.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/index-highgrade.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/index-ide.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/index-macx.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/index-python.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/index-teacher.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/ActorFiles.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/ActorStringUtils.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/builtin-actors-highgrade.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/compound_commands.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/files.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/indentifiers.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/index-classic.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/index-highgrade.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/lexical_cast.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/math.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/short_man.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/simple_commands.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/simple_string.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/standard_library-classic.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/standard_library-highgrade.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/stringutils.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/structure.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/language" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/language/sys_func.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/cources" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/cources/cources.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/cources" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/cources/kumir-course-window.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/cources" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/cources/kumir-course-window.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/cources" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/cources/kumir-course-window.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/editor" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/editor/editor.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/editor/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/editor/images/edit-find.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/editor/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/editor/images/edit-find.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/editor/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/editor/images/edit-find.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/editor/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/editor/images/edit-replace.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/editor/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/editor/images/edit-replace.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/editor/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/editor/images/edit-replace.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/index-classic.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/index-highgrade.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/index-ide.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/index-teacher.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/intro" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/intro/classic.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/intro" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/intro/highgrade.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/intro" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/intro/teacher.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/buttons.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/images/kumir-classic-mainwindow.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/images/kumir-classic-mainwindow.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/images/kumir-classic-mainwindow.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/images/kumir-ide-mainwindow.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/images/kumir-ide-mainwindow.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/images/kumir-ide-mainwindow.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/images/kumir-ide-startpage.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/images/kumir-ide-startpage.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/images/kumir-ide-startpage.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/menu.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/overview-with-tabs.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/overview-without-tabs.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/with-tabs.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/mainwindow" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/mainwindow/without-tabs.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/run" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/run/run.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/secondarywindows/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/secondarywindows/images/kumir-docking-places.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/secondarywindows/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/secondarywindows/images/kumir-docking-places.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/secondarywindows/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/secondarywindows/images/kumir-docking-places.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/secondarywindows/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/secondarywindows/images/secondary-window-title.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/secondarywindows/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/secondarywindows/images/secondary-window-title.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/secondarywindows/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/secondarywindows/images/secondary-window-title.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/secondarywindows" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/secondarywindows/secondarywindows.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_analysis.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_analysis.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_analysis.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_edit.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_edit.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_edit.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_pause.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_pause.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_pause.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_run.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_run.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/images/mode_run.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/session" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/session/session.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/teacher/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/teacher/images/kumir-teacher-mode-hidden.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/teacher/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/teacher/images/kumir-teacher-mode-hidden.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/teacher/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/teacher/images/kumir-teacher-mode-hidden.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/teacher/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/teacher/images/kumir-teacher-mode-protected.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/teacher/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/teacher/images/kumir-teacher-mode-protected.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/teacher/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/teacher/images/kumir-teacher-mode-protected.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/teacher/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/teacher/images/kumir-teacher-mode.pdf")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/teacher/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/teacher/images/kumir-teacher-mode.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/teacher/images" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/teacher/images/kumir-teacher-mode.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/userdocs/system/teacher" TYPE FILE FILES "/Users/denis/kumir2Master/userdocs/system/teacher/teacher.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/1_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/1_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/10_1_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/10_1_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/10_1_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/10_2_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/10_2_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/10_2_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/11_1_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/11_1_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/12_1_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/12_1_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/12_2_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/12_2_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/13_1_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/13_1_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/1_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/1_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/1_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/1_4.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/2_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/2_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/2_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/3_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/4_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/4_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/5_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/5_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/5_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/6_0_0.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/6_0_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/6_1_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/6_1_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/6_2_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/6_2_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/6_3_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/6_3_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/6_4_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/6_4_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/7_1_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/7_1_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/7_1_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/7_2_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/7_2_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/7_3_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/7_3_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/7_4_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/7_5_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/7_5_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/8_1_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/8_1_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/8_2_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/8_2_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/8_3_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/8_3_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/8_3_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/9_1_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/9_1_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/9_1_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/9_2_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/9_2_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/9_3_1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/9_3_2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/9_3_3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/9_3_4.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/alg_example.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/circ_example.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/labirint1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/labirint2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/labirint3.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/m1.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/m2.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/perem_example.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/repeat_example.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Fields" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Fields/while_example.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/alg1.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/alg2.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/alg_example.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/alg_example.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/circle_for1.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/circles_example.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/cond1.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/cond2.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/cond3.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/file1.txt")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/file2.txt")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/file_med.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/file_summa.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/forCircle1.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/if_example.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/koridor1.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/koridor2.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/maksimum.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/obkhod_lab.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/perem_example.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/rep_example.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/summa.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/v_kletku_B.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/v_kletku_B2_3.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/var1.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/var2.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/var3.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/while1.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/while5.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum/Texts" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/Texts/while_example.kum")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/dummy.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/perem_example.fil")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/courses/practicum" TYPE FILE FILES "/Users/denis/kumir2Master/courses/practicum/practicum.kurs.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/NOTICE.txt")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/draw.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/drawer.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/drawer_22x22.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/grasshopper-pult.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/grasshopper-pult.svg")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/grasshopper_22x22.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/kuznec_22x22.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/painter.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/painter_22x22.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/robot-pult.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/robot-pult_22x22.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/robot.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/robot25d-pult_22x22.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/robot25d_22x22.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/robot_22x22.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/turtle-pult.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/turtle-pult_22x22.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/turtle.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/turtle_22x22.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/vodoley-pult.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/vodoley-pult_22x22.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/actors" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/actors/vodoley.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/course.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/debug-step-into.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/debug-step-out.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/debug-step-over.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/document-close.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/document-edit.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/document-new.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/document-open.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/document-save-all.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/document-save-as.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/document-save.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/edit-clear.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/edit-copy.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/edit-cut.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/edit-delete.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/edit-paste.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/edit-redo.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/edit-select-all.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/edit-undo.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_oxygen" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_oxygen/NOTICE.txt")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_oxygen" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_oxygen/document-save-all.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_oxygen" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_oxygen/document-save-as.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_oxygen" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_oxygen/document-save.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/NOTICE.txt")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/category_core.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/clean_pane_small.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/debugger_start.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/debugger_stepinto_small.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/debugger_steponeproc_small.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/debugger_stepoverproc_small.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/editcopy.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/editcut.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/editpaste.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/filenew.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/fileopen.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/filesave.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/redo.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/run.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/stop.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/from_qtcreator" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/from_qtcreator/undo.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/16x16" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/16x16/statusbar-clear.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/16x16" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/16x16/statusbar-copy.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/16x16" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/16x16/statusbar-edit.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/16x16" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/16x16/statusbar-save.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/16x16" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/16x16/statusbar-terminal.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/18x18" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/18x18/statusbar-clear.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/18x18" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/18x18/statusbar-copy.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/18x18" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/18x18/statusbar-edit.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/18x18" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/18x18/statusbar-save.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/18x18" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/18x18/statusbar-terminal.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/edit-copy.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/edit-cut.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/edit-paste.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/edit-redo.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/edit-undo.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/file-new.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/file-open.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/file-save.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/run-blind.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/run-regular.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/run-step-in.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/run-step-out.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/run-step-over.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/run-stop.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/window-cources.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/window-robot-control.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/22x22" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/22x22/window-robot.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons/iconset/24x24" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/iconset/24x24/fullscreen-exit.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/media-playback-start.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/media-playback-stop.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/media-seek-forward.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/media-skip-forward.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/icons" TYPE FILE FILES "/Users/denis/kumir2Master/Kumir.app/Contents/Resources/icons/system-run.png")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/" TYPE DIRECTORY FILES "/Users/denis/kumir2Master/include/" FILES_MATCHING REGEX "/[^/]*\\.h[^/]*$")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/kumir2-libs/" TYPE DIRECTORY FILES "/Users/denis/kumir2Master/src/kumir2-libs/" FILES_MATCHING REGEX "/[^/]*\\.h[^/]*$")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/kumir2-libs/" TYPE DIRECTORY FILES "/Users/denis/kumir2Master/src/kumir2-libs/" FILES_MATCHING REGEX "/[^/]*\\.table$")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/devel" TYPE FILE PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ FILES "/Users/denis/kumir2Master/scripts/gen_actor_source.py")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/devel/cmake" TYPE DIRECTORY FILES "/Users/denis/kumir2Master/cmake/")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/Resources/devel/cmake/kumir2" TYPE FILE FILES "/Users/denis/kumir2Master/BUILD/distribution_build_config.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/denis/kumir2Master/BUILD/src/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/denis/kumir2Master/BUILD/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

#!/bin/bash
FILES=./Plugins/*.dylib
for f in $FILES
do
  echo "Processing $f file..."
  install_name_tool -change  QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $f
  install_name_tool -change  QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $f
  install_name_tool -change  QtSvg.framework/Versions/4/QtSvg @executable_path/../Frameworks/QtSvg.framework/Versions/4/QtSvg $f
  install_name_tool -change  QtDeclarative.framework/Versions/4/QtDeclarative @executable_path/../Frameworks/QtDeclarative.framework/Versions/4/QtDeclarative $f
  install_name_tool -change  QtScript.framework/Versions/4/QtScript  @executable_path/../Frameworks/QtScript.framework/Versions/4/QtScript  $f
  install_name_tool -change  QtXmlPatterns.framework/Versions/4/QtXmlPatterns  @executable_path/../Frameworks/QtXmlPatterns.framework/Versions/4/QtXmlPatterns  $f
  install_name_tool -change  QtSql.framework/Versions/4/QtSql  @executable_path/../Frameworks/QtSql.framework/Versions/4/QtSql  $f
  install_name_tool -change  QtNetwork.framework/Versions/4/QtNetwork  @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork  $f

  
  install_name_tool -change  QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml $f


  # take action on each file. $f store current file name
  
done
install_name_tool -change ../libExtensionSystem.dylib   @executable_path/../Libs/libExtensionSystem.dylib  ./Plugins/libBrowser.dylib
install_name_tool -change  QtWebKit.framework/Versions/4/QtWebKit @executable_path/../Frameworks/QtWebKit.framework/Versions/4/QtWebKit ./Plugins/libBrowser.dylib
install_name_tool -change  QtWebKit.framework/Versions/4/QtWebKit @executable_path/../Frameworks/QtWebKit.framework/Versions/4/QtWebKit ./Plugins/libCourseManager.dylib
install_name_tool -change ../libExtensionSystem.dylib   @executable_path/../Libs/libExtensionSystem.dylib  ./Plugins/libActorRobot.dylib 
install_name_tool -change ../libWidgets.dylib    @executable_path/../PlugIns/libWidgets.dylib   ./Plugins/libActorRobot.dylib
install_name_tool -change ../libExtensionSystem.dylib   @executable_path/../Libs/libExtensionSystem.dylib  ./Plugins/libActorIsometricRobot.dylib
install_name_tool -change ../libWidgets.dylib    @executable_path/../PlugIns/libWidgets.dylib   ./Plugins/libActorIsometricRobot.dylib
install_name_tool -change ../libExtensionSystem.dylib   @executable_path/../Libs/libExtensionSystem.dylib  ./Plugins/libKumirCompilerTool.dylib  
install_name_tool -change ../libDataFormats.dylib    @executable_path/../PlugIns/libDataFormats.dylib   ./Plugins/libKumirCompilerTool.dylib 

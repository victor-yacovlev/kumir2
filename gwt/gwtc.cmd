rem Arguments
rem   %1 - path to java.exe
rem   %2 - gwt-dev.jar
rem   %3 - gwt-user.jar
rem   %4 - module classpath
rem   %5 - war path
rem   %6 - main module name

if exist %5\%6\%7.nocache.js (
    echo Target already exists
) else (
    %1 -cp %2;%3;%4 com.google.gwt.dev.Compiler -war %5 %6
)

rem Arguments
rem   $1 - gwt-dev.jar
rem   $2 - gwt-user.jar
rem   $3 - module classpath
rem   $4 - war path
rem   $5 - main module name

if exist %4\%5\%5.nocache.js (
    echo Target already exists
) else (
    java -cp %1;%2;%3 com.google.gwt.dev.Compiler -war %4 %5
)

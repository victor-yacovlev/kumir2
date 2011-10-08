#!/bin/sh

# Arguments
#   $1 - gwt-dev.jar
#   $2 - gwt-user.jar
#   $3 - module classpath
#   $4 - war path
#   $5 - main module name

TARGET=$4/$5/$5.nocache.js

echo Target is $TARGET

if [ ! -e $TARGET ]
then
    java -cp $1:$2:$3 com.google.gwt.dev.Compiler -war $4 $5
else
    echo Target already exists
fi

rm -rf $4/../gwt-unitCache
rm -rf $4/WEB-INF

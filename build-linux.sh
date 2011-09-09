#!/bin/bash

QMAKE_BIN=/usr/bin/qmake
ANT_BIN=/usr/bin/ant


# Build web applications
#
# NOTE: this must be performed BEFORE qmake due to 
#       qmake generates "make install" list on existing files

for app in src-www
do
    if [ -e src-www/$app/build.xml ]
    then
	${ANT_BIN} -f src-www/$app/build.xml
	if [ $? -ne 0 ]
	then
	    echo -n "Error building web app "
	    echo $app
	    exit 1
	fi
	mkdir -p share/kumir2/webapps/$app
	cp -R src-www/$app/war/* share/kumir2/webapps/$app/
	# Remove unneccessary WEB-INF, because kumir is not a web-server :-)
	rm -rf share/kumir2/webapps/$app/WEB-INF
    fi
done

# Copy user documentation into helpviewer app
mkdir -p share/kumir2/webapps/helpviewer/data/russian/
cp userdocs/*.xml share/kumir2/webapps/helpviewer/data/russian/


# Determine where is qmake

if [ -e /usr/bin/qmake-qt4 ]
then
    QMAKE_BIN=/usr/bin/qmake-qt4
fi

# Run qmake. It also generates translations via python script
${QMAKE_BIN} CONFIG+=llvm

if [ $? -ne 0 ]
then
    echo "Error configuring by qmake"
    exit 2
fi

# Build kumir binary
make

if [ $? -ne 0 ]
then
    echo "Error building kumir"
    exit 3
fi

# Done
echo "Build success!"
echo "Now you can install kumir to /usr (or any other) prefix by command:"
echo "    make INSTALL_ROOT=/usr install"

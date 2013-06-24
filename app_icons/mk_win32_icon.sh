#!/bin/bash

SIZES=(32 48 64 128 256)
PARAMETERS="-alpha on -channel RGBA -depth 8"

if [ $# -eq 0 ]
then
    echo "Usage: $0 ICON_BASE_NAME"
    echo "  ICON_BASE_NAME	a base name of icon (ex. kumir2-classic)"
    echo
    exit 127
else
    for basename in $@
    do
        echo -n "Converting ${basename}: "
        unset names
        names=""
        for size in ${SIZES[*]}
        do
            names="$names linux/hicolor/${size}x${size}/apps/${basename}.png"
        done
        outname="win32/${basename}.ico"
        command="convert $names $PARAMETERS $outname"
        echo $command
        `$command`
    done
fi
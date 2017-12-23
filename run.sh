#!/bin/sh

map_file="mapblk.bytes"

if [ ! -f "$map_file" ]; then
    lua dumpmap.lua $map_file
fi

while getopts "D" arg
do
    case $arg in
        D)
            lua dumpmap.lua $map_file
            ;;
    esac
done

lua test.lua $map_file > c.html
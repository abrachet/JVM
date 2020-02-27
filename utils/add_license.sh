#!/bin/bash

tempfile=`mktemp /tmp/XXXXXX`
headerfile="utils/header.txt"

concat () {
    cat $headerfile $1 > $tempfile
    cp $tempfile $1
}

for output in `find . -name "*.c" -o -name "*.h" -o -name "*.cpp"`; do
    concat "$output"
done

rm "$tempfile"

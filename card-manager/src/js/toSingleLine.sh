#! /bin/sh

## script to convert javascript files to single line strings for use with C duktape

file="$1"
if [ "$file" == "" ]; then
        echo "Error: need file path"
        exit
fi

cat $file | tr -s '\t' ' ' | tr -d '\n'
echo ""

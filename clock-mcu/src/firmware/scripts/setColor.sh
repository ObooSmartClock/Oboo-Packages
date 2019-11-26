#! /bin/sh

inputHex=$(echo "$1" | tr [a-z] [A-Z])
inputDec=$(echo "ibase=16; $inputHex" | bc)
echo "~L$inputDec;" > /dev/ttyS1

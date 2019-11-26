#! /bin/sh

index="$1"
color="$2"

inputHex=$(echo "$color" | tr [a-z] [A-Z])
inputDec=$(echo "ibase=16; $inputHex" | bc)
cmd="~b$index$inputDec;"
echo "$cmd"
echo "~b$index$inputDec;" > /dev/ttyS1

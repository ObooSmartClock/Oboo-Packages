#! /bin/sh

## Change the currently active card

TOPIC="/card"

toSingleLine () {
	echo $(echo $1  | tr -s '\t' ' ' | tr -d '\n')
}

if [ "$1" == "" ]; then
    echo "ERROR: expecting direction argument: [left|right|first|last]"
    exit
fi



json='{
        "cmd":"select_card",
        "action": "'"$1"'"
    }'
echo "$json"
json=$(toSingleLine "$json")
mosquitto_pub -t $TOPIC -m "$json"
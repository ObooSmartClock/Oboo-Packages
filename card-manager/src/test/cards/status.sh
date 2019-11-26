#! /bin/sh

## set the status

TOPIC="/status"

toSingleLine () {
	echo $(echo $1  | tr -s '\t' ' ' | tr -d '\n')
}

if  [ "$1" == "" ] ||
    [ "$2" == "" ]; then
    echo "ERROR: expecting status argument [alarm|bluetooth|wifi|battery] and value"
    exit
fi

json='{
        "cmd":"update",
        "elements": [
            { "type": "'"$1"'",
              "value": '"$2"'
            }
        ]
    }'
echo "$json"
json=$(toSingleLine "$json")
mosquitto_pub -t $TOPIC -m "$json"
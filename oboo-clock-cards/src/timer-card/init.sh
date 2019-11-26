#! /bin/sh

TOPIC="/card"
NEW_CARD='{
    "cmd":"new_card",
    "bg_color": 13,
    "responseTopic": "/card_new1",
    "elements": [
        {
            "id": 0,
            "type": "image",
            "position": {
                "x": 2,
                "y": 46
            },
            "value": "/usr/bin/img/img_timer_bg.bin"
        },
        {
            "id": 1,
            "type": "image",
            "position": {
                "x": 21,
                "y": 10
            },
            "value": "/usr/bin/img/img_timer_buttons.bin"
        },
    	{
            "id": 2,
            "type": "text",
            "position": {
                "x": 98,
                "y": 103,
                "align": "center"
            },
            "size": 20,
            "value": "14:00"
        },
        {
            "id": 3,
            "type": "text",
            "position": {
                "x": 31,
                "y": 131,
                "align": "center"
            },
            "size": 80,
            "value": "13:52"
        }
    ]
}'

toSingleLine () {
	echo $(echo $1  | tr -s '\t' ' ' | tr -d '\n')
}

setup () {
	json=$(toSingleLine "$NEW_CARD")
	mosquitto_pub -t $TOPIC -m "$json"
}

# Generate a random number
# 	arg1 - min value
#	arg2 - max value
random () {
	echo $(awk -v min=$1 -v max=$2 'BEGIN{srand(); print int(min+rand()*(max-min+1))}')
}

# Issue a message to update a specific element
# 	arg1 - card ID
# 	arg2 - element ID
#	arg3 - new value
update () {
	json='{
	"cmd":"update_card",
	"id": '"$1"',
	"elements": [
		{
			"id": '"$2"',
			"value": "'"$3"'"
		}
		]
	}'
	echo "$json"
	json=$(toSingleLine "$json")
	mosquitto_pub -t $TOPIC -m "$json"
}

# parse arguments
if [ "$1" == "setup" ]; then
	setup
elif [ "$1" == "update" ]; then
  update $2 3 "$3"
fi

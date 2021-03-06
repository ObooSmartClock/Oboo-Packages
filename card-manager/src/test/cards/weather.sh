#! /bin/sh

day=$(date '+%A')
month=

TOPIC="/card"
NEW_CARD='{
    "cmd":"new_card",
    "bg_color": 13,
    "responseTopic": "/card_new1",
    "elements": [
        {
            "id": 0,
            "type": "text",
            "position": {
                "x": 165,
                "y": 30
            },
            "size": 20,
            "value": "#ff0000 deg# C"
        },
	    {
            "id": 1,
            "type": "text",
            "position": {
                "x": 135,
                "y": 45,
                "align": "right"
            },
            "size": 80,
            "value": "-3"
        },
    	{
            "id": 2,
            "type": "text",
            "position": {
                "x": 100,
                "y": 130,
                "align": "right"
            },
            "size": 30,
            "value": "'`date '+%A'`'"
        },
        {
            "id": 3,
            "type": "text",
            "position": {
                "x": 100,
                "y": 160,
                "align": "right"
            },
            "size": 30,
            "value": "'`date '+%b %d'`'"
        },
	    {
            "id": 4,
            "type": "text",
            "position": {
                "x": 100,
                "y": 220,
                "align": "right"
            },
            "size": 20,
            "value": "Precipitation"
        },
        {
            "id": 6,
            "type": "image",
            "position": {
                "x": 10,
                "y": 30
            },
            "value": "sunny"
        },
        {
            "id": 5,
            "type": "text",
            "position": {
                "x": 115,
                "y": 235,
                "align": "right"
            },
            "size": 60,
            "value": "10%"
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
    # expect additional argument specifying the card ID
    # temperature
    update "$2" "1" $(random -15 30)
    # date
    update "$2" "3" "Feb $(random 1 28)"
    # precipitation
    update "$2" "5" "$(random 3 90)%"
    # image
    update "$2" "6" "windy"
fi

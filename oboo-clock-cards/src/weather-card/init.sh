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
                "x": 194,
                "y": 22
            },
            "value": "/usr/bin/img/img_degC.bin"
        },
	    {
            "id": 1,
            "type": "text",
            "position": {
                "x": 130,
                "y": 44,
                "align": "right"
            },
            "size": 80,
            "value": "0"
        },
    	{
            "id": 2,
            "type": "text",
            "position": {
                "x": 117,
                "y": 142,
                "align": "right"
            },
            "size": 30,
            "value": "'`date '+%A'`'"
        },
        {
            "id": 3,
            "type": "text",
            "position": {
                "x": 142,
                "y": 173,
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
            "id": 5,
            "type": "text",
            "position": {
                "x": 115,
                "y": 236,
                "align": "right"
            },
            "size": 60,
            "value": "10%"
        },
        {
            "id": 6,
            "type": "image",
            "position": {
                "x": 10,
                "y": 30
            },
            "value": "/usr/bin/img/img_sunny.bin"
        }
        ,
        {
            "id": 7,
            "type": "image",
            "position": {
                "x": 25,
                "y": 240
            },
            "value": "/usr/bin/img/img_percipitation.bin"
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
    update "$2" "2" "Tuesday\nFeb $(random 1 28)"
    # precipitation
    update "$2" "5" "$(random 3 90)%"
    # image
    update "$2" "6" "/usr/bin/img/img_windy.bin"
fi

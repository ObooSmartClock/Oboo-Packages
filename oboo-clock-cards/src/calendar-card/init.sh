#! /bin/sh

TOPIC="/card"
NEW_CARD='{
    "cmd":"new_card",
    "bg_color": 13,
    "responseTopic": "/card_new1",
    "elements": [
        {
            "id": 0,
            "type": "calendar",
            "position": {
                "x": 60,
                "y": 70
            },
	    "size": 3,
            "value": "",
	    "date": {
              "year":  $(echo $(date "+%Y")),
              "month": $(echo $(date "+%m")),
              "day": $(echo $(date "+%d"))
            }
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

# Issue a message to update a specific element
# 	arg1 - card ID
# 	arg2 - element ID
update () {
	json='{
	"cmd":"update_card",
	"id": '"$1"',
	"elements": [
		{
			"id": '"$2"',
      "date": {
            "year":  $(echo $(date "+%Y")),
            "month": $(echo $(date "+%m")),
            "day": $(echo $(date "+%d"))
      }
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
    # date
    update "$2" "$3"
fi

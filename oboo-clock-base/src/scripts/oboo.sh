#!/bin/sh

## Oboo script for various functionality

### global variables
# options
bVerbose=0
bJson=0
bTest=0
bBase64=0
bError=0

#commands
bCmd=0
scriptCommand=""
scriptOption0=""
scriptOption1=""
scriptOption2=""

_Print () {
	echo "$1"
}

#############################
##### Print Usage ###########
configUsage () {
	_Print "Oboo Configuration File:"
	_Print "	oboo [OPTIONS] config generate"
	_Print "		Create a generic configuration file"
	_Print ""
}

cardsUsage () {
	_Print "Oboo Cards:"
	_Print "	oboo [OPTIONS] cards restart"
	_Print "		Restart the Oboo Cards and Card Manager"
	_Print ""
}

uhttpdUsage () {
	_Print "uHTTPd:"
	_Print "	oboo [OPTIONS] uhttpd cors [enable/disable]"
	_Print "		Configure CORS on uHTTPd and restart the service"
	_Print ""
}

usage () {
	_Print "Functionality:"
	_Print "	Configure Oboo products"
	_Print ""

	_Print "General Usage:"
	_Print "	oboo [OPTIONS] <COMMAND> <PARAMETER>"
	_Print ""

	configUsage
	cardsUsage
	uhttpdUsage

	_Print ""
	_Print "Command Line Options:"
	_Print "  -v      Increase output verbosity"
	#_Print "  -j      Set all output to JSON"
	_Print ""
}

########################################
###     Configuration Functions
########################################
# create a generic config file
configFileGenerate () {
	local config='{
	"config": {
		"hourMode": 12,
		"dayNightLed": true,
		"location": "Toronto, On",
		"brightnessTime": 1,
		"brightnessInfo": 7,
		"tz": "GMT",
		"timezone": "GMT0",
		"autoTimeZone": false
	},
	"cards": {
		"0": {
			"name": "weather",
			"id": 0,
			"tempUnit": "fahrenheit",
			"card": "Weather",
			"location": "Toronto, On",
			"distanceUnit": "imperial"
		}
	}
}'
	# echo "$config"
	echo "$config" > /etc/config.json
}

########################################
###     Card Functions
########################################
# restart the card manager and the cards
cardRestart () {
	/etc/init.d/card-manager restart
	/etc/init.d/oboo-cards restart
}

########################################
###     uhttpd Functions
########################################
# enable/disable uHTTPd CORS
uhttpdSetCors () {
	uci set uhttpd.main.ubus_cors="$1"
	uci commit uhttpd
}

# restart the uhttpd service
uhttpdRestart () {
	/etc/init.d/uhttpd restart
}

# enable cors and restart the service
uhttpdEnableCors () {
	uhttpdSetCors "1"
	uhttpdRestart
}

# disable cors and restart the service
uhttpdDisableCors () {
	uhttpdSetCors "0"
	uhttpdRestart
}

########################################
###     Parse Arguments
########################################


# parse arguments
while [ "$1" != "" ]
do
	case "$1" in
		# options
		-v|--v|-verbose|verbose)
			bVerbose=1
			shift
		;;
		# commands
		config)
			bCmd=1
			scriptCommand="config"
			shift
			scriptOption0="$1"
			shift
		;;
		cards)
			bCmd=1
			scriptCommand="cards"
			shift
			scriptOption0="$1"
			shift
		;;
		uhttpd)
			bCmd=1
			scriptCommand="uhttpd"
			shift
			scriptOption0="$1"
			shift
			scriptOption1="$1"
			shift
		;;
		*)
			echo "ERROR: Invalid Argument: $1"
			usage
			exit
		;;
	esac
done



########################################
########################################
###     Main Program
########################################

## commands
if [ $bCmd == 1 ]; then
	if [ "$scriptCommand" == "config" ]; then
		if [ "$scriptOption0" == "generate" ]; then
			configFileGenerate
		fi
	elif [ "$scriptCommand" == "cards" ]; then
		if [ "$scriptOption0" == "restart" ]; then
			cardRestart
		fi
	elif [ "$scriptCommand" == "uhttpd" ]; then
		if [ "$scriptOption0" == "cors" ]; then
			if [ "$scriptOption1" == "enable" ]; then
				uhttpdEnableCors
			elif [ "$scriptOption1" == "disable" ]; then
				uhttpdDisableCors
			fi
		fi
	fi

	_Print "> Done"
else
	usage
fi

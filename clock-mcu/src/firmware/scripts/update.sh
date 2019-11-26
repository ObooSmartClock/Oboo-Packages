#!/bin/sh

trap "exit 1" EXIT
export TOP_PID=$$


FLASH_SCRIPT="/usr/bin/mcu/flash.sh"
VERSION_PATH="/usr/bin/mcu/version"

# read the flashed version from UCI
getFlashedVersion () {
	local ver=$(uci -q get onion.\@onion[0].coprocessor)
	echo $ver
}

# read the latest coprocessor fw version on fS
getLatestVersion () {
	local ver=$(cat $VERSION_PATH | tr -d '\n' ' ' )
	echo $ver
}

# flash the latest version of firmware and update uci
#		arg1 - latest version
flashFirmware () {
	sh $FLASH_SCRIPT

	uci set onion.@onion[0].coprocessor="$1"
	uci commit onion
}

main () {
	flashed=$(getFlashedVersion)
	latest=$(getLatestVersion)

	echo "flashed $flashed"
	echo "latest $latest"

	if [ "$flashed" != "$latest" ]; then
		echo "Flashing latest!"
		flashFirmware "$latest"
	fi
}

main

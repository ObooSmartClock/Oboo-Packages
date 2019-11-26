# oboo-clock-fw-manager
Firmware manager for Oboo Clock

## `oboo-sw-upgrade.sh`

The script that is meant to be run to perform all software updates on Oboo Smart Clock.

Notes:

* Requires `onion-script` to easily set timezone
* MCU update is run to reflash microcontroller firmware if required
* Restart of `rpcd` required to make Oboo App ACL file active
* Oboo script is called to enable ubus CORS - required by Oboo App 

## Oboo Provisioning Scripts

Description of Oboo Provisioning scripts

### Get Version Information
#### Usage:
```
http://omega-2a46.local/cgi-bin/ver
```

#### Returns:
JSON object with various version information

### Scan for WiFi Networks
#### Usage: 
```
http://omega-2a46.local/cgi-bin/wifi-scan
```

#### Returns:
JSON array (with key `results`) that holds an object for each detected network

### List Configured WiFi Networks
#### Usage:
```
http://omega-2a46.local/cgi-bin/wifi-setup?cmd=list
```

#### Returns:
A JSON array (with key ‘results’) of configured wifi networks (from `/etc/config/wireless`). Each object holds the ssid, password, and an enabled boolean. If the enabled boolean is true, the device is currently connected to this network.
(Equivalent of calling wifisetup list on command line)

### Connect to a Network
#### Usage:
```
http://omega-2a46.local/cgi-bin/wifi-setup?cmd=add&ssid=T25pb25XaUZp&password=ZGVuaXNvbnN0cmVldG9uaW9ucw%3D%3D&encr=cHNrMg%3D%3D
```

The ssid, password, and encr parameters must be **base64 encoded** and then **url encoded**

Optional parameter: bssid 
Must also be base64 encoded and then url encoded

#### Returns:
A JSON object with a `status` string and `success` boolean.


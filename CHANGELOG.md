# Oboo Clock Base

# 0.0.8

* `0.0.8-5`
  * Reorganized code base
* `0.0.8-4`
  * Software update process now writes a notification to the screen when checking for updates
* `0.0.8-3`
  * Stability fixes for card manager
  * Wifi status icon will appear more often - independent of card manager process
* `0.0.8-2`
  * Software updates now use new updateConfig JS program to generate oboo config file
    * No longer installing config.json
  * Updated wifi hotplug script to denote wifi with the existence of a /tmp file
    * Also added a script that checks iwconfig for wifi connectivity
* `0.0.8-1`
  * Added calendar card enable/disable to config file
  * Update script now attempts to take maker edition models into account

# Oboo Cards

## 0.2.7

* `0.2.7-8`
  * Addition of updateConfig JS program that writes a config file that takes required fields into account but also maintains the existing config file
* `0.2.7-7`
  * Addition of Calendar Card
* `0.2.7-6`
  * When a card is found to be invalid by the Card Manager, the card process will exit 
    * Mechanism: message on `/cardResponse` topic is received that mentions updating an element failed because `cardId` is invalid
    * Weather, Music, and Timer cards updated
* `0.2.7-5`
  * Implemented enabling/disabling cards

# Card Manager

## 0.2.7

* `0.2.7-1`
  * Now supports creation and update of calendar elements

## 0.2.6

* `0.2.6-2`
  * Fix for bug where updating an element on a card that does not exist crashes the program
    * Will now continue running and will publish a message to `/cardResponse` topic alerting the card program

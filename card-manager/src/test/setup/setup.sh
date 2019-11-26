#! /bin/sh

## install required packages
opkg update 
opkg install mosquitto-ssl mosquitto-client-ssl

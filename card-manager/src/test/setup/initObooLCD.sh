#!/bin/sh

## Script to insert fbtft module and init TFT Expansion

## Setup for TFT Expansion
omega2-ctrl gpiomux set i2c gpio

## Insert the kernel module
#insmod fbtft_device name=tm022hdh26 busnum=32766 cs=1 gpios=dc:2 speed=98985984 rotate=270 txbuflen=153600
#insmod fbtft_device name=tm022hdh26 busnum=32766 cs=1 gpios=dc:5 speed=98985984 txbuflen=153600
insmod fbtft_device name=tm022hdh26 busnum=32766 cs=1 gpios=dc:5,reset:4 speed=98985984 txbuflen=153600
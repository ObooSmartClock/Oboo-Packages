#!/bin/sh

## Script to insert fbtft module and init TFT Expansion 

## Setup for TFT Expansion
# setup backlight and cs-demux
gpioctl dirout-high 1
gpioctl dirout-low 3

## Insert the kernel module
#insmod fbtft_device name=tm022hdh26 busnum=32766 cs=1 gpios=dc:2 speed=98985984 rotate=270 txbuflen=153600
insmod fbtft_device name=tm022hdh26 busnum=32766 cs=1 gpios=dc:2 speed=98985984 txbuflen=153600
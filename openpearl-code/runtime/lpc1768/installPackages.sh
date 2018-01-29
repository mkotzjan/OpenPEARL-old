#!/bin/bash

if [ "$EUID" -ne 0 ]
   then echo "Please run as root"
   exit
fi


FILE=gcc-arm-none-eabi-7-2017-q4-major-linux.tar.bz2

wget "https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2017q4/gcc-arm-none-eabi-7-2017-q4-major-linux.tar.bz2?revision=375265d4-e9b5-41c8-bf23-56cbe927e156?product=GNU%20Arm%20Embedded%20Toolchain,64-bit,,Linux,7-2017-q4-major" -O $FILE

#tar xjf $FILE -C /usr/local
rm -f $FILE


#!/bin/sh

# make sure /dev/ttyACM0 is correct with `arduino-cli board list`
# chown /dev/ttyACM0 to your user

arduino-cli compile --fqbn arduino:avr:uno
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno

#echo "UPLOAD DONE. NOW WRITING EEPROM"
#stty -F /dev/ttyACM0 raw 57600
#cat eeprom.data > /dev/ttyACM0
#cat /dev/ttyACM0

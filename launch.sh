#!/bin/bash

# Loop over all ttyUSB devices
for device in /dev/ttyUSB*; do
    # Check if the file actually exists (in case no ttyUSB* is found)
    if [ -e "$device" ]; then
        echo "Changing permissions for $device"
        sudo chmod a+rw "$device"
    fi
done

./bin/metalprofiler

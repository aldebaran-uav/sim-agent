#!/bin/bash

# Read drone count from config.json
DRONE_COUNT=$(jq -r '.uav_count' config.json)

if [ -z "$DRONE_COUNT" ]; then
    echo "Hata: uav_count config.json içinde bulunamadı veya okunamadı."
    exit 1
fi

killall -9 gzserver gzclient px4 &> /dev/null

gnome-terminal -- MicroXRCEAgent udp4 -p 8888 &

if [ -z "$PX4_PATH" ]; then
    echo "Hata: PX4_PATH değişkeni tanımlı değil. Lütfen ~/.bashrc dosyanızda örneğin : export PX4_PATH=~/PX4-Autopilot şeklinde tanımlayın."
    exit 1
fi

gnome-terminal -- $PX4_PATH/Tools/simulation/gazebo-classic/sitl_multiple_run.sh -s "plane:$DRONE_COUNT"

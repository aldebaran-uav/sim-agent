#!/bin/bash

killall -9 gzserver gzclient px4 &> /dev/null

# gnome-terminal -- MicroXRCEAgent udp4 -p 8888 &

if [ -z "$PX4_PATH" ]; then
    echo "Hata: PX4_PATH değişkeni tanımlı değil. Lütfen ~/.bashrc dosyanızda örneğin : export PX4_PATH=~/PX4-Autopilot şeklinde tanımlayın."
    exit 1
fi

gnome-terminal -- $PX4_PATH/Tools/simulation/gazebo-classic/sitl_multiple_run.sh -s plane_cam:1,plane:$(jq -r '.uav_count' "config.json") -w mcmillan_airfield
# gnome-terminal -- $PX4_PATH/Tools/simulation/gazebo-classic/sitl_multiple_run.sh -m plane -n $(jq -r '.uav_count' "config.json") -w mcmillan_airfield

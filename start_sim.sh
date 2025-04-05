#!/bin/bash

# Read drone count from config.json
DRONE_COUNT=$(jq -r '.uav_count' config.json)

if [ -z "$DRONE_COUNT" ]; then
    echo "Hata: uav_count config.json içinde bulunamadı veya okunamadı."
    exit 1
fi

killall -9 gzserver gzclient px4 &> /dev/null

if [ -z "$PX4_PATH" ]; then
    echo "Hata: PX4_PATH değişkeni tanımlı değil. Lütfen ~/.bashrc dosyanızda örneğin : export PX4_PATH=~/PX4-Autopilot şeklinde tanımlayın."
    exit 1
fi

for (( i=0; i<DRONE_COUNT; i++ ))
do
    POSE="$(($i * 4)) $(($i * 4)) 0 0 0 0"
    gnome-terminal -- bash -c "PX4_SYS_AUTOSTART=4003 PX4_SIM_MODEL=gz_rc_cessna PX4_GZ_MODEL_POSE=\"$POSE\" $PX4_PATH/build/px4_sitl_default/bin/px4 -i $i; exec bash"
    sleep 4
done
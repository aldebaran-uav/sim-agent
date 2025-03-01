#!/bin/bash

killall -9 gzserver gzclient px4 &> /dev/null

if [ -z "$PX4_PATH" ]; then
    echo "Hata: PX4_PATH değişkeni tanımlı değil. Lütfen ~/.bashrc dosyanızda örneğin : export PX4_PATH=~/PX4-Autopilot şeklinde tanımlayın."
    exit 1
fi

DRONE_COUNT=5

for (( i=0; i<DRONE_COUNT; i++ ))
do
    gnome-terminal -- bash -c "PX4_SYS_AUTOSTART=4003 PX4_SIM_MODEL=gz_rc_cessna PX4_GZ_MODEL_POSE=\"$((i*2))\" $PX4_PATH/build/px4_sitl_default/bin/px4 -i $i; exec bash"
    sleep 4
done

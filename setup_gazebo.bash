#!/bin/bash

export GAZEBO_PLUGIN_PATH=$GAZEBO_PLUGIN_PATH:${PX4_PATH}/build/build_gazebo-classic
export GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:${PX4_PATH}/Tools/simulation/gazebo-classic/sitl_gazebo-classic/models
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PX4_PATH}/build/build_gazebo-classic

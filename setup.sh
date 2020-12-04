#!/bin/bash
make clean
make
gnome-terminal --window  -- ./elevator_control
#./elevator_control
gnome-terminal --window  -- ./elevator_floor1
gnome-terminal --window  -- ./elevator_floor2
gnome-terminal --window  -- ./elevator_floor3
gnome-terminal --window  -- ./elevator
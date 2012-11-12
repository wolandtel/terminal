#!/bin/bash

display=:0
xauth=/var/lib/gdm/$display.Xauth

DISPLAY=$display XAUTHORITY=$xauth "$(dirname $0)"/terminal

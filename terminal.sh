#!/bin/bash

display=:0
xauth=/var/lib/gdm/$display.Xauth

cd "$(dirname $0)"
DISPLAY=$display XAUTHORITY=$xauth ./terminal 2>> terminal.log

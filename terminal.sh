#!/bin/bash

export DISPLAY=:0
export XAUTHORITY=/var/lib/gdm/$display.Xauth

"$(dirname $0)"/terminal $@

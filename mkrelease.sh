#!/bin/bash

target=$1

[ -z "$target" ] && target=terminal
[[ "$target" == "/" ]] && {
	echo "USAGE:	mkrelease.sh <release dir>"
	exit 1
}
[[ "${target:0:1}" != "/" ]] && target="$(pwd)/$target"
[ -d "$target" ] || mkdir "$target"
rm -rf "$target"/*
cd "$(dirname $0)"
cp -a terminal terminal.conf templates helper "$target"


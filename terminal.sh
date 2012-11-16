#!/bin/bash

wd="$(dirname $0)"

# ----------- CONFIG
	display=:0								#
	xauth=/var/lib/gdm/$display.Xauth		#
	pidfile="$wd/terminal.pid"				#
	logfile="$wd/terminal.log"				#
	terminal="$wd/terminal"					#
# ----------- CONFIG

confirm ()
{
	def=$2
	[[ "${def,Y}" != "y" && "${def,N}" != "n" ]] && def=y 
	
	if [[ "$def" == "y" ]]; then
		dispdef="[Y/n]"
	else
		dispdef="[y/N]"
	fi
	
	echo -n "$1? $dispdef "
	read -N1 answer
	if [ -z "$answer" ]; then
		answer=$def
	else
		echo
	fi
	
	[[ "${answer,Y}" == "y" ]]
	return $?
}

unlink ()
{
	file=$@
	echo -n "Удаляется '$file'... "
	rm "$file" 2>/dev/null && echo "[OK]" || echo "[FAIL]"
}

checklog ()
{
	file=$@
	touch "$file"
	[ -w "$file" ] || {
		echo "Файл журнала не может быть открыт."
		exit 1
	}
}

case "$1" in
	"start")
		[ -r "$pidfile" ] && pid=$(cat "$pidfile")
		running=$pid
		[ -n "$pid" ] && running=$(ps --no-headers --pid $pid -opid)
		[ -n "$running" ] && {
			echo "Терминал запущен."
			exit 1
		}
		rm -f "$pidfile"
	;;
	"status")
		[ -r "$pidfile" ] && pid=`cat "$pidfile"`
		[ -z "$pid" ] && {
			echo Терминал не запущен.
			exit 0
		}
		ps --pid $pid -ouser,uid,pid:9,pcpu,pmem,time,cmd:100
		exit 0
	;;
	"help" | "-h" | "--help")
		echo "USAGE: terminal.sh [status]"
		exit 0
	;;
esac

if [ -r "$pidfile" ]; then
	confirm "Остановить терминал" n || exit 0
	echo -n "Терминал останавливается... "
	kill $(cat "$pidfile") && rm -f "$pidfile" && echo "[OK]" || echo "[FAIL]"
	exit $?
fi

echo -n "Терминал запускается..."

checklog "$logfile"
DISPLAY=$display XAUTHORITY=$xauth "$terminal" 2>> "$logfile" &
echo $! > "$pidfile"
echo " OK"

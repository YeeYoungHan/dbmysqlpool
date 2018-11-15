#!/bin/sh
#------------------------------------------------------------------------------
# MySQLSlaveOverwatcher start/stop script 
#------------------------------------------------------------------------------
# programmer : yee young han ( websearch@naver.com )
#------------------------------------------------------------------------------
# start date : 2018/11/15
#------------------------------------------------------------------------------

# chkconfig: - 95 95
# description: init file for MySQLSlaveOverwatcher daemon

# program directory
root_dir=/home/websearch/OpenProject/DbMySQLPool/MySQLSlaveOverWatcher
program="MySQLSlaveOverwatcher.exe"
program_list="$program"
program_arg="$root_dir/MySQLSlaveOverwatcher.xml"
program_name="MySQLSlaveOverwatcher"
logfile=$root_dir/mysqlslaveoverwatcherdog.log
this_script=mysqlslaveoverwatcher.sh

# get process pid
getpids(){
	pids=`ps -Af | grep "$1" | grep -v grep | sed -e 's/^........ *//' -e 's/ .*//'`
	echo "$pids"
}

# start function
start() {
	# Start daemons.
	if [ -n "`getpids $program`" ]; then
		echo "$program_name: already running"
  else
    echo "$program_name: Starting..."
    ulimit -c unlimited
    $root_dir/$program $program_arg
	fi
}

# stop function
stop() {
	# Stop daemons.
  echo "$program_name : Stopping... "
  killall $program 
}

# status function
status() {
	pids=`getpids $program`
	if [ "$pids" != "" ]; then
		echo "$program_name: running"
  else
		echo "$program_name: not running"
	fi
	
	pids=`getpids "$this_script watchdog"`
	if [ "$pids" != "" ]; then
		echo "watchdog: running"
  else
		echo "watchdog: not running"
	fi
}

start_program(){
	d=`date +%Y/%m/%d_%H:%M:%S`

  echo "[$d] $1: Starting..." >> $logfile
  ulimit -c unlimited
  ulimit -n 65535
  $root_dir/$1 $program_arg
}

watchdog(){
	# wait until process run
	echo "watchdog: Starting..."
	sleep 60
	while [ 1 ]; do
	
		# If process is not found, execute process
		for program in $program_list
		do
			if [ -f "$root_dir/$program" ]; then
				if [ "`getpids $program`" = "" ]; then
			    start_program $program
			  fi
			else
				echo "[error] $root_dir$program is not exist."
			fi
		done
		
		sleep 10
	done
}

# watchdog start
watchdog_start(){
	$root_dir/$this_script watchdog &
}

# watchdog stop
watchdog_stop(){
	pids=`getpids "$this_script watchdog"`
	for p in $pids
	do
		kill $p
	done
	echo "watchdog : Stopped"
}

# main function.
case "$1" in
	start)
		start
		watchdog_start
		;;
	stop)
		watchdog_stop
		stop
		;;
	status)
		status
		;;
	restart)
		stop
		sleep 1
		start
		;;
	watchdog)
		watchdog
		;;
	*)
		echo $"Usage: $0 {start|stop|status|restart}"
		exit 1
		;;
esac


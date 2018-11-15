#!/bin/sh

version=`./MySQLSlaveOverWatcher.exe -v | grep version | awk 'BEGIN {FS="[ ]"}{print $2}'`
version=`echo $version | awk 'BEGIN {FS="[-]"}{print $2}'`
bit=`getconf LONG_BIT`
filename="MySQLSlaveOverWatcher-$version-cent6.3-$bitbit.tar.gz"
tar czvf $filename MySQLSlaveOverWatcher.exe MySQLSlaveOverWatcher.xml mysqlslaveoverwatcher.sh


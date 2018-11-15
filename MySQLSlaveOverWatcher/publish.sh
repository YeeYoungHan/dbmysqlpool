#!/bin/sh

version=`./MySQLSlaveOverWatcher.exe -v | grep version | awk 'BEGIN {FS="[ ]"}{print $2}'`
version=`echo $version | awk 'BEGIN {FS="[-]"}{print $2}'`
bit=`getconf LONG_BIT`
bit+="bit"
filename="MySQLSlaveOverWatcher-$version-cent6.3-$bit.tar.gz"
rm -f $filename
tar czvf $filename MySQLSlaveOverWatcher.exe MySQLSlaveOverWatcher.xml mysqlslaveoverwatcher.sh


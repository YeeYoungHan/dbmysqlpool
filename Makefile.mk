# Makefile for all of C++ mysql db pool
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2017/09/09

CC=g++
CFLAGS=-Wall -O2 -D_REENTRANT -g #-pg

ifeq ($(SYSTEM_BIT),64)
	SYSTEM_LIB=/usr/lib64
endif

# MySQL library path
MYSQL_INC=-I/usr/include/mysql
MYSQL_LIB=-L$(SYSTEM_LIB)/mysql -lmysqlclient


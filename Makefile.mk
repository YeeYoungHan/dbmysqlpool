# Makefile for all of C++ mysql db pool
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2017/09/09

CC=g++
CFLAGS=-Wall -O2 -D_REENTRANT -g #-pg

SYSTEM_BIT=$(shell getconf LONG_BIT)
SYSTEM_LIB=/usr/lib

MYSQL_VERSION=$(shell mysql --version | awk '{ printf("%d", $$3) }')

ifeq ($(SYSTEM_BIT),64)
	SYSTEM_LIB=/usr/lib64
endif

# MySQL library path
MYSQL_INC=-I/usr/include/mysql
MYSQL_LIB=-L$(SYSTEM_LIB)/mysql -lmysqlclient -lssl -lcrypto -lz

ifeq ($(MYSQL_VERSION),14)
	MYSQL_LIB=-L$(SYSTEM_LIB)/mysql -lmysqlclient
endif


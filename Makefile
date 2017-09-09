# Makefile for all of C++ mysql db pool
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2017/03/22

all:
	cd SipPlatform && make
	cd DbMySQLPool && make
	cd TestDbMySQLPool && make

clean:
	cd SipPlatform && make clean
	cd DbMySQLPool && make clean
	cd TestDbMySQLPool && make clean

install:


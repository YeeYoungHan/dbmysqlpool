# Makefile for DbPool
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2015/07/23

include ../Makefile.mk

INC=-I../SipPlatform -I/usr/include/mysql

.SUFFIXES: .o .cpp
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< $(INC)

all: libdbpool.a

LIB_OBJ=DbMySQLConnection.o DbMySQLPool.o \
	DbMySQLQueue.o DbMySQLThread.o DbCheckSqlInjection.o DbSetData.o

libdbpool.a: $(LIB_OBJ)
	$(AR) crv $@ $(LIB_OBJ)

clean:
	rm -f *.a
	rm -f *.o
	rm -f core.*

install:
	cp -f libdbpool.a $(INSTALL_LIB_IDR)
	cp -f *.h $(INSTALL_HEADER_IDR)


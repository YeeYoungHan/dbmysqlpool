# Makefile for TestDbMySQLPool
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2017/09/09

include ../Makefile.mk

INC=-I../DbMySQLPool -I../SipPlatform $(MYSQL_INC)
LIB=-L../DbMySQLPool -ldbpool \
	-L../SipPlatform -lsipplatform \
	$(MYSQL_LIB) \
	-ldl -lpthread

.SUFFIXES: .o .cpp
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< $(INC)

all: TestDbMySQLPool.exe

SRV_OBJ=TestDbMySQLPool.o

TestDbMySQLPool.exe: $(SRV_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SRV_OBJ) $(INC) $(LIB)

clean:
	rm -f *.exe
	rm -f *.o
	rm -f core.*


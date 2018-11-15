# Makefile for MySQLSlaveOverWatcher
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2018/11/15

include ../Makefile.mk

INC=-I../DbMySQLPool -I../SipPlatform -I../ServerPlatform -I../XmlParser \
	$(MYSQL_INC)
LIB=-L../DbMySQLPool -ldbpool \
	-L../SipPlatform -lsipplatform \
	-L../ServerPlatform -lserverplatform \
	-L../XmlParser -lxmlparser \
	$(MYSQL_LIB) \
	-ldl -lpthread

.SUFFIXES: .o .cpp
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< $(INC)

all: MySQLSlaveOverWatcher.exe

SRV_OBJ=MySQLSlaveOverWatcher.o MySQLSlaveOverWatcherSetup.o

MySQLSlaveOverWatcher.exe: $(SRV_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SRV_OBJ) $(INC) $(LIB)

clean:
	rm -f *.exe
	rm -f *.o
	rm -f core.*


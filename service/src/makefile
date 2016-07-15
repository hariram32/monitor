IDIR = ../include
CC = gcc
MYSQL_CFLAGS = -I/usr/include/mysql -DBIG_JOINS=1 -fno-strict-aliasing -g -DNDEBUG
CFLAGS = -I $(IDIR) $(MYSQL_CFLAGS)


ODIR = obj
LDIR = ../lib

MYSQL_LIBS = -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl
LIBS = $(MYSQL_LIBS)

_DEPS = header.h database.h initialise.h network.h process.h file.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = database.o initialise.o main.o network.o process.o file.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

monitor: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
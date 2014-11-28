CC = gcc
CFLAGS = -g -Wall
LFLAGS = 
INCLUDES = -I/
LIBS = -lpthread -lrt
SRC = santa.c lib_set.c lib_sem.c
OBJ = $(SRC:.c=.o)
MAIN = santa

.PHONY: depend clean

all:	$(MAIN)
		@echo	The solution to the Santa Claus Problem has been compiled

$(MAIN): $(OBJ)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LFLAGS) $(LIBS)

.c.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
		$(RM) *.o *~ $(MAIN)

depend: $(SRC)
		makedepend $(INCLUDES) $^
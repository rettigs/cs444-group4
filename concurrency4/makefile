CC = gcc
CFLAGS = -g -Wall
LFLAGS = 
INCLUDES = -I/
LIBS = -lpthread -lrt
SRC = barber.c queue.c
OBJ = $(SRC:.c=.o)
MAIN = barbershop

.PHONY: depend clean

all:	$(MAIN)
		@echo	The solution to the Sleeping Barber Problem has been compiled

$(MAIN): $(OBJ)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LFLAGS) $(LIBS)

.c.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
		$(RM) *.o *~ $(MAIN)

depend: $(SRC)
		makedepend $(INCLUDES) $^

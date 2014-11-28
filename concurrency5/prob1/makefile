CC = gcc
CFLAGS = -g -Wall
LFLAGS = 
INCLUDES = -I/
LIBS = -lpthread -lrt
SRC = prob1.c
OBJ = $(SRC:.c=.o)
MAIN = prob1

.PHONY: depend clean

all:	$(MAIN)
		@echo	The solution to Problem 1 has been compiled

$(MAIN): $(OBJ)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LFLAGS) $(LIBS)

.c.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
		$(RM) *.o *~ $(MAIN)

depend: $(SRC)
		makedepend $(INCLUDES) $^

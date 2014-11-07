CC = gcc
CFLAGS = -g -Wall
LFLAGS = 
INCLUDES = -I/
LIBS = -lpthread -lrt
SRC = list1.c
OBJ = $(SRC:.c=.o)
MAIN = list

.PHONY: depend clean

all:	$(MAIN)
		@echo	The linked-list three-way exclusive threading assignment has been compiled

$(MAIN): $(OBJ)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LFLAGS) $(LIBS)

.c.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
		$(RM) *.o *~ $(MAIN)

depend: $(SRC)
		makedepend $(INCLUDES) $^

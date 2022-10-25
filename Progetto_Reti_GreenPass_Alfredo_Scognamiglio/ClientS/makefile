CC = gcc
CFLAGS = -I.
DEPS = wrapperSysCall.h 
OBJ = clientS.c 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
clientS: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

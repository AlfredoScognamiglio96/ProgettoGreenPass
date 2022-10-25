CC = gcc
CFLAGS = -I.
DEPS = wrapperSysCall.h 
OBJ = clientT.c 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
clientT: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

CC = gcc
CFLAGS = -I.
DEPS = wrapperSysCall.h 
OBJ = serverV.c -lpthread

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
serverV: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

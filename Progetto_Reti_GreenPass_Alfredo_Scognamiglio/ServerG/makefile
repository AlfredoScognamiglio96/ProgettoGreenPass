CC = gcc
CFLAGS = -I.
DEPS = wrapperSysCall.h 
OBJ = serverG.c -lpthread

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
serverG: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

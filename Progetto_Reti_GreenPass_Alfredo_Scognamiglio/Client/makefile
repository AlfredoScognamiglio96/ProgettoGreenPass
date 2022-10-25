CC = gcc
CFLAGS = -I.
DEPS = wrapperSysCall.h 
OBJ = client.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
client: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	

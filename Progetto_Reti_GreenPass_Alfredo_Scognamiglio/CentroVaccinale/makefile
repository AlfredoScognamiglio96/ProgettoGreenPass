CC = gcc
CFLAGS = -I.
DEPS = wrapperSysCall.h 
OBJ = centro_vaccinale.c -lpthread

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
centro_vaccinale: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	

CC = gcc
CFLAGS = -shared -fPIC -g
PRELOAD = ourown.so

all = $(PRELOAD)

$(PRELOAD): main.c
	$(CC) main.c -ldl -o $(PRELOAD) $(CFLAGS)

clean:
	rm -f $(PRELOAD)

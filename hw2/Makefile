CC = gcc
CFLAGS = -shared -fPIC -g
PRELOAD = sandbox.so
LAUNCHER = sandbox

all: $(PRELOAD) $(LAUNCHER)

$(PRELOAD): main.c
	$(CC) main.c -ldl -o $(PRELOAD) $(CFLAGS)

$(LAUNCHER): launcher.c
	$(CC) launcher.c -ldl -o $(LAUNCHER) 

clean:
	rm -f $(PRELOAD) $(LAUNCHER)
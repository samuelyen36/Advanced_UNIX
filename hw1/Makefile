CC:= gcc
OUT:= a.out
C_FILENAME:= main.c
OBJ_FILE:= tcp.o main.o udp.o
C_FLAG:= -Wall

all: $(OBJ_FILE)
	$(CC) -o $(OUT) $(OBJ_FILE) $(C_FLAG)

main.o: main.c
	$(CC) -o main.o main.c -c

tcp.o: tcp.h tcp.c
	$(CC) -o tcp.o tcp.c -c

udp.o: udp.h udp.c
	$(CC) -o udp.o udp.c -c

clean: 
	rm -f a.out tcp.o main.o
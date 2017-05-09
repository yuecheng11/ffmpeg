all:main
CC=gcc
CLIBSFLAGS=-l avutil -l avcodec -l avformat
main:main.o
	$(CC) -o main main.o $(CLIBSFLAGS)
main.o:main.c
	$(CC) -o main -c main.c $(CLIBSFLAGS)
clean:
	rm main main.o

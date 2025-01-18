CFLAGS= -O2 # -DBIG_ENDIAN
# Uncomment -DBIG_ENDIAN for big-endian processors (m68k etc)
CC=gcc

all: sod32 forth.img glossary.txt ../applications/forth/image_data.h

sod32: engine.o special.o term.o main.o 
	$(CC) -o sod32 engine.o special.o term.o main.o

engine.o: engine.c sod32.h

special.o: special.c sod32.h

main.o: main.c sod32.h

forth.img: extend.4th extend_g.4th kernel.img 
	echo 'S" extend.4th" INCLUDED '|./sod32 kernel.img 

forth_a.img: extend_a.4th extend_g.4th kernel.img 
	echo 'S" extend_a.4th" INCLUDED '|./sod32 kernel.img 

kernel.img: kernel.4th cross.4th
	echo 'S" cross.4th" INCLUDED '|./sod32 forth.img

glossary.txt: kernel.4th extend_a.4th extend_g.4th doglos.4th
	./sod32 forth.img < doglos.4th

../applications/forth/image_data.h: forth_a.img
	python3 bin2c.py forth_a.img >../applications/forth/image_data.h

clean:
	rm sod32 *.o forth.img glossary.txt

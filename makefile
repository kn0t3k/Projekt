#--------------------------------------
# Projekt: IFJ14						
# Soubor: makefile						
# Autori: Denk Filip (xdenkf00)		
#	  Jurica Tomas (xjuric22)		
#	  Knotek Martin (xknote11)	
#	  Kohut Jan (xkohut08)		
#	  Piskula David (xpisku02)	
# Prekladac: GCC 4.8.2-7				
# Datum:   5.12.2014					
#----------------------------------------

CFLAGS=-std=c99 -Wall -pedantic -g
BIN=project
CC=gcc
RM=rm -f

all: projekt ial.o str.o main.o parser.o scaner.o 

#-----------jednotlive moduly--------------------------------------------
ial.o: ial.c ial.h
	$(CC) $(CFLAGS) -c -o ial.o ial.c

str.o: str.c str.h
	$(CC) $(CFLAGS) -c -o str.o str.c

parser.o: parser.c parser.h scaner.c scaner.h str.c str.h 
	$(CC) $(CFLAGS) -c -o parser.o parser.c

scaner.o: scaner.c scaner.h str.c str.h 
	$(CC) $(CFLAGS) -c -o scaner.o scaner.c

main.o: str.c str.h parser.c parser.h scaner.c scaner.h main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

#-----------prelozeni projektu z jednotlivych .o modulu--------------------------------------------	

projekt: str.o ial.o parser.o scaner.o main.o
	$(CC) $(CFLAGS) -o $(BIN) str.o ial.o scaner.o parser.o main.o


#-----------smazani souboru--------------------------------------------	
clean:
	$(RM) *.o $(BIN)

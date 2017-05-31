all: frogger

frogger: main.o frogger.o player.o log.o logrow.o loglist.o gameglobals.o console.o
	clang -Wall -g -lcurses -pthread -o frogger main.o frogger.o player.o log.o logrow.o loglist.o gameglobals.o console.o

main.o: main.c
	clang -Wall -g -c main.c

frogger.o: frogger.c
	clang -Wall -g -c frogger.c

player.o: player.c
	clang -Wall -g -c player.c

log.o: log.c
	clang -Wall -g -c log.c
	
logrow.o: logrow.c
	clang -Wall -g -c logrow.c
	
loglist.o: loglist.c
	clang -Wall -g -c loglist.c

gameglobals.o: gameglobals.c
	clang -Wall -g -c gameglobals.c
	
console.o: console.c
	clang -Wall -g -c console.c
	
clean:
	rm -rf *o frogger
all:
	gcc -Wall -std=c99 -ISDL2/include -LSDL2/lib ./src/*.c -o renderer -lmingw32 -lSDL2main -lSDL2 -mwindows

run:
	./renderer

clean:
	rm renderer
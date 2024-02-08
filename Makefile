build:
	gcc -Wall -std=c99 -ISDL2/include -LSDL2/lib ./src/*.c -lmingw32 -lSDL2main -lSDL2 -lm -mwindows -o renderer

run:
	./renderer

clean:
	rm renderer
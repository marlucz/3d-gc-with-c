build:
	gcc -Wall -std=c99 -lm -ISDL2/include -LSDL2/lib ./src/*.c -lmingw32 -lSDL2main -lSDL2  -mwindows -o renderer

run:
	./renderer

clean:
	rm renderer
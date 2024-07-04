all:
	g++ -Isrc/include -Lsrc/lib -o clac clac.cpp RC.o -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -static-libgcc
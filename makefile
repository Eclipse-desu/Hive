main: makefile main.cpp Hive.o
	g++ Hive.o main.cpp -o main.exe -fopenmp -Wall -Wextra -static -std=c++20 -lgraphics -lgdi32 -lgdiplus

Hive.o: makefile Hive.cpp Hive.h
	g++ -c Hive.cpp -o Hive.o -fopenmp -Wall -Wextra -static -std=c++20 -lgraphics -lgdi32 -lgdiplus

test2: makefile test2.cpp
	g++ test2.cpp -o test2.exe -lgraphics -lgdi32 -lgdiplus

clean:
	del *.o
	del *.exe
executor.out : main.o logger.o compiler.o monitor.o
	g++ --std=c++0x -O2 -o executor.out main.o logger.o compiler.o monitor.o

main.o : main.cpp
	g++ --std=c++0x -O2 -c main.cpp

logger.o : logger.cpp logger.h
	g++ --std=c++0x -O2 -c logger.cpp

compiler.o : compiler.cpp compiler.h
	g++ --std=c++0x -O2 -c compiler.cpp

monitor.o : monitor.cpp monitor.h
	g++ --std=c++0x -O2 -c monitor.cpp

clean :
	rm executor main.o logger.o compiler.o monitor.o

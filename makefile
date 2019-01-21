myftps: myftps.o func.o
		gcc -o myftps myftps.o func.o
myftps.o: myftps.c
		gcc -c myftps.c
myftps.o: myftps.h

myftpc: myftpc.o func.o
		gcc -o myftpc myftpc.o func.o
myftpc.o: myftpc.c
		gcc -c myftpc.c
func.o: func.c
		gcc -c func.c
func.o: myftpc.h
myftpc.o: myftpc.h

clean: 
		rm *.o myftps myftpc

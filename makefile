OBJECTS = lab2.o main.o

validate_sudoku: $(OBJECTS)
	gcc -o $@ $(OBJECTS) 

lab2.o:lab2.c
	gcc -c -pthread lab2.c

main.o: 
	gcc -c main.c

clean:
	-rm *.o core validate_sudoku

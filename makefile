obj = face.o action.o main.o ai.o

bm : $(obj)	
	gcc -o bm $(obj) -lcurses -lpthread -g
face.o : face.c face.h action.h
	gcc -c -Wall face.c
ai.o : ai.c face.h action.h
	gcc -c -Wall ai.c
main.o : main.c face.h action.h
	gcc -c -Wall main.c
action.o : action.c action.h face.h
	gcc -c -Wall action.c
.PHONY : clean
clean : 
	rm bm $(obj)


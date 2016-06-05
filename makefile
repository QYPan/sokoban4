obj = face.o action.o main.o ai.o kill.o

bm : $(obj)	
	gcc -pg -o bm $(obj) -lcurses -lpthread -g
face.o : face.c face.h action.h
	gcc -pg -c -Wall face.c -g
ai.o : ai.c face.h action.h
	gcc -pg -c -Wall ai.c -g
kill.o : kill.c ai.h action.h
	gcc -pg -c -Wall kill.c -g
main.o : main.c face.h action.h
	gcc -pg -c -Wall main.c -g
action.o : action.c action.h face.h
	gcc -pg -c -Wall action.c -g
.PHONY : clean
clean : 
	rm bm gmon.out $(obj)


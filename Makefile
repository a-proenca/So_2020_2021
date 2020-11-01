all: arbitro 


arbitro:
	gcc -c arbitro.c
	gcc structs.h arbitro.c -o arbitro
	
clean: cleana

cleana:
	rm arbitro
	rm arbitro.o 
	


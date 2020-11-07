all: arbitro cliente


arbitro:
	gcc -c arbitro.c
	gcc structs.h arbitro.c -o arbitro
	
cliente:
	gcc -c cliente.c
	gcc structs.h cliente.c -o cliente	

clean: cleana cleanc

cleana:
	rm arbitro
	rm arbitro.o 
	
cleanc:
	rm cliente
	rm cliente.o 


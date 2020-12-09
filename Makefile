all: arbitro cliente jogo


arbitro:
	gcc -c arbitro.c
	gcc structs.h arbitro.c -o arbitro -lpthread
	
cliente:
	gcc -c cliente.c
	gcc structs.h cliente.c -o cliente	

jogo:
	gcc -c G_004.c
	gcc  G_004.c -o G_004	

clean: cleana cleanc cleanj clean_pipes

cleana:
	rm arbitro
	rm arbitro.o 
	
cleanc:
	rm cliente
	rm cliente.o 
	
cleanj:
	rm G_004.o

clean_pipes:
	find. -maxdepth 1 -type p -delete
																			
install:
	cp G_004 Jogo
	rm G_004

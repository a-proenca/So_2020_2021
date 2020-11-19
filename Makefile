all: arbitro cliente jogo


arbitro:
	gcc -c arbitro.c
	gcc structs.h arbitro.c -o arbitro
	
cliente:
	gcc -c cliente.c
	gcc structs.h cliente.c -o cliente	

jogo:
	gcc -c G_004.c
	gcc  G_004.c -o jogo	

clean: cleana cleanc cleanj

cleana:
	rm arbitro
	rm arbitro.o 
	
cleanc:
	rm cliente
	rm cliente.o 
	
cleanj:
	rm jogo
	rm G_004.o
																			
install:
	cp jogo Jogo
	rm jogo

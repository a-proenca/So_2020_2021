all: arbitro cliente jogo


arbitro:
	gcc -c arbitro.c
	gcc structs.h arbitro.c -o arbitro
	
cliente:
	gcc -c cliente.c
	gcc structs.h cliente.c -o cliente	

jogo:
	gcc -c $(GAMEDIR)/G_004.c
	gcc  $(GAMEDIR)/G_004.c -o $(GAMEDIR)/jogo	

clean: cleana cleanc cleanj

cleana:
	rm arbitro
	rm arbitro.o 
	
cleanc:
	rm cliente
	rm cliente.o 
	
cleanj:
	rm $(GAMEDIR)/jogo
	rm G_004.o
																			


all: arbitro cliente jogo


arbitro:
	gcc -c arbitro.c
	gcc structs.h arbitro.c -o arbitro -lpthread
	
cliente:
	gcc -c cliente.c
	gcc structs.h cliente.c -o cliente -lpthread

jogo:
	gcc -c G_004.c
	gcc  G_004.c -o G_004	

clean: cleana cleanc cleanj

cleana:
	rm arbitro
	rm arbitro.o 
	
cleanc:
	rm cliente
	rm cliente.o 
	
cleanj:
	rm G_004.o
																	
install:
	cp G_004 Jogo
	rm G_004

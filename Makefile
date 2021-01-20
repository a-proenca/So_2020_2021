all: arbitro cliente jogo


arbitro:
	gcc -c arbitro.c
	gcc structs.h arbitro.c -o arbitro -lpthread
	
cliente:
	gcc -c cliente.c
	gcc structs.h cliente.c -o cliente -lpthread

jogo:
	/bin/ls Jogo > jogos.txt
	gcc -c G_004.c
	gcc  G_004.c -o G_004	
	gcc -c G_005.c
	gcc  G_005.c -o G_005

clean: cleana cleanc cleanj

cleana:
	rm arbitro
	rm arbitro.o 
	
cleanc:
	rm cliente
	rm cliente.o 
	
cleanj:
	rm G_004.o
	rm G_005.0
	rm jogos.txt
																	
install:
	cp G_004 Jogo
	cp G_005 Jogo
	rm G_004
	rm G_005

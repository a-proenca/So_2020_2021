all: arbitro cliente jogo install


arbitro:
	gcc structs.h arbitro.c -o arbitro -lpthread
	
cliente:
	gcc structs.h cliente.c -o cliente -lpthread

jogo:
	/bin/ls Jogo > jogos.txt
	gcc  G_004.c -o G_004	
	gcc  G_005.c -o G_005


clean: cleana cleanc cleanj


cleana:
	rm arbitro
	
cleanc:
	rm cliente
	
cleanj:
	rm jogos.txt
																	
install:
	cp G_004 Jogo
	cp G_005 Jogo
	rm G_004
	rm G_005

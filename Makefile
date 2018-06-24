create: server.c client.c
	gcc -o server server.c begasep_common.c -I. -Wall -g
	gcc -o client client.c begasep_common.c -I. -Wall -g

clean:	
	rm server client a.out

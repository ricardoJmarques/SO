all:sobusrv sobucli

sobusrv:
	gcc -Wall --ansi -pedantic -g -o sobusrv servidor.c

sobucli:
	gcc -Wall --ansi -pedantic -g -o sobucli cliente.c

clean:
	rm -f sobucli sobusrv *.o .a *~ Makefile.bak $(OUT)


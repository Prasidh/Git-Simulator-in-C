all: WTF WTFserver

WTFserver: WTFserver.c
	gcc -lpthread -o WTFserver WTFserver.c

WTF: WTF.c
	gcc -o WTF WTF.c -lcrypto

clean:
	rm -rf WTF WTFserver

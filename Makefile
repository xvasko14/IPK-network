# IPK 2
# Michal Vasko
# xvasko14

CC = gcc 
CFLAGS = -std=gnu99 -pedantic -Wall -pthread 

all: chat_client

ftrestd: chat_client.c
	$(CC) $(CFLAGS) chat_client.c -o chat_client 
clean:
	rm -f *.o chat_client



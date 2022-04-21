/*
** delivery_boy.c -- a fork of talker, datagram "client" demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT "4950"	// порт, на который будут отправляться данные

int main(int argc, char *argv[])
{
	int sockfd;//дескриптор сокета
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	if (argc != 3) {
		fprintf(stderr,"delivery_boy can transfer small files over the network\nUsage: ./delivery_boy.out hostname path_to_file\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; // установим AF_INET для использования IPv4
	hints.ai_socktype = SOCK_DGRAM;//указываем тип протокола передачи - UDP, так как дейтаграммы

	if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
		//getaddrinfo(имя или адрес хоста, порт для подключения, ссылка на структуру, описывающую параметры адреса - семейство протоколов и тип - SOCK_STREAM для TCP или SOCK_DGRAM для UDP)
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("delivery_boy: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "delivery_boy: failed to create socket\n");
		return 2;
	}


//deserializing file
FILE *fileptr;
char *buffer;
long filelen;

fileptr = fopen(argv[2], "rb");  // Open the file in binary mode
fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
filelen = ftell(fileptr);             // Get the current byte offset in the file
rewind(fileptr);                      // Jump back to the beginning of the file

buffer = (char *)malloc(filelen * sizeof(char)); // Enough memory for the file
fread(buffer, filelen, 1, fileptr); // Read in the entire file
fclose(fileptr); // Close the file

  //sending data

	if ((numbytes = sendto(sockfd, buffer, filelen, 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("delivery_boy: sendto");
		exit(1);
	}

	freeaddrinfo(servinfo);

	printf("delivery_boy: sent %d bytes to %s\n", numbytes, argv[1]);
	close(sockfd);

	return 0;
}

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "myftpc.h"


int main(int argc, char *argv[]) {
	int count, datalen;
    struct proctable *pt;


	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

    skt.sin_family = AF_INET;
	skt.sin_port = htons(PORT);
	skt.sin_addr.s_addr = inet_addr(argv[1]);

	if (connect(sd, (struct sockaddr *)&skt, sktlen)) {
		perror("connect");
		return (-1);
	}

	send_file(sd, "test.txt", "transfer.txt");

	//close(sd);

	return 0;
}

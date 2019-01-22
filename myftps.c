#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "myftps.h"

int main (int argc, char *argv[]) {
    struct sockaddr_in sskt;
    struct in_addr addr;
    struct proctable *pt;

    //ソケット構造体の初期化
    memset(&sskt, 0, sizeof(sskt));
    sskt.sin_family = AF_INET;
    sskt.sin_port = htons(PORT);
    sskt.sin_addr.s_addr = htonl(INADDR_ANY);

    //ソケット記述子
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return (-1);
    }

    //bind
    if (bind(sd, (struct sockaddr *)&sskt, sizeof(sskt)) == -1) {
        perror("bind");
        return (-1);
    }

	
	//listen
	if (listen(sd, 5) < 0) {
		perror("listen");
		return (-1);
	}
	
	//accept
	if ((sd2 = accept(sd, (struct sockaddr *)&skt, &sktlen)) < 0) {
		perror("accept");
		return (-1);
	}
	
	memset(&data, 0, sizeof data);
	recv(sd2, (struct myftph_data*)&data, sizeof data, 0);
	if (data.type == 0x06) {
		recv_file(sd2, data.data);
	} else {
		printf("errro\n");
	}

    close(sd);

    return 0;
}

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
	 while (1) {
		int sd3;
		if ((sd3 = accept(sd, (struct sockaddr *)&skt, &sktlen)) < 0) {
			perror("accept");
			return (-1);
		}
		if (fork() != 0) {
			continue;
		} else {
			sd2 = sd3;
			break;
		}
	}

//	if ((sd2 = accept(sd, (struct sockaddr *)&skt, &sktlen)) < 0) {
//		perror("accept");
//		return (-1);
//	}
	
	while (1) { 
		memset(&data, 0, sizeof data);
		memset(&header, 0, sizeof header);
		recv(sd2, (struct myftph*)&header, sizeof header, 0);
		switch (header.type) {
			case 0x01:  //QUIT
				set_myftph(&header, 0x10, 0x00, 0);
				send(sd2, (struct myftph*)&header, sizeof header, 0);
				break;
			case 0x02:  //PWD
				if (fork() == 0) {
					FILE	*fp;
					char out[256];
					char	*cmdline = "/bin/pwd";
					if ( (fp=popen(cmdline,"r")) ==NULL) {
						perror("popen");
					}
					while(fgets(out, 256, fp) != NULL) {
						(void) fputs(buf, stdout);
					}
					(void) pclose(fp);
					out[strlen(out)-1] = '\0';
					set_myftph_data(&data, 0x10, 0, strlen(out), out);
					send(sd2, (struct myftph_data*)&data, sizeof data, 0);
				} 
				break;
			case 0x03:  //CWD
				recv(sd2, (char *)buf, header.length, 0);
				printf("chdir\n");
				chdir(buf);
				break;
			case 0x04:  //LIST
				break;
			case 0x05:  //RETR
				recv(sd2, (char *)buf, header.length, 0);
				send_file_r(sd2, buf, buf); 
				break;
			case 0x06:  //STOR
				recv(sd2, (char *)buf, header.length, 0);
				recv_file(sd2, buf);
				break;
			default:   //Error
				set_myftph(&header, 0x11, 0x02, 0);
				send(sd2, (struct myftp*)&header, sizeof header, 0);
		}
	}
    close(sd);
	close(sd2);

    return 0;
}

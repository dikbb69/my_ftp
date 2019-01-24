#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "myftpc.h"


int main(int argc, char *argv[]) {
	int count, datalen, n;
    struct proctable *pt;
	char cmd[128], token[3][64];


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

	while (1) {
		char *args[4];
		printf("myFTP%% ");
		fgets(cmd, 128, stdin);
		cmd[strlen(cmd)-1] = '\0';
		if ((n = tokenize(cmd, token)) < 0) {
			printf("Invalid command\n");
		} else if (n == 0) {
			printf("\n");
			continue;
		} else {
			int i;
			for (i = 0; i < 4; i++) args[i] = NULL;
			for (i = 0; i < n; i++) {
				printf("token[%d]: %s\n", i, token[i]);
				args[i] = token[i];	
			}
			if (!strcmp(token[0], "quit")) {
				if (n > 1) {
					printf("Syntax Error: quit\n");
				} else {
					set_myftph(&header, 0x01, 0, 0);
					send(sd,(struct myftph*)&header, sizeof header, 0);
					recv(sd, (struct myftph*)&header, sizeof header, 0);
					if (header.type == 0x10) {
						//close(sd);
						exit(0);
					} else {
						printf("close error\n");
						exit(1);
					}
				}
			} else if (!strcmp(token[0], "pwd")) {
				if (n > 1) {
					printf("Syntax Error: pwd\n");
				} else {
					set_myftph(&header, 0x02, 0, 0);
					send(sd, (struct myftph*)&header, sizeof header, 0);
					recv(sd, (struct myfhph_data*)&data, sizeof data, 0);
					if (data.type != 0x10) {
						printf("Error\n");
					} else {
						printf("%s\n", data.data);
					}
				}
			} else if (!strcmp(token[0], "cd")) {
				if (n != 2) {
					printf("Syntax Error: cd path\n");
				} else {
					set_myftph_data(&data, 0x03, 0, strlen(token[1]), token[1]);
					send(sd, (struct myftph_data*)&data, sizeof header + strlen(token[1]), 0);
				}
			} else if (!strcmp(token[0], "dir")) {
				if (n == 3) {
					printf("Syntax Error: dir [path]\n");
				} else {
				}
			} else if (!strcmp(token[0], "lpwd")) {
				if (n > 1) {
					printf("Syntax Error: lpwd\n");
				} else {
					if (fork() == 0) {
					    execvp("/bin/pwd", args);
					} else {
						usleep(5000);
					}
				}
			} else if (!strcmp(token[0], "lcd")) {
				if (n != 2) {
					printf("Syntax Error: lcd path\n");
				} else {
					if (chdir(token[1]) < 0) 
						perror("lcd");
					continue;
				}
			} else if (!strcmp(token[0], "ldir")) {
				if (n == 3) {
					printf("Syntax Error: ldir [path]\n");
				} else {
				}
			} else if (!strcmp(token[0], "get")) {
				if (n == 1) {
					printf("Syntax Error: get path(src) [path(dst)]\n");
				} else {
					memset(&data, 0, sizeof data);
					set_myftph_data(&data, 0x05, 0, strlen(token[1]), token[1]);
					send(sd, (struct myftph_data*)&data, sizeof header + strlen(token[1]), 0);
					if (n == 2) {
						recv_file_r(sd, token[1]);
					} else {
						recv_file_r(sd, token[2]);
					}
				}
			} else if (!strcmp(token[0], "put")) {
				if (n == 1) {
					printf("Syntax Error: put path(src) [path(dst)]\n");
				} else {
					if (n == 2) 
						send_file(sd, token[1], token[1]);
					else 
						send_file(sd, token[1], token[2]);
				}
			} else if (!strcmp(token[0], "help")) {
				if (n > 1) {
					printf("Syntax Error: help\n");
				} else {
					
				}
			}
		}
	}


	//close(sd);

	return 0;
}

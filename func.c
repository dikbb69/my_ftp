#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "myftpc.h"


void set_myftph_data(struct myftph_data *p, uint8_t type, uint8_t code, uint16_t length, char *data) {
	p->type = type;
	p->code = code;
	p->length = length;
	memcpy(p->data, data, length);
}
void set_myftph(struct myftph* p, uint8_t type, uint8_t code, uint16_t length){
	p->type = type;
	p->code = code;
	p->length = length;
}

int tokenize(char *buff, char token[][64]) {
    char *ptr;
    ptr = buff;
    int count = 0, len = 0;

    do {
        if (*ptr != ' ' && *ptr != '\t') {
			if (count > 2) return -1;
            token[count][len++] = *ptr;
        } else if (len > 0) {
			if (count > 2) return -1;
            token[count][len] = '\0';
            count++;
            len = 0;
        }
    } while (*++ptr != '\0');
    token[count][len] = '\0';
    if (len == 0) count--;
    return count + 1;
}

int send_file(int sd, char *file, char *path) {
	int    fd;
	char    buff[1024];
	int    rc;

	fd = open(file, O_RDONLY);
	if (fd == -1) {
		perror("open");
		return -1;
	}
	memset(&header, 0, sizeof header);
	set_myftph_data(&data, 0x06, 0, strlen(path), path);
	send(sd, (struct myftph_data*)&data, sizeof header + strlen(path), 0);
	printf("send 0x06\n");

	if (recv(sd, (struct myftph*)&header, sizeof header, 0) < 0) {
		perror("recv");
	}
	printf("recv type:%d code:%d length:%d\n", header.type, header.code, header.length);
	if (header.type != 0x10) {
		//error process
		printf("error\n");
		return -1;
	} else {
		while (1) {
			memset(buff, 0, sizeof buff);
			memset(&data, 0, sizeof data);
			rc = read(fd, buff, 1024);
			if (rc == -1) {
				perror("read");
				close(fd);
				return -1;
			} else if (rc == 0) {
				set_myftph_data(&data, 0x20, 0x00, strlen(buff), buff);
				send(sd, (struct myftph_data*)&data,  sizeof header + strlen(buff), 0);
				break;
			} else {
				printf("read:%d - %s\n", rc, buff);
				set_myftph_data(&data, 0x20, 0x01, rc, buff);
				send(sd, (struct myftph_data *)&data, sizeof header + rc, 0);
			}
		}
	}
	//close(fd);
	
	return 1;
}

int recv_file(int sd, char *path) {
	char buf[1024];
	int fd;

	memset(&header, 0, sizeof header);
	memset(&buf, 0, sizeof buf);

	fd = open(path, O_WRONLY|O_CREAT, 0644);
	if (fd == -1) {
		perror("open");
		set_myftph(&header, 0x12, 0x00, 0);
		send(sd, (struct myftph*)&header, sizeof header, 0);
		return -1;
	}

	set_myftph(&header, 0x10, 0x02, 0);
	if (send(sd, (struct myftph*)&header, sizeof header, 0) < 0) {
		perror("send");
	}

	do {
		memset(&header, 0, sizeof header);
		recv(sd2, (struct myftph *)&header, sizeof header, 0);
		if (header.code == 0x00) {
			printf("file recv\n");
			close(fd);
		    return 0;
		}
		memset(buf, 0, sizeof buf);
		recv(sd2, (char *)buf, header.length, 0);
		if (write(fd, buf, header.length) < 0) {
			perror("write");
			return -1;
		}
		printf("type: %d, code: %d, length: %d, \n"
		"data: %s\n", header.type, header.code, header.length, buf);
	} while (header.code == 0x01);
		 
	
	//close(fd);

	return 0;
}

int send_file_r(int sd, char *file, char *path) {
	int    fd;
	char    buff[1024];
	int    rc;

	fd = open(file, O_RDONLY);
	if (fd == -1) {
		perror("open");
		return -1;
	}
	set_myftph(&header, 0x10, 0x01, 0);
	send(sd2, (struct myftph*)&header, sizeof header, 0);

	memset(&header, 0, sizeof header);

	if (recv(sd, (struct myftph*)&header, sizeof header, 0) < 0) {
		perror("recv");
	}
	printf("recv type:%d code:%d length:%d\n", header.type, header.code, header.length);
	if (header.type != 0x10) {
		//error process
		printf("error\n");
		return -1;
	} else {
		while (1) {
			memset(buff, 0, sizeof buff);
			memset(&data, 0, sizeof data);
			rc = read(fd, buff, 1024);
			if (rc == -1) {
				perror("read");
				close(fd);
				return -1;
			} else if (rc == 0) {
				set_myftph_data(&data, 0x20, 0x00, strlen(buff), buff);
				send(sd, (struct myftph_data*)&data,  sizeof header + strlen(buff), 0);
				break;
			} else {
				printf("read:%d - %s\n", rc, buff);
				set_myftph_data(&data, 0x20, 0x01, rc, buff);
				send(sd, (struct myftph_data *)&data, sizeof data, 0);
			}
		}
	}
	//close(fd);
	
	return 1;
}

int recv_file_r(int sd, char *path) {
	char buf[1024];
	int fd;

	memset(&header, 0, sizeof header);
	memset(&buf, 0, sizeof buf);

	recv(sd, &header, sizeof header, 0);

	fd = open(path, O_WRONLY|O_CREAT, 0644);
	if (fd == -1) {
		perror("open");
		set_myftph(&header, 0x12, 0x00, 0);
		send(sd, (struct myftph*)&header, sizeof header, 0);
		return -1;
	}


	do {
		memset(&header, 0, sizeof header);
		recv(sd2, (struct myftph *)&header, sizeof header, 0);
		if (header.code == 0x00) {
			printf("file recv\n");
			close(fd);
		    return 0;
		}
		memset(buf, 0, sizeof buf);
		recv(sd2, (char *)buf, header.length, 0);
		if (write(fd, buf, header.length) < 0) {
			perror("write");
			return -1;
		}
		printf("type: %d, code: %d, length: %d, \n"
		"data: %s\n", header.type, header.code, header.length, buf);
	} while (header.code == 0x01);
		 
	
	//close(fd);

	return 0;
}
void ppkt_data() {
	printf("type: %d, code: %d, length: %d\n", data.type, data.code, data.length);
}

void ppkt() {
	printf("type: %d, code: %d, length: %d\n", header.type, header.code, header.length);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

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

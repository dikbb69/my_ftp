#define DATASIZE 1024
#define PORT 51230 

struct myftph_data {
    uint8_t  type;
    uint8_t  code;
    uint16_t length;
    char     data[DATASIZE];
};

struct myftph {
    uint8_t   type;
    uint8_t   code;
    uint16_t  length;
};

struct myftph_data data;
struct myftph header;
struct sockaddr_in skt;
socklen_t sktlen = sizeof skt;

int sd, sd2;

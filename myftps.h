#define DATASIZE 1024
#define PORT 51222 

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
static socklen_t sktlen = sizeof skt;

extern void set_myftph_data(struct myftph_data*, uint8_t, uint8_t, uint16_t, char*);
extern void set_myftph(struct myftph*, uint8_t, uint8_t, uint16_t);
extern int send_file(int sd, char *, char *); 
extern int recv_file(int sd,  char *path);
extern int send_file_r(int sd, char *, char *); 
extern int recv_file_r(int sd, char *path);

int sd, sd2;
char buf[1024], file[64];

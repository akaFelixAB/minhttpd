#define AF_INET 2
#define SOCK_STREAM 1
#define IPPROTO_TCP 6

#define MAX_PENDING_CONNECTION 10

#define stdout 1
#define stderr 2

typedef unsigned short uint16_t;

/* Must be padded to at least 16 bytes */
typedef struct {
    uint16_t sin_family; /* 2  */
    uint16_t sin_port;   /* 4  Big endian */
    int sin_addr;        /* 8  */
    char sin_zero[8];    /* 16 */
} sockaddr_in_t;

int read(int fildes, const void* buf, unsigned long buf_len);
int write(int fildes, const void* buf, unsigned long buf_len);
int socket(int domain, int type, int protocol);
int bind(int socket, void* address, unsigned long address_len);
int listen(int socket, int backlog);
int accept(int socket, void* address, unsigned long* address_len);
int close(int fildes);

int exit(int status);

unsigned long strlen(const char* str) {
    const char* ptr;
    for (ptr = str; *ptr; ptr++);
    return ptr - str;
}

uint16_t swap_uint16(uint16_t x) {  // Turn into big endian
    return (((x << 8) & 0xFF00) | ((x >> 8) & 0x00FF));
}

#define print(s) write(stdout, s, strlen(s))

#define println(s) write(stdout, s "\n", strlen(s "\n"))

#define eprintln(s) write(stderr, s "\n", strlen(s "\n"))

#define die(s)             \
    eprintln("FATAL: " s); \
    exit(1)

#define perror(s) eprintln("FATAL: " s)

int tcp_listen(int port) {
    static sockaddr_in_t addr;

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        die("socket");
    }
    /* Address is zero = any address */
    addr.sin_family = AF_INET;
    addr.sin_port = swap_uint16(port);
    if (bind(sock, &addr, sizeof(addr)) < 0) {
        die("bind");
    }
    if (listen(sock, MAX_PENDING_CONNECTION) < 0) {
        die("listen");
    }
    return sock;
}

int main(int argc, char* argv[]) {
    int sock = tcp_listen(8080);
    while (1) {
        static char* buf[8192];
        int n;
        int clientfd = accept(sock, 0, 0);
        if (clientfd < 0) {
            perror("accept");
            continue;
        }
        /* TODO: fork */
        while (1) {
            n = read(clientfd, buf, sizeof(buf));
            if (n > 0) {
                print("received: ");
                write(stdout, buf, n);
            } else {
                if (n < 0) perror("read");
                break;
            }
        }
        close(clientfd);
    }
    return 0;
}

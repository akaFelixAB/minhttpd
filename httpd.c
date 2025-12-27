#define AF_INET 2
#define SOCK_STREAM 1
#define IPPROTO_TCP 6
#define SO_REUSEADDR 2
#define SOL_SOCKET 1
#define SHUT_RDWR 2
#define O_RDONLY 0

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
int setsockopt(int socket, int level, int option_name, const void* option_value,
               unsigned long option_len);
int shutdown(int socket, int how);

int open(const char* path, int flags);
int fork();
void exit(int status) __attribute__((noreturn));

/* forked process are copy-on-write 
 * so there shouldn't be any data race issues
 */
char http_buf[8192];

unsigned long strlen(const char* str) {
    const char* ptr;
    for (ptr = str; *ptr; ptr++);
    return ptr - str;
}

uint16_t swap_uint16(uint16_t x) {  // Turn into big endian
    return (((x << 8) & 0xFF00) | ((x >> 8) & 0x00FF));
}

#define fprint(fd, s) write(fd, s, strlen(s))

#define fprintln(fd, s) fprint(fd, s "\n")

#define print(s) fprint(stdout, s)

#define println(s) fprintln(stdout, s)

#define eprintln(s) fprintln(stderr, s)

#define die(s)             \
    eprintln("FATAL: " s); \
    exit(1)

#define perror(s) eprintln("ERROR: " s)

int tcp_listen(int port) {
    static int yes = 1;
    static sockaddr_in_t addr;

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        die("socket");
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
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

/*Consume all data from client
 * HTTP request ends with an empty line
 * line saperated by \r\n, but might handle just \n
 */

int isspace(char c) {
    switch (c) {
        case '\r':
        case ' ':
        case '\t':
            return 1;
    }
    return 0;
}

void http_consume(int clientfd) {
    char* buf = http_buf;
    int n;
    int last_was_newline = 0;
    while (1) {
        n = read(clientfd, buf, sizeof(http_buf) - 1); // leave space for null terminator
        if (n > 0) {
            if (write(stdout, buf, n) < 0) {
                perror("write");
                return;
            }
        } else {
            if (n < 0) perror("read");
            break;
        }
        buf[n] = 0; // null terminate
        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                if (last_was_newline) {
                    // Detected two consecutive newlines (\n\n or \r\n\r\n)
                    println("End of HTTP request");
                    return;
                }
                last_was_newline = 1;
            } else if (buf[i] != '\r') {
                last_was_newline = 0;
            }
        }
    }
}

void http_drop(int clientfd) {
    shutdown(clientfd, SHUT_RDWR);
    close(clientfd);
}

#define http_code(clientfd, x) \
    fprint(clientfd, "HTTP/1.1 " x "\r\n\r\n")

int http_serve(int clientfd, char* file_path) {
    char* buf = http_buf;
    int f;
    http_consume(clientfd);
    http_code(clientfd, "200 OK");
    /* TODO: content length */
    f = open(file_path, O_RDONLY);
    if (f < 0) {
        perror("open");
        http_code(clientfd, "404 Not Found");
        return 1;
    }
    while (1) {
        int n = read(f, buf, sizeof(buf));  // leave space for null terminator
        if (n > 0) {
            if (write(clientfd, buf, n) < 0) {
                perror("write162");
                return 1;
            }
        } else {
            if (n < 0) perror("read");
            break;
        }
    }
    http_drop(clientfd);
    return 0;
}

int atoi(const char* str) {
    int res = 0;
    for (; *str; str++) {
        if (*str < '0' || *str > '9') return 0;
        res = res * 10 + (*str - '0');
    }
    return res;
}

void usage() {
    println("httpd <file-to-serve> [port]");
    println("Default port is 8080 if not specified");
    exit(1);
}

int main(int argc, char* argv[]) {
    int sock;
    int port = 8080; // default port
    if (argc < 3) {
        usage();
    }
    port = atoi(argv[2]);
    if (!port) {
        usage();
    }

    sock = tcp_listen(port);
    while (1) {
        int pid;
        int clientfd = accept(sock, 0, 0);
        if (clientfd < 0) {
            perror("accept");
            continue;
        }

        /* fork a process to handle the request */
        pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        }
        if (!pid) {
            return http_serve(clientfd, argv[1]);
        }
    }
    return 0;
}

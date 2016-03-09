#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define TRUE   1
#define FALSE  0

// Process number
#define FORK_NUM 1

#define PORT "/dev/ttyS1"
#define FAILED -1
#define SUCCESS 0

#define UDP_SERVER_PORT 8000
#define TCP_SERVER_PORT 8080
#define BACK_LOG 10

#define IF_NAME "br0" //"eth0"
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

#define FLAG_UDP 1
#define FLAG_TCP 2

int udp_server_socket_fd = 0;
int tcp_server_socket_fd  = 0;
int fd_tcp_accepted[BACK_LOG];

fd_set fdsr;
int maxsock = 0;
#define fds_reset() FD_ZERO(&fdsr)
#define fds_add(fd) FD_SET(fd, &fdsr); if (fd > maxsock) maxsock = fd
#define fds_isset(fd) FD_ISSET(fd, &fdsr)
#define fds_clr(fd) FD_CLR(fd, &fdsr)

pthread_t th_udp_client;

static void server_loop();
void output_data(int flag, char *buf, int len);

static int create_udp_server();
int poll_udp_server(int socket_fd);
static int create_tcp_server();
int poll_tcp_server(int socket_fd);

int UART_Open(int fd,char* port)
{
    fd = open(port, O_RDWR|O_NOCTTY|O_NDELAY);
    if (FAILED == fd)
    {
        perror("Can't Open Serial Port");
	printf("failed prot = %s \n", port);
        return FAILED;
    }
    // Recover serial to be blocked
    if(fcntl(fd, F_SETFL, 0) < 0)
    {
        printf("fcntl failed!\n");
        return FAILED;
    }
    else
    {
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
    }
    //Test whether terminal equipment
    if(0 == isatty(STDIN_FILENO))
    {
        printf("standard input is not a terminal device\n");
        return FAILED;
    }
    else
    {
        printf("isatty success!\n");
    }
    printf("fd->open=%d\n",fd);
    return fd;
}

void UART_Close(int fd)
{
    close(fd);
}

int UART_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)
{
    int   i;
    int   status;
    int   speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300};
    int   name_arr[] = {115200, 38400, 19200, 9600, 4800, 2400, 1200, 300};

    struct termios options;

    /* tcgetattr (fd, & options) obtained parameters related with fd, and save them in the options, 
    this function can also test the configuration is correct, the serial port is available and so on. 
    If the call is successful, the function returns 0, if the call fails, the function returns 1.
    */
    if  ( tcgetattr( fd,&options)  !=  0)
    {
        perror("SetupSerial 1");
        return FAILED;
    }

    // Set input and output baud rates
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
        if  (speed == name_arr[i])
        {
            cfsetispeed(&options, speed_arr[i]);
            cfsetospeed(&options, speed_arr[i]);
        }
    }

    //Modify the control mode to ensure that the program does not occupy the serial port
    options.c_cflag |= CLOCAL;
    //Modify the control mode to enable reading input data from a serial port
    options.c_cflag |= CREAD;

    //Setting data flow control
    switch(flow_ctrl)
    {
        case 0 ://Do not use flow control
            options.c_cflag &= ~CRTSCTS;
            break;

        case 1 ://Use hardware flow control
            options.c_cflag |= CRTSCTS;
            break;

        case 2 ://Use software flow control
            options.c_cflag |= IXON | IXOFF | IXANY;
            break;
    }

    //Set the data bits
    //Mask other flag
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
        case 5:
            options.c_cflag |= CS5;
            break;
        case 6:
            options.c_cflag |= CS6;
            break;
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr,"Unsupported data size\n");
            return FAILED;
    }

    //Setting the parity bit
    switch (parity)
    {
        case 'n':
        case 'N': // No parity
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;

        case 'o':
        case 'O':// Odd parity
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
            break;

        case 'e':
        case 'E':// Even parity
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;

        case 's':
        case 'S': // Spaces
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;

        default:
            fprintf(stderr,"Unsupported parity\n");
            return FAILED;
    } 

    // Set stop bits
    switch (stopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;

        case 2:
            options.c_cflag |= CSTOPB;
            break;

        default:
            fprintf(stderr,"Unsupported stop bits\n");
            return FAILED;
    }

    // Modify the output mode, the raw data output
    options.c_oflag &= ~OPOST;

    //options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //options.c_lflag &= ~(ISIG | ICANON);

    // Set the wait time and minimum receive characters
    options.c_cc[VTIME] = 1; /* Reads a character waiting 1 * (1/10) s */
    options.c_cc[VMIN] = 1; /* The minimum number of characters to read 1 */

    //If data overflow occurs, just flush data, without reading
    tcflush(fd,TCIFLUSH);

    //Activate configuration (set termios modified data to the serial port)
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        perror("com set error!\n");
        return FAILED;
    }
    return SUCCESS;
}

int UART_Recv(int fd, char *rcv_buf,int data_len)
{
    int len,fs_sel;
    fd_set fs_read;

    struct timeval time;

    FD_ZERO(&fs_read);
    FD_SET(fd,&fs_read);

    time.tv_sec = 10;
    time.tv_usec = 0;

    //Use select Serial multiplex communication
    //fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);
    //if(fs_sel)
    //{
        len = read(fd,rcv_buf,data_len);
        printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);
        return len;
    //}
    //else
    //{
    //    printf("Sorry,I am wrong!");
    //    return FAILED;
    //}
}

int UART_Send(int fd, char *send_buf,int data_len)
{
    int len = 0;
   
    len = write(fd,send_buf,data_len);
    if (len == data_len )
              {
                     return len;
              }     
    else   
        {
               
                tcflush(fd,TCOFLUSH);
                return FALSE;
        }
   
}

void uart_loop() {
    int fd;
    char send_buf[20]="tiger john";
    fd = UART_Open(fd,PORT);
    UART_Set(fd,38400,0,8,1,'N');
    //dup2(fd,1);

    printf("whzwhz \n");
    fprintf(stderr, "errwhzwhz \n");

    /*if (FALSE == open("/var/tmp/whz.txt", O_CREAT|O_TRUNC|O_RDWR))
    {
        perror("Can't Open whz.txt");
        //return(FALSE);
    }*/

    int len = 0;
    char rcv_buf[100]; 
    int i;
    /*while(1) {
        len = UART_Recv(fd, rcv_buf,9);
        if(len > 0)
        {
           rcv_buf[len] = '\0';
           printf("receive data is %s\n",rcv_buf);
           printf("len = %d\n",len);
        }
        else
        {
           printf("cannot receive data\n");
        }
    }*/

    for(i = 0;i < 10;i++)
     {
            len = UART_Send(fd,send_buf,10);
            if(len > 0)
                   printf(" %d send data successful\n",i);
            else
                   printf("send data failed!\n");
      
            sleep(2);
     }
     UART_Close(fd); 
}

int main() 
{
    // Fork all process
    int cpid, ppid = 0;
    pid_t pid;
    pid = fork();
/*
    int i;
    for (i = 0; i < FORK_NUM; i++)
    {
        pid = fork();
        if (pid == 0 || pid == -1) break;
    }
*/
    switch (pid) {
        case -1:
            /* error */
            perror("fork");
            exit(1);
            break;
        case 0:
            /* child, success */
            cpid = getpid();
            printf("child pid = %d\n", cpid);
            break;
        default:
            /* parent, success */
            ppid = getpid();
            printf("parent pid = %d\n", ppid);
            exit(0);
            break;
    }

    if (getpid() == cpid) { // Only child
        uart_loop();
        //server_loop();
    } else {
        printf("Not child, exit!");
    }

    return 0;
}

static void server_loop() {
    if (udp_server_socket_fd <= 0) {
        udp_server_socket_fd = create_udp_server();
        if (FAILED == udp_server_socket_fd) {
            close(udp_server_socket_fd);
        } else {
            printf("UDP: create success\n");
        }
    }
    if (tcp_server_socket_fd <= 0) {
        tcp_server_socket_fd = create_tcp_server();
        if (FAILED == tcp_server_socket_fd) {
            close(tcp_server_socket_fd);
        } else {
            printf("TCP: create success\n");
        }
    }

    // Init client and data buffer
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buf_recv[BUFFER_SIZE];

    int i = 0;
    int conn_amount = 0;
    int close_cnt = 0;
    // timeout setting
    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;

    /* Data transmission */
    while (1) {
        // initialize file descriptor set
        fds_reset();
        fds_add(udp_server_socket_fd);
        fds_add(tcp_server_socket_fd);
        // add active connection to fd set
        for (i = 0; i < BACK_LOG; i++) {
            if (fd_tcp_accepted[i] > 0) {
                fds_add(fd_tcp_accepted[i]);
            }
        }

        // select, delay timeout: tv
        int ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
        if (ret < 0) {
            perror("select");
            break;
        } else if (ret == 0) {
            //printf("TCP: select timeout\n");
            continue;
        }

        // check every fd in the set
        if (fds_isset(udp_server_socket_fd)) { // udp
            /* Receive data */
            bzero(buf_recv, BUFFER_SIZE);
            if (recvfrom(udp_server_socket_fd, buf_recv, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len) == FAILED) {
                perror("UDP: Receive Data Failed\n");
                continue;
                //exit(1);
            }

            output_data(FLAG_UDP, buf_recv, strlen(buf_recv));
        }
        if (fds_isset(tcp_server_socket_fd)) { // tcp new
            int new_fd = accept(tcp_server_socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
            if (new_fd <= 0) {
                perror("TCP: accept");
                continue;
            }

            bzero(buf_recv, BUFFER_SIZE);
            // add to accepted queue
            if (conn_amount < BACK_LOG) {
                fd_tcp_accepted[conn_amount++] = new_fd;

                sprintf(buf_recv, "new client[%d] %s:%d", conn_amount, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                output_data(FLAG_TCP, buf_recv, strlen(buf_recv));
            } else {
                sprintf(buf_recv, "max connections, close");
                output_data(FLAG_TCP, buf_recv, strlen(buf_recv));

                close(new_fd);
                //break;
            }
        } 

        for (i = 0; i < conn_amount; i++) { // tcp accepted
            if (fds_isset(fd_tcp_accepted[i])) {
                bzero(buf_recv, BUFFER_SIZE);
                ret = recv(fd_tcp_accepted[i], buf_recv, sizeof(buf_recv), 0);
                if (ret > 0) { // receive data
                    if (ret <= BUFFER_SIZE) {
                        memset(&buf_recv[ret], '\0', 1);
                    }

                    output_data(FLAG_TCP, buf_recv, strlen(buf_recv));
                } else { // client close
                    close(fd_tcp_accepted[i]);
                    FD_CLR(fd_tcp_accepted[i], &fdsr);
                    fd_tcp_accepted[i] = 0;
                    close_cnt++;

                    sprintf(buf_recv, "client[%d] close", i);
                    output_data(FLAG_TCP, buf_recv, strlen(buf_recv));
                }
            }
        }
        // Throw the closed fd to the last
        while (close_cnt > 0) {
            for (i = 0; i < (conn_amount - 1); i++) {
                if (0 == fd_tcp_accepted[i] && fd_tcp_accepted[i + 1] != 0) {
                    fd_tcp_accepted[i] = fd_tcp_accepted[i + 1];
                    fd_tcp_accepted[i + 1] = 0;
                }
            }
            close_cnt--;
            conn_amount--;
        }

    }

    /*
    // Create thread func
    if (pthread_create(&th_udp_client, NULL, (void *)&udp_client, NULL) != 0) {
        printf("pthread_create error!\n");
        exit(1);
    }
    // Wait thread exit
    if (pthread_join(th_udp_client, NULL) != 0) {
        printf("pthread_join error!\n");
        exit(1);
    }else {
        printf("pthread_join success!\n");
    }
    */
}

void output_data(int flag, char *buf, int len) {
    char pdata[BUFFER_SIZE];
    if (flag == FLAG_UDP) {
        strcpy(pdata, "<UDP> ");
    } else if (flag == FLAG_TCP) {
        strcpy(pdata, "<TCP> ");
    }
    /* Copy data from the buffer */
    int max_len = BUFFER_SIZE - strlen(pdata);
    strncat(pdata, buf, ((len > max_len) ? max_len : len));
    // Output
    printf("%s\n", pdata);
}

static int create_udp_server() {
    /* Create UDP socket */
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (FAILED == socket_fd)
    {
        perror("UDP: Create Socket Failed\n");
        return FAILED;
    }

    /* Reuse socket addr */
    int reuseAddr = TRUE;
    if (FAILED == (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuseAddr, sizeof(reuseAddr)))) {
        perror("UDP: setsockopt error\n");
        return FAILED;
    }

    /* Init UDP server address */
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Default address
    server_addr.sin_port = htons(UDP_SERVER_PORT);

    struct ifreq ifr = {0};
    strcpy(ifr.ifr_name, IF_NAME); // Default network name
    if (ioctl(socket_fd, SIOCGIFADDR, &ifr) < 0) {
        perror("UDP: ioctl error\n");
    } else {
        char* address = inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr);
        server_addr.sin_addr.s_addr = inet_addr(address); // Current address
        printf("UDP: Current IP address: %s\n",address);
    }

    /* Bind socket */
    if (FAILED == (bind(socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr)))) {
        perror("UDP: Server Bind Failed\n");
        return FAILED;
    }

    return socket_fd;
}

int poll_udp_server(int socket_fd) {

    /* Define an address, for capturing client address */
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buf_recv[BUFFER_SIZE];
    char pdata[BUFFER_SIZE];
    /* Data transmission */
    while(1)
    {
        /* Receive data */
        bzero(buf_recv, BUFFER_SIZE);
        if (FAILED == (recvfrom(socket_fd, buf_recv, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len)))
        {
            perror("UDP: Receive Data Failed\n");
            continue;
            //exit(1);
        }

        /* Copy data from the buffer */
        bzero(pdata, BUFFER_SIZE);
        strncpy(pdata, buf_recv, ((strlen(buf_recv) > BUFFER_SIZE) ? BUFFER_SIZE : strlen(buf_recv)));
        // Output
        printf("%s\n", pdata);
    }
    close(socket_fd);

    return 0;
}

static int create_tcp_server() {
    /* Create UDP socket */
    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (FAILED == socket_fd)
    {
        perror("TCP: Create Socket Failed\n");
        return FAILED;
    }

    /* server socket is nonblocking */
    int flag = fcntl(socket_fd, F_GETFL, 0);
    if (flag > 0) {
        if (FAILED == fcntl(socket_fd, F_SETFL, flag | O_NONBLOCK)) {
            perror("TCP: Set socket nonblock failed");
        }
    }

    /* close server socket on exec so CGIs can't write to it */
    if (FAILED == fcntl(socket_fd, F_SETFD, 1)) {
        perror("TCP: Set close-on-exec on failed!");
    }

    /* Reuse socket addr */
    int reuseAddr = TRUE;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuseAddr, sizeof(reuseAddr)) == FAILED) {
        perror("TCP: setsockopt error\n");
        return FAILED;
    }

    /* Init UDP server address */
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Default address
    server_addr.sin_port = htons(TCP_SERVER_PORT);

    struct ifreq ifr = {0};
    strcpy(ifr.ifr_name, IF_NAME); // Default network name
    if (ioctl(socket_fd, SIOCGIFADDR, &ifr) < 0) {
        perror("TCP: ioctl error\n");
    } else {
        char* address = inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr);
        server_addr.sin_addr.s_addr = inet_addr(address); // Current address
        printf("TCP: Current IP address: %s\n",address);
    }

    /* Bind socket */
    if (bind(socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr)) == FAILED) {
        perror("TCP: Server Bind Failed\n");
        return FAILED;
    }

    /* listen: large number just in case your kernel is nicely tweaked */
    if (listen(socket_fd, BACK_LOG) == FAILED) {
        perror("TCP: Unable to listen\n");
        return FAILED;
    }

    return socket_fd;
}

int poll_tcp_server(int socket_fd) {
    fd_set fdsr;
    int i = 0;
    int maxsock = socket_fd;
    int conn_amount = 0;
    int close_cnt = 0;
    // timeout setting
    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buf_recv[BUFFER_SIZE];
    char pdata[BUFFER_SIZE];

    while (1) {
        // initialize file descriptor set
        FD_ZERO(&fdsr);
        FD_SET(socket_fd, &fdsr);
        // add active connection to fd set
        for (i = 0; i < BACK_LOG; i++) {
            if (fd_tcp_accepted[i] > 0) {
                FD_SET(fd_tcp_accepted[i], &fdsr);
            }
        }

        int ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
        if (ret < 0) {
            perror("TCP: select");
            break;
        } else if (ret == 0) {
            //printf("TCP: select timeout\n");
            continue;
        }

        // check every fd in the set
        for (i = 0; i < conn_amount; i++) {
            if (FD_ISSET(fd_tcp_accepted[i], &fdsr)) {
                bzero(buf_recv, BUFFER_SIZE);
                ret = recv(fd_tcp_accepted[i], buf_recv, sizeof(buf_recv), 0);
                if (ret > 0) { // receive data
                    if (ret <= BUFFER_SIZE) {
                        memset(&buf_recv[ret], '\0', 1);
                    }
                    /* Copy data from the buffer */
                    bzero(pdata, BUFFER_SIZE);
                    strncpy(pdata, buf_recv, ((strlen(buf_recv) > BUFFER_SIZE) ? BUFFER_SIZE : strlen(buf_recv)));
                    // Output
                    printf("<TCP> client[%d]: %s\n", i, pdata);
                } else { // client close
                    close(fd_tcp_accepted[i]);
                    FD_CLR(fd_tcp_accepted[i], &fdsr);
                    fd_tcp_accepted[i] = 0;
                    close_cnt++;
                    printf("<TCP> client[%d] close\n", i);
                }
            }
        }

        // Throw the closed fd to the last
        while (close_cnt > 0) {
            for (i = 0; i < (conn_amount - 1); i++) {
                if (0 == fd_tcp_accepted[i] && fd_tcp_accepted[i + 1] != 0) {
                    fd_tcp_accepted[i] = fd_tcp_accepted[i + 1];
                    fd_tcp_accepted[i + 1] = 0;
                }
            }
            close_cnt--;
            conn_amount--;
        }

        // check whether a new connection comes
        if (FD_ISSET(socket_fd, &fdsr)) {
            int new_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
            if (new_fd <= 0) {
                perror("TCP: accept");
                continue;
            }

            // add to fd queue
            if (conn_amount < BACK_LOG) {
                fd_tcp_accepted[conn_amount++] = new_fd;
                printf("<TCP> new client[%d] %s:%d\n", conn_amount, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                if (new_fd > maxsock) {
                    maxsock = new_fd;
                }
            } else {
                printf("<TCP> max connections, close\n");
                close(new_fd);
                //break;
            }
        }

    }

    close(socket_fd);

    return 0;
}



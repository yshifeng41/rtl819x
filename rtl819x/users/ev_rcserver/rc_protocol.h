#ifndef RC_PROTOCOL_H
#define RC_PROTOCOL_H

#include <netinet/in.h>

#define MCU_UART_HEADER_LEN 13
#define MAX_DATA_LENGTH 128

#define UDP_DATA_LENGTH 14

#define FC_PACKAGE_LEN(x) ((x)[1])
#define FC_PACKAGE_CRC_A(x) ((x)[FC_PACKAGE_LEN(x)-2])
#define FC_PACKAGE_CRC_B(x) ((x)[FC_PACKAGE_LEN(x)-1])

#define UART_PACKAGE_LEN(x) ((x)[2])
#define UART_PACKAGE_CRC(x) ((x)[UART_PACKAGE_LEN(x)+2])

#define FC_SERVER_PORT 6666
#define FC_SERVER_IP "192.168.1.254"

#define UDP_SERVER_PORT 8000
#define TCP_SERVER_PORT 8080
#define BACK_LOG 10

typedef unsigned char	uint8;

struct sockaddr_in fc_server_addr; 
int fc_client_socket_fd;

void RC_ParseUartBuf(char *rec_buf, int len);
void printBuf(uint8 *data, int len, const char *name);

#endif
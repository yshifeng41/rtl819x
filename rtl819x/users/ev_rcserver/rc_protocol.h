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

typedef unsigned char	uint8;

struct sockaddr_in server_addr; 
int client_socket_fd;

void RC_ParseUartBuf(char *rec_buf, int len);
void printBuf(uint8 *data, int len, const char *name);

#endif
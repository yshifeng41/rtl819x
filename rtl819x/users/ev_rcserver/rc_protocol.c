#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "rc_protocol.h"

uint8 pUart1RxData[MAX_DATA_LENGTH] = {0};
uint8 pUart1RxLastRemainData[MAX_DATA_LENGTH] = {0};
uint8 hasPendingRemainDate = 0;
int lastRemainDate_index = 0;

void printBuf(uint8 *data, int len, const char *name) {
    int i;
    printf("len = %d, printBuf  %s:\n", len, name);
    for (i = 0; i < len; i++) {
          printf("0x%02x ", *(data + i));
    }
    printf("\n");
}

void printBufandSend(uint8 *data, int len, const char *name) {
    //printBuf(data, len, name);
    if (fc_client_socket_fd > 0) {
       if(sendto(fc_client_socket_fd, data, len, 0, (struct sockaddr*)&fc_server_addr, sizeof(fc_server_addr)) < 0) {
            printf("Send data Failed! \n");
       }
   } else {
       printf("Socket Create Failed:\n");
   }
}

uint8 generateCrc8(uint8 *ptr, uint8 len) {
    uint8 crc = 0;
    uint8 i;
    while (len--) {
        for (i = 0x80; i != 0; i >>= 1) {
            if((crc & 0x80) != 0) {
                crc <<= 1;
                crc ^= 0x07;
            } else {
                crc <<= 1;
            }
            if ((*ptr & i) != 0) {
                crc ^= 0x07;
            }
        }
        ptr++;
    }
    return (crc);
}

uint8 checkUartdataCrc(int head_index, uint8 *data) {
    if (UART_PACKAGE_LEN(data + head_index) > MAX_DATA_LENGTH) return 0;
    uint8 pSize =UART_PACKAGE_LEN(data + head_index); // + siezof(0x5555) + sizeof(len)
    uint8 calcCrc = generateCrc8(&data[head_index + 3], (pSize -4));
    return (calcCrc == UART_PACKAGE_CRC(data + head_index));
}

uint8 checkFCdataCrc(uint8 *data) {
    if (FC_PACKAGE_LEN(data) > MAX_DATA_LENGTH) return 0;
    uint8 chkA = 0x00;
    uint8 chkB = 0x00;
    uint8 j;
    for (j = 1; j < FC_PACKAGE_LEN(data) - 2; j++) {
        chkA += *(data + j);
        chkB += chkA;
    }
    uint8 a = FC_PACKAGE_CRC_A(data);
    uint8 b = FC_PACKAGE_CRC_B(data);
    return (chkA == a && chkB == b);
}

void formatUartdataToFCdata(int head_index, uint8 *data) {
    int i;
    int FCdata_len = *(data + (head_index + MCU_UART_HEADER_LEN ));
    uint8 *FCdata = malloc(FCdata_len*sizeof(uint8));
    *FCdata = 0x99;  //preamble(0x99)
    for(i = 0; i < FCdata_len + 1; i++) {
          *(FCdata + i + 1) = *(data + (head_index + MCU_UART_HEADER_LEN + i));
    }
    if (checkFCdataCrc(FCdata)) {
        uint8 *UDPdata = malloc(UDP_DATA_LENGTH*sizeof(uint8));
        UDPdata[0] = 0x01;
        UDPdata[1] = 0x0c;
        UDPdata[2] = 0xa9;
        for(i = 0; i < UDP_DATA_LENGTH - 3; i++) {
            UDPdata[i + 3] = data[head_index + MCU_UART_HEADER_LEN + i + 4]; // +4 for fc header 0x12 0xa9 0x35 0x00
        }
        printBufandSend(UDPdata, UDP_DATA_LENGTH, "UDPdata");
        free(UDPdata);
    } else {
        printBuf(FCdata, FCdata_len, "FCdataCrc fail");
        printf("FCdate Crc check failed!\n");
    }
    free(FCdata);
}

void RC_ParseUartBuf(char *rec_buf, int len) {
    int i, j;

    bzero(pUart1RxData, sizeof(pUart1RxData));
    for(i = 0; i < len; i++) {
        pUart1RxData[i] = (uint8)(*(rec_buf + i));
    }

    i = 0;
    while(i < len) {
        if ((i + 1 < len) && pUart1RxData[i] == 0x55 && pUart1RxData[i + 1] == 0x55) {
            if ((i + 2 < len) && ((UART_PACKAGE_LEN(&pUart1RxData[i]) + i) <= len)) {
                if (checkUartdataCrc(i, pUart1RxData)) {
                    formatUartdataToFCdata(i, pUart1RxData);
                } else {
                    printf("Uart Crc check failed!\n");
                    printBuf(pUart1RxData, len, "CrcFail");
                }
                i += UART_PACKAGE_LEN(&pUart1RxData[i]); //Point to Next Uart Package header
            } else {
                while(i < len) {
                    if (lastRemainDate_index < MAX_DATA_LENGTH) {
                        pUart1RxLastRemainData[lastRemainDate_index++] = pUart1RxData[i];
                    }
                    i++;
                }
                hasPendingRemainDate = 1;
                break;
            }
        } else {
            if (lastRemainDate_index < MAX_DATA_LENGTH && hasPendingRemainDate) {
                pUart1RxLastRemainData[lastRemainDate_index++] = pUart1RxData[i];
                j = 0;
                while(j < lastRemainDate_index) {
                    if ((j + 1 < lastRemainDate_index) && pUart1RxLastRemainData[j] == 0x55 && pUart1RxLastRemainData[j + 1] == 0x55) {
                        if ((j + 2 < lastRemainDate_index) && ((UART_PACKAGE_LEN(&pUart1RxLastRemainData[j]) + j) < lastRemainDate_index)) {
                            if (checkUartdataCrc(j, pUart1RxLastRemainData)) {
                                formatUartdataToFCdata(j, pUart1RxLastRemainData);
                            }  else {
                                printf("Uart Crc check failed!\n");
                                printBuf(pUart1RxLastRemainData, len, "CrcFail remain");
                            }
                            hasPendingRemainDate = 0;
                            lastRemainDate_index = 0;
                            bzero(pUart1RxLastRemainData, sizeof(pUart1RxLastRemainData));
                        }
                        break;
                    } else {
                        j++;
                    }
                }
            }
            i++;
        }
    }
}


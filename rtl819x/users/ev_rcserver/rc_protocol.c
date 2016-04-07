#include <string.h>
#include "rc_protocol.h"

#define MCU_UART_HEADER_LEN 13
#define MAX_DATA_LENGTH 255

typedef unsigned char	uint8;
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

void formatUartdataToFCdata(int head_index, uint8 *data) {
    int i;
    int FCdata_len = *(data + (head_index + MCU_UART_HEADER_LEN));
    uint8 *FCdata = malloc(FCdata_len*sizeof(uint8));
    *FCdata = 0x99;  //preamble(0x99)
    for(i = 1; i < FCdata_len; i++) {
          *(FCdata + i) = *(data + (head_index + MCU_UART_HEADER_LEN + i -1));
    }
    printBuf(FCdata, FCdata_len, "FCdata");
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
            if ((i + 2 < len) && ((pUart1RxData[i + 2] + i + 2) < len)) {
                formatUartdataToFCdata(i, pUart1RxData);
                i += pUart1RxData[i + 2] + 3; //Point to Next Uart Package header
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
                        if ((j + 2 < len) && ((pUart1RxLastRemainData[j + 2] + j + 2) < lastRemainDate_index)) {
                            formatUartdataToFCdata(j, pUart1RxLastRemainData);
                            hasPendingRemainDate = 0;
                            lastRemainDate_index = 0;
                            bzero(pUart1RxLastRemainData, sizeof(pUart1RxLastRemainData));
                            break;
                        } else {
                            break;
                        }
                    } else {
                        j++;
                    }
                }
            }
            i++;
        }
    }
}


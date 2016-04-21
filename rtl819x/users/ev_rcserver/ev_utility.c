#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ev_utility.h"

int do_system(const char *cmd) {
    pid_t status;

    status = system(cmd);

    if (-1 == status) {
        printf("do system error!");
        return -1;
    } else {
        printf("exit status value = [0x%x]\n", status);

        if (WIFEXITED(status)) {
            if (0 == WEXITSTATUS(status)) {
                printf("do system successfully.\n");
            } else {
                printf("do system fail, exit code: %d\n", WEXITSTATUS(status));
                return -2;
            }
        } else {
            printf("exit status = [%d]\n", WEXITSTATUS(status));
            return -3;
        }
    }
    return 0;
}


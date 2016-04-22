#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/if_ether.h> 
#include <linux/wireless.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/signal.h>

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

static inline int
iw_get_ext(int                  skfd,           /* Socket to the kernel */
           char *               ifname,         /* Device name */
           int                  request,        /* WE ID */
           struct iwreq *       pwrq)           /* Fixed part of the request */
{
  /* Set device name */
  strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);
  /* Do the request */
  return(ioctl(skfd, request, pwrq));
}

int get_wlan_bssinfo(char *interface, WLAN_BSS_INFO_Tp pInfo)
{

    int skfd=0;
    struct iwreq wrq;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd==-1)
		return -1;
    /* Get wireless name */
    if ( iw_get_ext(skfd, interface, SIOCGIWNAME, &wrq) < 0)
      /* If no wireless name : no wireless extensions */
      {
      	 close( skfd );
        return -1;
      }

    wrq.u.data.pointer = (caddr_t)pInfo;
    wrq.u.data.length = sizeof(bss_info);

    if (iw_get_ext(skfd, interface, SIOCGIWRTLGETBSSINFO, &wrq) < 0){
    	 close( skfd );
	return -1;
	}
    close( skfd );

    return 0;
}

//STATE_DISABLED=0, STATE_IDLE=1, STATE_SCANNING=2, STATE_STARTED=3, STATE_CONNECTED=4, STATE_WAITFORKEY=5
int is_wlan_connected(const char *wlan_if)
{	
	WLAN_BSS_INFO_T bss;
	if ( get_wlan_bssinfo(wlan_if, &bss) < 0)
			return -1;

	return bss.state == STATE_CONNECTED;
}


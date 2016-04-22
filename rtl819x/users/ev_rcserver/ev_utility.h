#ifndef INCLUDE_EV_UTILITY_H
#define INCLUDE_EV_UTILITY_H
#include <sys/wait.h>

//----------------get_sta_info  define
//#define MAX_STA_NUM 32 
#define SSID_LEN	32
/* flag of sta info */
#define STA_INFO_FLAG_AUTH_OPEN     	0x01
#define STA_INFO_FLAG_AUTH_WEP      	0x02
#define STA_INFO_FLAG_ASOC          	0x04
#define STA_INFO_FLAG_ASLEEP        	0x08

typedef enum _wlan_mac_state {
    STATE_DISABLED=0, STATE_IDLE, STATE_SCANNING, STATE_STARTED, STATE_CONNECTED, STATE_WAITFORKEY
} wlan_mac_state;

/* wlan driver ioctl id */
#define SIOCGIWRTLSTAINFO   		0x8B30	// get station table information
#define SIOCGIWRTLSTANUM		0x8B31	// get the number of stations in table
#define SIOCGIWRTLSCANREQ		0x8B33	// scan request
#define SIOCGIWRTLGETBSSDB		0x8B34	// get bss data base
#define SIOCGIWRTLJOINREQ		0x8B35	// join request
#define SIOCGIWRTLJOINREQSTATUS		0x8B36	// get status of join request
#define SIOCGIWRTLGETBSSINFO		0x8B37	// get currnet bss info
#define SIOCGIWRTLGETWDSINFO		0x8B38
#define SIOCGMISCDATA	0x8B48	// get misc data

typedef struct wlan_bss_info {
    unsigned char state;
    unsigned char channel;
    unsigned char txRate;
    unsigned char bssid[6];
    unsigned char rssi, sq;	// RSSI  and signal strength
    unsigned char ssid[SSID_LEN+1];
} WLAN_BSS_INFO_T, *WLAN_BSS_INFO_Tp;

typedef struct bss_info {
    unsigned char bssid[8];	// first 6 bytes indicate as BSS ID/AP mac address, last 2 bytes are padding data
    unsigned char ssid[34];	// SSID in string
    unsigned short channel;
    unsigned char network;	// network type, bit-mask value: 
                            // 1 ¡V 11b, 2 ¡V 11g, 4 ¡V 11a-legacy, 8 ¡V 11n, 16 ¡V 11a-n
    unsigned char type;	// AP or Ad-hoc. 0 - AP, 1 - Ad-hoc
    unsigned char encrypt;// encryption type. 0 ¡V open, 1 ¡V WEP, 2 ¡V WPA, 3 ¡VWPA2, 
                            // 4 ¡VMixed Mode (WPA+WPA2)
    unsigned char rssi;		// received signal strength. 1-100
} bss_info;

int do_system(const char *cmd);

int is_wlan_connected(const char *wlan_if);

#endif


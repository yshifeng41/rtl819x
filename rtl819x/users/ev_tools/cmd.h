/*
  *   cmd header for 8198
  *
  *	Copyright (C)2008, Realtek Semiconductor Corp. All rights reserved.
  *
  *	$Id: cmd.h,v 1.5 2010/12/29 09:58:22 brian Exp $
  */

#ifndef INCLUDE_CMD_H
#define INCLUDE_CMD_H

// command ID
#define id_set_mib						0x01
#define id_get_mib						0x02
#define id_sysinit							0x03
#define id_getstainfo						0x0a
#define id_getassostanum					0x05
#define id_getbssinfo						0x06
#define id_getstats						0x07
#define id_getlanstatus					0x08
#define id_trigger_wps					0x09
#define id_ioctl						0x0b
#define id_syscall						0x04
#define id_firm_upgrade						0x20
#define id_set_mibs						0x1a
#define id_sync_to_server			0x0f
#define id_request_scan			0x21
#define id_get_scan_result		0x22
#define id_start_lite_clnt_connect	0x23
#define id_wlan_sync	0x24
#define id_set_wlan_on 	0x25
#define id_set_wlan_off 0x26
#define id_getlanRate				0x10
#define id_getlanRatePercent		0x11
#define id_setlanBandwidth			0x12
#define id_get_macfilter_rule		0x60
#define id_add_macfilter_rule_imm		0x61
#define id_del_macfilter_rule_imm		0x62
#define id_firm_check_sig_checksum	0x47
#define id_firm_check_flash_data	0x48
#define id_firm_upgrade_reboot		0x49

#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
#define id_set_rgb_blink_start		0x4a
#define id_set_rgb_blink_end		0x4b
#endif

#define id_get_fw_version		0x50
#define id_ev_get_fw_version		0x70
#define FW_VERSION_MAX_LEN		24

#define TEMP_MIB_FILE "/var/hcd_mib"

#define LAST_ENTRY_ID 0 //LAST_ENTRY_ID

#define MAX_HOST_CMD_LEN  65536
#define MAX_PATH_LEN 256
#define CMD_DEF(name, func) \
	{id_##name, #name,  func}

typedef struct{
    unsigned int thread_id;
    unsigned int cmd_type;
	unsigned int status;	//0:SUCCESS, 1:FAIL, 2:buf overflow
	unsigned int length;	//the buf length of return value 
	unsigned char error_info[64];
}RTK_INBAND_RESULT_HEADER,*pRTK_INBAND_RESULT_HEADER;

typedef struct{
	RTK_INBAND_RESULT_HEADER header;
	unsigned char *buf;
}RTK_INBAND_RESULT, *pRTK_INBAND_RESULT;


// CMD table for action
struct cmd_entry {                          
	int 		id;				// cmd id
	char 	*name;		// cmd name in string                 			
	int		 (*func)(char *data,int len);   //the fuction for act 
};           	


struct user_net_device_stats {
    unsigned long long rx_packets;	/* total packets received       */
    unsigned long long tx_packets;	/* total packets transmitted    */
    unsigned long long rx_bytes;	/* total bytes received         */
    unsigned long long tx_bytes;	/* total bytes transmitted      */
    unsigned long rx_errors;	/* bad packets received         */
    unsigned long tx_errors;	/* packet transmit problems     */
    unsigned long rx_dropped;	/* no space in linux buffers    */
    unsigned long tx_dropped;	/* no space available in linux  */
    unsigned long rx_multicast;	/* multicast packets received   */
    unsigned long rx_compressed;
    unsigned long tx_compressed;
    unsigned long collisions;

    /* detailed rx_errors: */
    unsigned long rx_length_errors;
    unsigned long rx_over_errors;	/* receiver ring buff overflow  */
    unsigned long rx_crc_errors;	/* recved pkt with crc error    */
    unsigned long rx_frame_errors;	/* recv'd frame alignment error */
    unsigned long rx_fifo_errors;	/* recv'r fifo overrun          */
    unsigned long rx_missed_errors;	/* receiver missed packet     */
    /* detailed tx_errors */
    unsigned long tx_aborted_errors;
    unsigned long tx_carrier_errors;
    unsigned long tx_fifo_errors;
    unsigned long tx_heartbeat_errors;
    unsigned long tx_window_errors;
};

#define RTL819X_IOCTL_READ_PORT_STATUS			(SIOCDEVPRIVATE + 0x01)	
#define RTL819X_IOCTL_READ_PORT_STATS	              (SIOCDEVPRIVATE + 0x02)	
#define RTL819X_IOCTL_READ_PORT_RATE			(SIOCDEVPRIVATE + 0x03)

#define _PATH_PROCNET_DEV	"/proc/net/dev"
#define _RTL_PORT_RATE		"/proc/rtl865x/portRate"
#define _RTL_PORT_BANDWIDTH		"/proc/rtl865x/port_bandwidth"

struct lan_port_status {
    unsigned char link;
    unsigned char speed;
    unsigned char duplex;
    unsigned char nway;    	
}; 

struct port_statistics {
	unsigned int  rx_bytes;
	unsigned int  rx_unipkts;
	unsigned int  rx_mulpkts;
	unsigned int  rx_bropkts;
	unsigned int  rx_discard;
	unsigned int  rx_error;
	unsigned int  tx_bytes;
	unsigned int  tx_unipkts;
	unsigned int  tx_mulpkts;
	unsigned int  tx_bropkts;
	unsigned int  tx_discard;
	unsigned int  tx_error;
};
struct port_rate {
	unsigned int  port_id;
	unsigned int  rx_rate;
	unsigned int  tx_rate;
	unsigned int  rx_percent;
	unsigned int  tx_percent;
};

/*
typedef struct wlan_rate{
	unsigned int id;
	unsigned char rate[20];
}WLAN_RATE_T, *WLAN_RATE_Tp;
*/
typedef enum _Synchronization_Sta_State{
    STATE_Min		= 0,
    STATE_No_Bss	= 1,
    STATE_Bss		= 2,
    STATE_Ibss_Active	= 3,
    STATE_Ibss_Idle	= 4,
    STATE_Act_Receive	= 5,
    STATE_Pas_Listen	= 6,
    STATE_Act_Listen	= 7,
    STATE_Join_Wait_Beacon = 8,
    STATE_Max		= 9
} Synchronization_Sta_State;

int do_cmd(int id , char *cmd ,int cmd_len ,int relply);
int do_action(int id , char *cmd ,int cmd_len ,int relply);


#endif 

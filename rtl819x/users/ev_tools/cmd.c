/*================================================================*/
/* System Include Files */

#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <string.h>
#include <sys/types.h> 
#include <sys/uio.h> 
#include <unistd.h> 
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/wait.h> 
#include <net/if.h>
#include <linux/if_packet.h>
#include "../inband_lib/wireless_copy.h"
/*================================================================*/
/* Local Include Files */

#include <sys/stat.h>

#include "ev_tools.h"
#include "mib.h"
#include "cmd.h"
#include "wlan_if.h"
#include "site_survey.h"

#ifdef KERNEL_3_10
#include <linux/fs.h>
#endif
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
#include "RgbLedBlink.h"
#endif
#include "../boa/src/version.c"	
//#include "inband_if.h"
#define MAX_INBAND_PAYLOAD_LEN 1480


/*=Local variables===============================================================*/
WLAN_RATE_T rate_11n_table_20M_LONG[]={
	{MCS0, 	"6.5"},
	{MCS1, 	"13"},
	{MCS2, 	"19.5"},
	{MCS3, 	"26"},
	{MCS4, 	"39"},
	{MCS5, 	"52"},
	{MCS6, 	"58.5"},
	{MCS7, 	"65"},
	{MCS8, 	"13"},
	{MCS9, 	"26"},
	{MCS10, 	"39"},
	{MCS11, 	"52"},
	{MCS12, 	"78"},
	{MCS13, 	"104"},
	{MCS14, 	"117"},
	{MCS15, 	"130"},
	{0}
};
WLAN_RATE_T rate_11n_table_20M_SHORT[]={
	{MCS0, 	"7.2"},
	{MCS1, 	"14.4"},
	{MCS2, 	"21.7"},
	{MCS3, 	"28.9"},
	{MCS4, 	"43.3"},
	{MCS5, 	"57.8"},
	{MCS6, 	"65"},
	{MCS7, 	"72.2"},
	{MCS8, 	"14.4"},
	{MCS9, 	"28.9"},
	{MCS10, 	"43.3"},
	{MCS11, 	"57.8"},
	{MCS12, 	"86.7"},
	{MCS13, 	"115.6"},
	{MCS14, 	"130"},
	{MCS15, 	"144.5"},
	{0}
};
WLAN_RATE_T rate_11n_table_40M_LONG[]={
	{MCS0, 	"13.5"},
	{MCS1, 	"27"},
	{MCS2, 	"40.5"},
	{MCS3, 	"54"},
	{MCS4, 	"81"},
	{MCS5, 	"108"},
	{MCS6, 	"121.5"},
	{MCS7, 	"135"},
	{MCS8, 	"27"},
	{MCS9, 	"54"},
	{MCS10, 	"81"},
	{MCS11, 	"108"},
	{MCS12, 	"162"},
	{MCS13, 	"216"},
	{MCS14, 	"243"},
	{MCS15, 	"270"},
	{0}
};
WLAN_RATE_T rate_11n_table_40M_SHORT[]={
	{MCS0, 	"15"},
	{MCS1, 	"30"},
	{MCS2, 	"45"},
	{MCS3, 	"60"},
	{MCS4, 	"90"},
	{MCS5, 	"120"},
	{MCS6, 	"135"},
	{MCS7, 	"150"},
	{MCS8, 	"30"},
	{MCS9, 	"60"},
	{MCS10, 	"90"},
	{MCS11, 	"120"},
	{MCS12, 	"180"},
	{MCS13, 	"240"},
	{MCS14, 	"270"},
	{MCS15, 	"300"},
	{0}
};
//changes in following table should be synced to VHT_MCS_DATA_RATE[] in 8812_vht_gen.c
const unsigned short VHT_MCS_DATA_RATE[3][2][20] = 
	{	{	{13, 26, 39, 52, 78, 104, 117, 130, 156, 156,
			 26, 52, 78, 104, 156, 208, 234, 260, 312, 312},			// Long GI, 20MHz
			{14, 29, 43, 58, 87, 116, 130, 144, 173, 173,
			29, 58, 87, 116, 173, 231, 260, 289, 347, 347}	},		// Short GI, 20MHz
		{	{27, 54, 81, 108, 162, 216, 243, 270, 324, 360, 
			54, 108, 162, 216, 324, 432, 486, 540, 648, 720}, 		// Long GI, 40MHz
			{30, 60, 90, 120, 180, 240, 270, 300,360, 400, 
			60, 120, 180, 240, 360, 480, 540, 600, 720, 800}},		// Short GI, 40MHz
		{	{59, 117,  176, 234, 351, 468, 527, 585, 702, 780,
			117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560}, 	// Long GI, 80MHz
			{65, 130, 195, 260, 390, 520, 585, 650, 780, 867, 
			130, 260, 390, 520, 780, 1040, 1170, 1300, 1560,1733}	}	// Short GI, 80MHz
	};

#define CFG_FILE_IN "/etc/Wireless/realtekap.conf"

char *lan_link_spec[2] = {"DOWN","UP"};
char *lan_speed_spec[3] = {"10M","100M","1G"};
char *enable_spec[2] = {"DIsable","Enable"};
//#define FLASH_DEVICE_KERNEL               ("/dev/mtd")
#define FLASH_DEVICE_KERNEL				("/dev/mtdblock0")

#define FLASH_DEVICE_ROOTFS              ("/dev/mtdblock1")

#define HDR_INFO_OFFSET 4
#define HDR_IOCTL_DATA_OFFSET 4+2
#define LOCAL_NETIF  ("br0")
static int ioctl_sock=-1;
static int ev_tools_inband_ioctl_chan=-1;
#define CMD_INBAND_IOCTL 0x03
#define CMD_INBAND_SYSTEMCALL 0x04

#define SIGNATURE_ERROR		(1<<0)
#define WEBCHECKSUM_ERROR	(1<<1)
#define ROOTCHECKSUM_ERROR	(1<<2)
#define LINUXCHECKSUM_ERROR	(1<<3)

//flash|memory data cmp
#define FLASHDATA_ERROR		(1<<8)
#define WEBDATA_ERROR		(1<<9)
#define LINUXDATA_ERROR		(1<<10)
#define ROOTDATA_ERROR		(1<<11)

static int firm_upgrade_status = 0;

/**/
static int _get_mib(char *cmd , int cmd_len);
static int _set_mib(char *cmd , int cmd_len);
static int _set_mibs(char *cmd , int cmd_len);
//static int _set_mibs(void);
static int _getstainfo(char *cmd , int cmd_len);
static int _getassostanum(char *cmd , int cmd_len);
static int _getbssinfo(char *cmd , int cmd_len);
static int _sysinit(char *cmd , int cmd_len);
static int _getstats(char *cmd , int cmd_len);	
static int _getlanstatus(char *cmd , int cmd_len);
static int _getlanRate(char *cmd,int cmd_len);
static int _setlanBandwidth(char *cmd, int cmd_len);

static int _firm_upgrade(char *cmd , int cmd_len);
static int _firm_check_signature_checksum(char *cmd , int cmd_len);
static int _firm_check_flash_data(char *cmd , int cmd_len);
static int _firm_upgrade_reboot(char *cmd , int cmd_len);
static int host_ioctl_receive(char *ioctl_data_p,int ioctl_data_len);
static int host_systemcall_receive(char *data,int data_len);

static int _request_scan(char *cmd , int cmd_len);
static int _get_scan_result(char *cmd , int cmd_len);

static int _get_firmware_version(char *cmd, int cmd_len);
static int _ev_get_firmware_version(char *cmd, int cmd_len);
static int _connect_ap(char *cmd, int cmd_len);
static int set_profile_to_flash(char *wlan_if, char *ssid,char *passwd,int encrypt_type);

struct cmd_entry cmd_table[]={ \
/*Action cmd - ( name, func) */
	CMD_DEF(set_mib, _set_mib),
	CMD_DEF(get_mib, _get_mib),	
	CMD_DEF(getstainfo, _getstainfo),
	CMD_DEF(getassostanum, _getassostanum),
	CMD_DEF(getbssinfo, _getbssinfo),
	CMD_DEF(sysinit, _sysinit),
	CMD_DEF(getstats, _getstats),
	CMD_DEF(getlanstatus, _getlanstatus),
	CMD_DEF(getlanRate, _getlanRate),
	CMD_DEF(setlanBandwidth, _setlanBandwidth),
	CMD_DEF(set_mibs, _set_mibs),
	CMD_DEF(ioctl, host_ioctl_receive),
	CMD_DEF(syscall, host_systemcall_receive),

	CMD_DEF(request_scan,_request_scan),
	CMD_DEF(get_scan_result,_get_scan_result),
        CMD_DEF(ev_get_fw_version, _ev_get_firmware_version),
#ifdef CONFIG_HCD_FLASH_SUPPORT  
    CMD_DEF(firm_upgrade, _firm_upgrade),
    CMD_DEF(firm_check_sig_checksum,_firm_check_signature_checksum),
    CMD_DEF(firm_check_flash_data,_firm_check_flash_data),
    CMD_DEF(firm_upgrade_reboot,_firm_upgrade_reboot),
#endif
	/* last one type should be LAST_ENTRY - */   
	{0}
};

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

int getWlJoinResult(char *interface, unsigned char *res)
{
    int skfd;
    struct iwreq wrq;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);

    /* Get wireless name */
    if ( iw_get_ext(skfd, interface, SIOCGIWNAME, &wrq) < 0){
      /* If no wireless name : no wireless extensions */
      close( skfd );
        return -1;
	}
    wrq.u.data.pointer = (caddr_t)res;
    wrq.u.data.length = 1;

    if (iw_get_ext(skfd, interface, SIOCGIWRTLJOINREQSTATUS, &wrq) < 0){
    	close( skfd );
	return -1;
	}
    close( skfd );

    return 0;
}

int do_connect(char *ssid, char *wpa_key, int encrypt)
{
    printf("[%s:%d]\n", __FUNCTION__,__LINE__);
    int ret = 0;
    char command[50];
    const char *wlanif = "wlan0";
    const char *wlanifp = "wlan0-vxd";
    int vxd_wisp_wan = 1;
    int encrypt_type;
    WLAN_BSS_INFO_T bss;

    if (encrypt)
        encrypt_type = 11;
    else
        encrypt_type = 0;

    if (get_wlan_bssinfo(wlanifp, &bss) < 0){
        printf("[%s:%d] check  failed, ignored \n", __FUNCTION__,__LINE__);
    } else {
        if (bss.state == STATE_CONNECTED && !strcmp(bss.ssid, ssid)) {
            printf("[%s:%d] already connected %s \n", __FUNCTION__,__LINE__, ssid);
            return ret;
        }
    }

    set_profile_to_flash(wlanifp, ssid, wpa_key, encrypt_type);
    printf("[%s:%d]\n", __FUNCTION__,__LINE__);

    if(vxd_wisp_wan){
        sprintf(command,"ifconfig %s-vxd down",wlanif);
        system(command);
    }
    sprintf(command,"ifconfig %s down",wlanif);
    system(command);
    sprintf(command,"flash set_mib %s",wlanif);
    system(command);
    sprintf(command,"ifconfig %s up",wlanif);
    system(command);
    if(vxd_wisp_wan){
        sprintf(command,"flash set_mib %s-vxd",wlanif);                      
        system(command);
        sprintf(command,"ifconfig %s-vxd up",wlanif);
        system(command);
    }
    // wlan0 entering forwarding state need some time
    sleep(3);

    system("sysconf wlanapp start wlan0 br0");
    //system("sysconf init gw bridge");
    sleep(1);
    /*sysconf upnpd 1(isgateway) 1(opmode is bridge)*/
    system("sysconf upnpd 1 1");
    sleep(1);

    int wait_time = 0;
    int max_wait_time = 15;
    unsigned char res;
    while (1)  {
        if (getWlJoinResult(wlanifp, &res) < 0) {
            printf("[%s:%d] conneting failed \n", __FUNCTION__,__LINE__);
            ret = -1;
            break;
        }

        if(res==STATE_Bss  || res==STATE_Ibss_Idle || res==STATE_Ibss_Active) { // completed 
            printf("[%s:%d] join success res = %d \n", __FUNCTION__,__LINE__, res);
            break;
        } else {
            if (wait_time++ > max_wait_time) {
                printf("[%s:%d] connecting time out \n", __FUNCTION__,__LINE__);
                ret = -1;
                break;
            }
        }
        sleep(1);
    }
    if (encrypt && ret == 0) {
        wait_time = 0;
        max_wait_time = 15;

        while (wait_time++ < max_wait_time) {
            if (get_wlan_bssinfo(wlanifp, &bss) < 0){
                printf("[%s:%d] get bssinfo failed \n", __FUNCTION__,__LINE__);
                ret = -1;
                break;
            }
            if (bss.state == STATE_CONNECTED){
                printf("[%s:%d] check encrypt success \n", __FUNCTION__,__LINE__);
                break;
            }
            sleep(1);
        }
        if (wait_time > max_wait_time){
            printf("[%s:%d] connecting time out \n", __FUNCTION__,__LINE__);
            ret = -1;
        }
    }
    printf("[%s:%d]\n", __FUNCTION__,__LINE__);
    return ret;
}

int do_cmd(int id , char *cmd ,int cmd_len ,int relply)
{
	int i=0,ret=-1,len=0;
	
	printf("[%d],%d,cmd:%d,%s [%s]:[%d].\n",i,id,cmd_len,cmd,__FUNCTION__,__LINE__);
	while (cmd_table[i].id != LAST_ENTRY_ID) {
		if ((cmd_table[i].id == id))	{
			ret = cmd_table[i].func(cmd,cmd_len);
			printf("[%d],%d,%s ret:%d,[%s]:[%d].\n",i,id,cmd,ret,__FUNCTION__,__LINE__);
			break;
		}	
		i++;
	}
	//no reply
	
	if(!relply || cmd_table[i].id == id_ioctl )
		return ret;

	if(ret > MAX_INBAND_PAYLOAD_LEN) //mark_issue , it will be reply in it's func in command table
		return ret;

	//reply rsp pkt
	if (ret >= 0) { 
		if (ret == 0) { 
			cmd[0] = '\0';
			ret = 1;
		}

	}
	else{ //error rsp	
		cmd[0] = (unsigned char)( ~ret + 1);			
	}			
	
	return ret;
}

int get_encrypt_type(int encrypt,int passwd_len)
{
       int type;
       switch(encrypt){
               case 0:
                       type = 0;
               break;
               case 1:
                       if(passwd_len == 5)
                               type = 1;
                       else if(passwd_len == 10)
                               type = 2;
                       else if (passwd_len == 13)
                               type = 3;
                       else if(passwd_len == 26)
                               type = 4;
                       else
                               type = -1;
               break;
               case 4:
                       type = 9;
               break;
               case 2:
                       type = 10;
               break;
               case 6:
                       type = 11;
               break;
               default:
                       type = -1;
               break;
       }
       return type;
}

static int set_profile_to_flash(char *wlan_if, char *ssid,char *passwd,int encrypt_type)
{
       unsigned char buffer[128]={0};
       unsigned char value[128];
       char config_prefix[16];
       //unsigned char tmp[128];
       unsigned char *p;
       //unsigned int security_type=0;
       //unsigned int wsc_auth=0, wsc_enc=0;
       //unsigned char wsc_psk[65] = {0};

       system("flash setconf start");
       if(!strcmp(wlan_if,"wlan0"))
               sprintf(config_prefix,"WLAN0_");
       else if(!strcmp(wlan_if,"wlan0-vxd"))
               sprintf(config_prefix,"WLAN0_VXD_");
       else if(!strcmp(wlan_if,"wlan1"))
               sprintf(config_prefix,"WLAN1_");
       else if(!strcmp(wlan_if,"wlan1-vxd"))
               sprintf(config_prefix,"WLAN1_VXD_");

       sprintf(buffer, "flash setconf %sSSID %s", config_prefix,ssid);
       system(buffer);
       sprintf(buffer, "flash setconf %sWLAN_DISABLED 0", config_prefix);
       system(buffer); 
       sprintf(buffer, "flash setconf %sMODE 1", config_prefix);
       system(buffer); 
       switch(encrypt_type)
       {
               case 0:
                       sprintf(buffer, "flash setconf %sENCRYPT 0", config_prefix);
                       system(buffer);
                       break;
               case 1:
                       sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWEP 1", config_prefix);
                       system(buffer);
                       sprintf(value, "%02x%02x%02x%02x%02x", passwd[0], passwd[1], passwd[2], passwd[3], passwd[4]);
                       value[10] = 0;
                       sprintf(buffer, "flash setconf %sWEP64_KEY1  %s", config_prefix, value);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 0", config_prefix);
                       system(buffer);                 
                       sprintf(buffer, "flash setconf %sAUTH_TYPE  2", config_prefix);
                       system(buffer);
                       break;
               case 2:
                       sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWEP 1", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWEP64_KEY1     %s", config_prefix, passwd);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 1", config_prefix);
                       system(buffer);                 
                       sprintf(buffer, "flash setconf %sAUTH_TYPE 2", config_prefix);
                       system(buffer);                 
                       break;
               case 3:
                       sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWEP 2", config_prefix);
                       system(buffer);
                       sprintf(value, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                               passwd[0], passwd[1], passwd[2], passwd[3], passwd[4], passwd[5], passwd[6],
                               passwd[7], passwd[8], passwd[9], passwd[10], passwd[11], passwd[12]
                               );
                       value[26] = 0;
                       sprintf(buffer, "flash setconf %sWEP128_KEY1 %s", config_prefix, value);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 0", config_prefix);
                       system(buffer);                 
                       sprintf(buffer, "flash setconf %sAUTH_TYPE 2", config_prefix);
                       system(buffer);                 
                       break;
               case 4:
                       sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWEP 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWEP128_KEY1 %s", config_prefix, passwd);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 1", config_prefix);
                       system(buffer);                 
                       sprintf(buffer, "flash setconf %sAUTH_TYPE 2", config_prefix);
                       system(buffer);                 
                       break;
               case 5:
                       sprintf(buffer, "flash setconf %sENCRYPT 4", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA2_CIPHER_SUITE 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
                       system(buffer);

                       break;
               case 6:
                       sprintf(buffer, "flash setconf %sENCRYPT 4", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA2_CIPHER_SUITE 1", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
                       system(buffer);
                       break;
               case 7:
                       sprintf(buffer, "flash setconf %sENCRYPT 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_CIPHER_SUITE 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
                       system(buffer);
                       break;
               case 8:
                       sprintf(buffer, "flash setconf %sENCRYPT 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_CIPHER_SUITE 1", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
                       system(buffer);
               
                       break;
               case 9:
                       sprintf(buffer, "flash setconf %sENCRYPT 4", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA2_CIPHER_SUITE 3", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
                       system(buffer);
                       break;
               case 10:
                       sprintf(buffer, "flash setconf %sENCRYPT 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_CIPHER_SUITE 3", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
                       system(buffer);
               
                       break;
               case 11:
                       sprintf(buffer, "flash setconf %sENCRYPT 6", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_CIPHER_SUITE 3", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA2_CIPHER_SUITE 3", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
                       system(buffer);
                       sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
                       system(buffer);
                   break;
               default:
                       sprintf(buffer, "flash setconf %sENCRYPT 0", config_prefix);
                       system(buffer);
                       break;
       }
       
       //sprintf(buffer, "flash setconf %sSC_SAVE_PROFILE 2", config_prefix);
       //system(buffer);
       //pCtx->sc_save_profile = 2;
       
       system("flash setconf end");
       return 1;
       
}

static int get_mib_value(int type, int vidx,unsigned char *value, unsigned char *mib_name, unsigned int is_string)
{
	FILE *stream;
	char line[256];
	char string[64];
	unsigned char cmd_buf[256];
	unsigned char *p;
	if(type ==0 || type ==1)
		if(vidx == 0)
			sprintf(cmd_buf, "flash get wlan%d %s > %s", type, mib_name, TEMP_MIB_FILE);
		else if(vidx == -1)
			sprintf(cmd_buf, "flash get wlan%d-vxd %s > %s", type, mib_name, TEMP_MIB_FILE);
		else
			sprintf(cmd_buf, "flash get wlan%d-va%d %s > %s", type,vidx, mib_name, TEMP_MIB_FILE);
	else
		sprintf(cmd_buf, "flash get %s > %s", mib_name, TEMP_MIB_FILE);
	system(cmd_buf);
	stream = fopen (TEMP_MIB_FILE, "r" );
	if ( stream != NULL ) {		
		if(fgets(line, sizeof(line), stream))
		{
			sscanf(line, "%*[^=]=%[^\n]", string);
			p=string;
			while(*p == ' ')
				p++;
			if((is_string == 1))
				p++;	//skip "
			strcpy(value, p);
			if(is_string == 1)
			{
				value[strlen(value)-1] = '\0';
			}
		}
		fclose(stream );	
	}	
	return 0;
}

#ifdef CONFIG_HCD_FLASH_SUPPORT
static int fwChecksumOk(char *data, int len)
{
        unsigned short sum=0;
        int i;

        for (i=0; i<len; i+=2) {
#ifdef _LITTLE_ENDIAN_
                sum += WORD_SWAP( *((unsigned short *)&data[i]) );
#else
                sum += *((unsigned short *)&data[i]);
#endif

        }
        return( (sum==0) ? 1 : 0);
}

void rtk_check_flash_mem(int linuxOffset,int linuxlen,int webOffset,int weblen,int rootOffset,int rootlen,char* upload_data,
						int webhead,int roothead,int linuxhead)
{
	int fh;
	char *flash_mem = NULL;

	/* mtdblock0 */
	fh = open(FLASH_DEVICE_KERNEL, O_RDWR);
	if(fh == -1)
	{
		firm_upgrade_status |= FLASHDATA_ERROR;
		return;
	}

	/* linux */
	if(linuxOffset  != -1)
	{
		lseek(fh, linuxOffset, SEEK_SET);
		flash_mem = (char*)malloc(linuxlen);
		if(flash_mem == NULL)
		{
			firm_upgrade_status |= FLASHDATA_ERROR;
			close(fh);
			return;
		}

		read(fh,flash_mem,linuxlen);
		if(memcmp(&(upload_data[linuxhead]),flash_mem,linuxlen) != 0)
		{
			firm_upgrade_status |= LINUXDATA_ERROR;
		}
		free(flash_mem);
		flash_mem == NULL;
	}

	/* web */
	if(webOffset != -1)
	{
		lseek(fh, webOffset, SEEK_SET);
		flash_mem = (char*)malloc(weblen);
		if(flash_mem == NULL)
		{
			firm_upgrade_status |= FLASHDATA_ERROR;
			close(fh);
			return;
		}

		read(fh,flash_mem,weblen);
		if(memcmp(&(upload_data[webhead]),flash_mem,weblen) != 0)
		{
			firm_upgrade_status |= WEBDATA_ERROR;
		}
		free(flash_mem);
		flash_mem == NULL;
	}
	close(fh);

	/* mtdblock1 */
	/* root */
	fh = open(FLASH_DEVICE_ROOTFS, O_RDWR);
	if(fh == -1)
	{
		firm_upgrade_status |= FLASHDATA_ERROR;
		return;
	}
	
	if(rootOffset != -1)
	{
		lseek(fh, rootOffset, SEEK_SET);
		flash_mem = (char*)malloc(rootlen);
		if(flash_mem == NULL)
		{
			firm_upgrade_status |= FLASHDATA_ERROR;
			close(fh);
			return;
		}

		read(fh,flash_mem,rootlen);
		if(memcmp(&(upload_data[roothead]),flash_mem,rootlen) != 0)
		{
			firm_upgrade_status |= ROOTDATA_ERROR;
		}
		free(flash_mem);
		flash_mem == NULL;
	}

	close(fh);
}


void rtk_all_interface(int action)
{
	switch(action)
	{
	case 0:
	system("ifconfig ppp0 down> /dev/console");
	system("ifconfig eth0 down> /dev/console");
	system("ifconfig eth1 down> /dev/console");
	system("ifconfig eth2 down> /dev/console");
	system("ifconfig eth3 down> /dev/console");
	system("ifconfig eth4 down> /dev/console");
	system("ifconfig wlan0 down> /dev/console");
	system("ifconfig wlan1 down> /dev/console");

	break;
	case 1:
	system("ifconfig eth0 up > /dev/console");
	system("ifconfig eth1 up > /dev/console");
	system("ifconfig eth2 up > /dev/console");
	system("ifconfig eth3 up > /dev/console");
	system("ifconfig eth4 up> /dev/console");
	system("ifconfig wlan0 up> /dev/console");
	system("ifconfig wlan1 up> /dev/console");
	system("ifconfig ppp0 up> /dev/console");

	break;
	default:
	printf("error rtk_all_interface\n");
	break;
	}

}

int rtk_FirmwareCheck(char *upload_data, int upload_len)
{
	int head_offset=0 ;
	int isIncludeRoot=0;
	int		 len;
    int          locWrite;
    int          numLeft;
    int          numWrite;
    IMG_HEADER_Tp pHeader;
	int flag=0, startAddr=-1, startAddrWeb=-1;	
    int fh;
   	unsigned char buffer[200];
   	firm_upgrade_status = 0;
   	int webOffset = -1,weblen,rootOffset = -1,rootlen,linuxOffset = -1,linuxlen;
	int webhead,roothead,linuxhead;

	//printf("[%s]:%d\n",__func__,__LINE__);

	int fwSizeLimit = 0x800000;
	
while(head_offset <   upload_len) {
    locWrite = 0;
    pHeader = (IMG_HEADER_Tp) &upload_data[head_offset];
    len = pHeader->len;
    numLeft = len + sizeof(IMG_HEADER_T) ;

    
    // check header and checksum
    if (!memcmp(&upload_data[head_offset], FW_HEADER, SIGNATURE_LEN) ||
			!memcmp(&upload_data[head_offset], FW_HEADER_WITH_ROOT, SIGNATURE_LEN))
    	flag = 1;
    else if (!memcmp(&upload_data[head_offset], WEB_HEADER, SIGNATURE_LEN))
    	flag = 2;
    else if (!memcmp(&upload_data[head_offset], ROOT_HEADER, SIGNATURE_LEN)){
    	flag = 3;
    	isIncludeRoot = 1;
    }	
#if 0		
	else if ( !memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) )	 {
		int type, status, cfg_len;
		cfg_len = updateConfigIntoFlash(&upload_data[head_offset], 0, &type, &status);
		
		if (status == 0 || type == 0) { // checksum error
			strcpy(buffer, "Invalid configuration file!");
			goto ret_upload;
		}
		else { // upload success
			strcpy(buffer, "Update successfully!");
			head_offset += cfg_len;
			update_cfg = 1;
		}    	
		continue;
	}
#endif	
    else {
       	strcpy(buffer, "Invalid file format!");
    	//winfred_wang
    	firm_upgrade_status |= SIGNATURE_ERROR;
		goto ret_upload;
    }

       if(len > fwSizeLimit){ //len check by sc_yang 
      		sprintf(buffer, "Image len exceed max size 0x%x ! len=0x%x",fwSizeLimit, len);
		goto ret_upload;
    }
    if ( (flag == 1) || (flag == 3)) {
    	if ( !fwChecksumOk(&upload_data[sizeof(IMG_HEADER_T)+head_offset], len)) {
      		sprintf(buffer, "Image checksum mismatched! len=0x%x, checksum=0x%x", len,
			*((unsigned short *)&upload_data[len-2]) );
			if(flag == 1)
				firm_upgrade_status |= LINUXCHECKSUM_ERROR;
			else if(flag == 3)
				firm_upgrade_status |= ROOTCHECKSUM_ERROR;
				
		goto ret_upload;
		}
    }
    else {
    	char *ptr = &upload_data[sizeof(IMG_HEADER_T)+head_offset];
    	if ( !CHECKSUM_OK(ptr, len) ) {
     		sprintf(buffer, "Image checksum mismatched! len=0x%x", len);
     		firm_upgrade_status |= WEBCHECKSUM_ERROR;
		goto ret_upload;
		}
    }

	head_offset += len + sizeof(IMG_HEADER_T) ;
	startAddr = -1 ; //by sc_yang to reset the startAddr for next image	
    }
	return 0;

 ret_upload:	
  	fprintf(stderr, "%s\n", buffer);	
	return -1;	
}

int rtk_FirmwareUpgrade(char *upload_data, int upload_len)
{
	int head_offset=0 ;
	int isIncludeRoot=0;
	int		 len;
    int          locWrite;
    int          numLeft;
    int          numWrite;
    IMG_HEADER_Tp pHeader;
	int flag=0, startAddr=-1, startAddrWeb=-1;	
    int fh;
   	unsigned char buffer[200];
   	firm_upgrade_status = 0;
   	int webOffset = -1,weblen,rootOffset = -1,rootlen,linuxOffset = -1,linuxlen;
	int webhead,roothead,linuxhead;

	//printf("[%s]:%d\n",__func__,__LINE__);

	int fwSizeLimit = 0x800000;

	/* do interface down */
	rtk_all_interface(0);
	
	
while(head_offset <   upload_len) {
    locWrite = 0;
    pHeader = (IMG_HEADER_Tp) &upload_data[head_offset];
    len = pHeader->len;
    numLeft = len + sizeof(IMG_HEADER_T) ;

    
    // check header and checksum
    if (!memcmp(&upload_data[head_offset], FW_HEADER, SIGNATURE_LEN) ||
			!memcmp(&upload_data[head_offset], FW_HEADER_WITH_ROOT, SIGNATURE_LEN))
    	flag = 1;
    else if (!memcmp(&upload_data[head_offset], WEB_HEADER, SIGNATURE_LEN))
    	flag = 2;
    else if (!memcmp(&upload_data[head_offset], ROOT_HEADER, SIGNATURE_LEN)){
    	flag = 3;
    	isIncludeRoot = 1;
    }	
#if 0		
	else if ( !memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) )	 {
		int type, status, cfg_len;
		cfg_len = updateConfigIntoFlash(&upload_data[head_offset], 0, &type, &status);
		
		if (status == 0 || type == 0) { // checksum error
			strcpy(buffer, "Invalid configuration file!");
			goto ret_upload;
		}
		else { // upload success
			strcpy(buffer, "Update successfully!");
			head_offset += cfg_len;
			update_cfg = 1;
		}    	
		continue;
	}
#endif	
    else {
       	strcpy(buffer, "Invalid file format!");
    	//winfred_wang
    	firm_upgrade_status |= SIGNATURE_ERROR;
		goto ret_upload;
    }

	/*do checksum in rtk_FirmwareCheck */
	#if 0
       if(len > fwSizeLimit){ //len check by sc_yang 
      		sprintf(buffer, "Image len exceed max size 0x%x ! len=0x%x",fwSizeLimit, len);
		goto ret_upload;
    }
    if ( (flag == 1) || (flag == 3)) {
    	if ( !fwChecksumOk(&upload_data[sizeof(IMG_HEADER_T)+head_offset], len)) {
      		sprintf(buffer, "Image checksum mismatched! len=0x%x, checksum=0x%x", len,
			*((unsigned short *)&upload_data[len-2]) );
			if(flag == 1)
				firm_upgrade_status |= LINUXCHECKSUM_ERROR;
			else if(flag == 3)
				firm_upgrade_status |= ROOTCHECKSUM_ERROR;
				
		goto ret_upload;
	}
    }
    else {
    	char *ptr = &upload_data[sizeof(IMG_HEADER_T)+head_offset];
    	if ( !CHECKSUM_OK(ptr, len) ) {
     		sprintf(buffer, "Image checksum mismatched! len=0x%x", len);
     		firm_upgrade_status |= WEBCHECKSUM_ERROR;
		goto ret_upload;
	}
    }
	#endif


    if(flag == 3)
    	fh = open(FLASH_DEVICE_ROOTFS, O_RDWR);
    else
    fh = open(FLASH_DEVICE_KERNEL, O_RDWR);

    if ( fh == -1 ) {
       	strcpy(buffer, "File open failed!");
		goto ret_upload;
    } else {

	if (flag == 1) {
		if ( startAddr == -1){
			//startAddr = CODE_IMAGE_OFFSET;
			startAddr = pHeader->burnAddr ;
		}
		linuxOffset = startAddr;
	}
	else if (flag == 3) {
		if ( startAddr == -1){
			startAddr = 0; // always start from offset 0 for 2nd FLASH partition
		}
		rootOffset = startAddr;
	}
	else {
		if ( startAddrWeb == -1){
			//startAddr = WEB_PAGE_OFFSET;
			startAddr = pHeader->burnAddr ;
		}
		else
			startAddr = startAddrWeb;

		webOffset = startAddr;
	}
	lseek(fh, startAddr, SEEK_SET);
	if(flag == 3){
		fprintf(stderr,"\r\n close all interface");		
		locWrite += sizeof(IMG_HEADER_T); // remove header
		numLeft -=  sizeof(IMG_HEADER_T);
		//kill_processes();
		//sleep(2);
	}	
	//fprintf(stderr,"\r\n flash write");	

	//winfred_wang
	if(flag == 1){
		linuxlen = numLeft;
		linuxhead = locWrite+head_offset;
	}else if(flag == 2){
		weblen  = numLeft;
		webhead = locWrite+head_offset;
	}else if(flag == 3){
		rootlen = numLeft;
		roothead = locWrite+head_offset;
	}
	
	numWrite = write(fh, &(upload_data[locWrite+head_offset]), numLeft);
	//numWrite = numLeft;
	/*sprintf(buffer,"write flash flag=%d,locWrite+head_offset=%d,numLeft=%d,startAddr=%x\n",
				flag,locWrite+head_offset,numLeft,startAddr);
	fprintf(stderr, "%s\n", buffer);
	hex_dump(&(upload_data[locWrite+head_offset]),16);*/

	if (numWrite < numLeft) {

		sprintf(buffer, "File write failed. locWrite=%d numLeft=%d numWrite=%d Size=%d bytes.", locWrite, numLeft, numWrite, upload_len);

	goto ret_upload;
	}

	locWrite += numWrite;
 	numLeft -= numWrite;
	sync();
#ifdef KERNEL_3_10
	if(ioctl(fh,BLKFLSBUF,NULL) < 0){
		printf("flush mtd system cache error\n");
	}
#endif
	close(fh);

	head_offset += len + sizeof(IMG_HEADER_T) ;
	startAddr = -1 ; //by sc_yang to reset the startAddr for next image	
    }
} //while //sc_yang 
	//winfred_wang 
	rtk_check_flash_mem(linuxOffset,linuxlen,webOffset,weblen,rootOffset,rootlen,upload_data,webhead,roothead,linuxhead);

	/* ifconfig interface up */
	rtk_all_interface(1);
	
  return 0;
  ret_upload:	
  	fprintf(stderr, "%s\n", buffer);	

	rtk_all_interface(1);
  	
	return -1;
}

static int _firm_upgrade(char *cmd , int cmd_len)
{
	int fd,ret=0;
	unsigned char cmd2[2];
	ret=rtk_FirmwareCheck(cmd,cmd_len);
	if(ret < 0 ) {
		/*let do_cmd issue failed rsp*/
		return ret;
	}
	else
	{
		ret=MAX_INBAND_PAYLOAD_LEN+1;
		memset(cmd2,0x0,sizeof(cmd2));
	}

	//winfred_wang
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
	rgb_led_blink_setTriggerMode(FLASH_TRIGGER);
#endif
		
	//printf("[%s]:%d\n",__func__,__LINE__);
	rtk_FirmwareUpgrade(cmd,cmd_len);		

	//winfred_wang
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
	rgb_led_blink_setTriggerMode(TIMER_TRIGGER);
#endif

	//autoreboot;
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
	ret = rgb_led_blink_reboot_check();
	if(!ret)
		system("reboot");
#else
	system("reboot");
#endif

	return ret;
}

static int _firm_check_signature_checksum(char *cmd , int cmd_len)
{
	char tmpBuf[128] = {0};
	int len = 0;
	//printf("[%s]:%d\n",__func__,__LINE__);

	//printf("firm_upgrade_status=%x\n",firm_upgrade_status);
	
	if((firm_upgrade_status & 0xff) != 0)
	{
		if(firm_upgrade_status & SIGNATURE_ERROR)
			len += snprintf(tmpBuf+len,128 -len,"signature error\n");
		if(firm_upgrade_status & WEBCHECKSUM_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"web checksum error\n");
		if(firm_upgrade_status & ROOTCHECKSUM_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"root checksum error\n");
		if(firm_upgrade_status & LINUXCHECKSUM_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"linux checksum error\n");
	}else{
		len += snprintf(tmpBuf+len,128-len,"firmware signature checksum correct\n");
	}

	

	memcpy(cmd,tmpBuf,len);
	return len;
}


static int _firm_check_flash_data(char *cmd , int cmd_len)
{
	char tmpBuf[128] = {0};
	int len = 0;
	//printf("[%s]:%d\n",__func__,__LINE__);
	
	if((firm_upgrade_status & 0xff00) != 0)
	{
		if(firm_upgrade_status & WEBDATA_ERROR)
			len += snprintf(tmpBuf+len,128 -len,"web data error\n");
		if(firm_upgrade_status & LINUXDATA_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"linux data error\n");
		if(firm_upgrade_status & ROOTDATA_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"root data error\n");
		if(firm_upgrade_status & FLASHDATA_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"other error\n");
	}else{
		len += snprintf(tmpBuf+len,128-len,"firmware data correct\n");
	}

	

	memcpy(cmd,tmpBuf,len);
	return len;
}

static int _firm_upgrade_reboot(char *cmd , int cmd_len)
{
	//printf("[%s]:%d\n",__func__,__LINE__);
	system("reboot");
	return 0;
}

#endif


static int host_ioctl_receive(char *ioctl_data_p,int ioctl_data_len)
{
	int rx_len=0;
	int ret=-1;
	struct iwreq iwr;
	struct ifreq ifr;
	struct iw_point *data;
	int ioctl_op=0;
	int error_code=0;
	int data_len=0;
	char *data_ptr;
	int opt_len=0;	
	unsigned char *is_iwreq;
	unsigned char *use_pointer;
	//int reply=1; //good reply
			 

	 //rcv ioctl succefully
	 memcpy(&ioctl_op,ioctl_data_p,sizeof(int));

	 is_iwreq = ioctl_data_p+HDR_INFO_OFFSET;
	 use_pointer = is_iwreq + 1;

	 //printf("%s print_len=%d\n",__FUNCTION__,ioctl_data_len);
	 //hex_dump(ioctl_data_p, ioctl_data_len);

	 if( !(*is_iwreq) )//use ifreq
	 {
		//printf("ifreq op_code = %x \n",ioctl_op);
		data_ptr = (char *)&ifr;
		data_len = sizeof(struct ifreq);
			   memcpy(data_ptr,ioctl_data_p+HDR_IOCTL_DATA_OFFSET,data_len);
	 }	 
	 else
	 {	
		//printf("iwreq op_code = %x \n",ioctl_op);
		data_ptr = (char *)&iwr;
		data_len = sizeof(struct iwreq);
		memcpy(data_ptr,ioctl_data_p+HDR_IOCTL_DATA_OFFSET,data_len);
		
		//check iwreq data pointer
		if( *use_pointer )
		{
			  iwr.u.data.pointer = (caddr_t) (ioctl_data_p +HDR_IOCTL_DATA_OFFSET+data_len);
			  //printf("use data pointer ioh_obj_p->rx_data=%x, iwr.u.data.pointer=%x , iwr.u.data.length=%d!!!\n",ioctl_data_p,iwr.u.data.pointer,iwr.u.data.length);
			  opt_len = iwr.u.data.length;
		}		

		if( ioctl_op == 0xffffffff ){
			int n=0;
			struct ifreq ifr;
			struct sockaddr_ll addr;
			int ifindex;
			int sock=-1;

			sock = socket(PF_PACKET, SOCK_RAW, 0);
			if (socket < 0) {
				perror("socket[PF_PACKET,SOCK_RAW]");
				return -1;
			}	
			memset(&ifr, 0, sizeof(ifr));
			strncpy(ifr.ifr_name, "wlan0", sizeof(ifr.ifr_name));

			while (ioctl(ioctl_sock , SIOCGIFINDEX, &ifr) != 0) {
				printf(" ioctl(SIOCGIFINDEX) failed!\n");
				sleep(1);
			}
			ifindex = ifr.ifr_ifindex;	
			memset(&addr, 0, sizeof(addr));
			addr.sll_family = AF_PACKET;
			addr.sll_ifindex = ifindex;
			if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
				perror("bind[PACKET]");
				return -1;
			}
			
		 	//send wlan
		 	if ((n = send(sock, iwr.u.data.pointer, iwr.u.data.length, 0)) < 0) {
				printf("send_wlan failed!");
				return -1;
			}

			close(sock);
			return n;
		 }
	 }

	 if( ioctl_sock < 0 ) {
	 	ioctl_sock = socket(PF_INET, SOCK_DGRAM, 0);
		if (ioctl_sock < 0) {
			printf("\nioctl socket open failed !!!!!!!!!!!!\n");
			perror("socket[PF_INET,SOCK_DGRAM]");
			return -1;
		}
	}

	 if (ioctl(ioctl_sock, ioctl_op, data_ptr) < 0) {
		printf("\nioctl fail  ioctl_op=%x!!!!!!!!!!!!\n",ioctl_op);
		error_code = -1;
		//reply =2 ; //bad reply
		ret = 2;
	}

	//send reply		
	//copy erro code to reply (now in rx_data)
	if( *use_pointer ) {
		//printf("%s %d iwr.u.data.length:%d\n",__FUNCTION__,__LINE__,iwr.u.data.length);
		memcpy(ioctl_data_p,(char *)&error_code,sizeof(int));	
		memcpy(ioctl_data_p+HDR_IOCTL_DATA_OFFSET+sizeof(iwr),iwr.u.data.pointer,iwr.u.data.length);	//mark_test 		ret = iwr.u.data.length;
		ioctl_data_len += iwr.u.data.length;
	} else {
		memcpy(ioctl_data_p,(char *)&error_code,sizeof(int));
		memcpy(ioctl_data_p+HDR_IOCTL_DATA_OFFSET,data_ptr,data_len);	//mark_test	
	}

	//return ret;
	/*
	 if( ev_tools_inband_ioctl_chan < 0 ) {
	 	ev_tools_inband_ioctl_chan = inband_open(LOCAL_NETIF,NULL,ETH_P_RTK_NOTIFY,0);

		if(ev_tools_inband_ioctl_chan < 0)
		{
		       printf(" inband_open failed!\n");
		        return -1;
		}
	}*/
	return ret;
}

static int host_systemcall_receive(char *cmd , int cmd_len) //mark_cmd
{
	char *param;		
	char *tmp;
	FILE *fp;
	int resp_len=0;
	char res[64*200]; //mark_issue
	char tmp_cmd[cmd_len+1];
	int retry_count=0;
	
	cmd[cmd_len]='\0';//mark_patch
	memset(res,0,sizeof(res));
	if(strstr(cmd,"init.sh") || strstr(cmd,"reboot"))
	{
		strcpy(tmp_cmd,cmd);
		res[0] = '\0';
		sleep(1);
		system(tmp_cmd);
		return MAX_INBAND_PAYLOAD_LEN+1;
	}
	else if(strstr(cmd,"sysconf"))
	{
		strcpy(tmp_cmd,cmd);
		res[0] = '\0';
		system(tmp_cmd);
		return MAX_INBAND_PAYLOAD_LEN+1;
	}
RETRY_SYSTEM_CALL:	
    //printf("system call = %s\n",data);
	fp = popen(cmd, "r");
	if (fp)
	{
		while (!feof(fp)&&resp_len+MAX_INBAND_PAYLOAD_LEN<sizeof(res)) 
		{
			// 4 bytes for FCS
			//if (resp_len + sizeof(*obj->rx_header) + 4 == sizeof(obj->rx_buffer))
			//break;	// out of buffer

			resp_len += fread(	&res[resp_len],
							sizeof(char), 
							MAX_INBAND_PAYLOAD_LEN, 
							fp);
		}
	}
	else
	{
		printf("popen error!!!\n");
		return -1; //error reply in do_cmd
	}
	/*retry more for rebust inband cmd execution*/
	if(resp_len <= 0){
		if (fp)
		{
			pclose(fp);
		}
		if(retry_count++ < 3){
			//system call fail, do cmd again
			printf("host_systemcall_receive: len=%d retry system call\n",resp_len);
			goto RETRY_SYSTEM_CALL;
		}
		else
		{
			return -1;
		}
	}	
	pclose(fp);
	//mark_issue , always syscmd reply here so , ret set to MAX_INBAND_PAYLOAD_LEN +1
	return MAX_INBAND_PAYLOAD_LEN+1;
}
static int _request_scan(char *cmd , int cmd_len)
{
	int status;
	char errbuf[100];
	cmd[cmd_len] = '\0';
	if(getWlSiteSurveyRequest(cmd, &status) < 0)
		sprintf(errbuf,"Scan request failed!\n");
	else
		sprintf(errbuf,"Auto Scan running!\n");
	printf("%s",errbuf);
	return MAX_INBAND_PAYLOAD_LEN+1;

}
static int _get_scan_result(char *cmd , int cmd_len)
{	
  char errBuf[100]; //per bssinfo or error msg  
  char tmpbuf[100],tmpbuf2[50]; //per bssinfo or error msg
  char ssr_result[MAX_BSS_DESC*200]; //mark_issue , use alloc ?
  SS_STATUS_Tp pStatus=NULL;
  BssDscr *pBss;
  int i,wait_time =0;
  cmd[cmd_len] = '\0';
  if (pStatus==NULL) {
		pStatus = calloc(1, sizeof(SS_STATUS_T));
		if ( pStatus == NULL ) {
			strcpy(errBuf, "Allocate buffer failed!\n");
			goto ssr_err_out;
		}
  }

  pStatus->number = 0; // request BSS DB
  while(getWlSiteSurveyResult(cmd, pStatus) < 0){
	  if ( wait_time++ < 2 ) {
	  		sleep(1);
	  }
	  else{
	  	strcpy(errBuf, "Read site-survey status failed!\n");
			goto ssr_err;
	  }
  }

  if(pStatus->number<0 || pStatus->number > MAX_BSS_DESC)
  {
  	strcpy(errBuf, "invalid scanned ap num!\n");
  	goto ssr_err;
  }	

  sprintf(ssr_result,"total_ap_num=%d\n",pStatus->number);  

  for (i=0; i<pStatus->number && pStatus->number!=0xff; i++) {

		//add ##########################
		strcat(ssr_result,"##########\n");

		pBss = &pStatus->bssdb[i];		
		// fill ap index
		sprintf(tmpbuf,"ap_index=%d\n",i+1);
		strcat(ssr_result,tmpbuf);

		// fill ap ssid
   		
		memcpy(tmpbuf2, pBss->bdSsIdBuf, pBss->bdSsId.Length);
		tmpbuf2[pBss->bdSsId.Length] = '\0';
		
		sprintf(tmpbuf,"ap_ssid=%s\n",tmpbuf2);		
		strcat(ssr_result,tmpbuf);
   
		// fill ap mac
		sprintf(tmpbuf,"ap_bssid=%02x:%02x:%02x:%02x:%02x:%02x\n",
			pBss->bdBssId[0], pBss->bdBssId[1], pBss->bdBssId[2],
			pBss->bdBssId[3], pBss->bdBssId[4], pBss->bdBssId[5]);
		strcat(ssr_result,tmpbuf);
		
		// fill ap mode B,G,N
		get_ap_mode(pBss->network,tmpbuf2);
		sprintf(tmpbuf,"ap_mode=%s\n",tmpbuf2);		
		strcat(ssr_result,tmpbuf);
	
		// fill ap channel
		sprintf(tmpbuf,"ap_channel=%d\n",pBss->ChannelNumber);
		strcat(ssr_result,tmpbuf);
	
		// fill ap encryption type
		get_ap_encrypt(pBss,tmpbuf2);
		sprintf(tmpbuf,"ap_encrypt=%s\n",tmpbuf2);		
		strcat(ssr_result,tmpbuf);
	
		// fill ap signal strength
		sprintf(tmpbuf,"ap_signal=%d\n",pBss->rssi);
		strcat(ssr_result,tmpbuf);

  }		

 	
  	free(pStatus); 
	printf("%s",ssr_result);
	return MAX_INBAND_PAYLOAD_LEN+1;
ssr_err :
	free(pStatus); 
ssr_err_out :
	printf("%s",errBuf);
	return -1;
}

static int _ev_get_firmware_version(char *cmd , int cmd_len)
{
	printf("[%s:%d]\n", __FUNCTION__,__LINE__);
	int ret = 1,length;
	unsigned char errbuff[48] = {0};
	char *version = (char *)malloc(FW_VERSION_MAX_LEN);
	if(version == NULL){
		strcpy(errbuff, "error,not enough memory\n");
		return (MAX_INBAND_PAYLOAD_LEN + 1);
	}

        if(version == NULL)
		ret = 0;
	
	if (ret == 0)
	{
		strcpy(errbuff, "error,rtk_get_firmware_version failed\n");
		goto ssr_err_out;
	} else {
	    version[0] = 0;
	    strcpy(version, fwVersion);
	    printf("version number: %s\n", version);
	}
	
	length = strlen(version);
	printf("%s.%d.return to client. firmware_version %s\n",__FUNCTION__,__LINE__, version);
	free(version);
	return (MAX_INBAND_PAYLOAD_LEN + 1);

ssr_err_out :
	printf("%s",errbuff);
	free(version);
	return (MAX_INBAND_PAYLOAD_LEN + 1);
}

static int _set_mib(char *cmd , int cmd_len)
{
	char *param,*val,*next;	
	int ret = RET_OK,access_flag=ACCESS_MIB_SET | ACCESS_MIB_BY_NAME;
	char *intf, *tmp;

	cmd[cmd_len]='\0';//mark_patch
	do {
		intf = cmd;
		param = strchr(intf,'_')+1;
		if( !strchr(param,'=') ){
			printf("invalid command format:%s\n",cmd);
			return -1;
		}
		val = strchr(param,'=')+1;

		intf[param-intf-1] = '\0';
		param[val-param-1] = '\0';

		if( !strcmp(intf,"wps") )
			break;

		cmd = strchr(val,'\n');
		if(cmd)
		    *cmd = '\0';
		printf(">>> set %s=%s to %s\n",param,val,intf);
		ret = access_config_mib(access_flag,param,val,intf);  //ret the

		if(cmd && (*(cmd+1) != '\0')) 
		    cmd++;
		else
			break;	
	} while(cmd);

	//free(intf);
	return ret;
}

int config_from_local(unsigned char *ptr)
{
	int fh = 0;
	struct stat status;

	if (stat(CFG_FILE_IN, &status) < 0) {
		printf("stat() error [%s]!\n", CFG_FILE_IN);
		return -1;
	}

	fh = open(CFG_FILE_IN,O_RDONLY);
	if( fh < 0 ){
		printf("File open failed\n");
		return -1;
	}

	/*
	ptr = (unsigned char *)calloc(0,status.st_size);
	if( !ptr ){
		printf("%d:memory alloc failed\n",status.st_size);
		return -1;
	}
	*/
	lseek(fh, 0L, SEEK_SET);

	if (read(fh, ptr, status.st_size) != status.st_size) {		
		printf("read() error [%s]!\n", CFG_FILE_IN);
		return -1;	
	}
	close(fh);
	return 0;
}

int parse_then_set(unsigned char *str_start, unsigned char *str_end)
{
	int ret;
	unsigned char *param, *value, *intf, line[100] = {0};

	memcpy(line,str_start,str_end-str_start);
	param = line;
	intf = param;
	param = strchr(intf,'_')+1;
	value = strchr(param,'=')+1;
	intf[param-intf-1] = '\0';
	param[value-param-1] = '\0';
	printf(">>> %s %s %s = %s \n",__FUNCTION__,intf,param,value);
	ret = access_config_mib(ACCESS_MIB_BY_NAME|ACCESS_MIB_SET,param,value,intf);
	return ret;
}

static int _set_mibs(char *cmd , int cmd_len)
//static int _cfgswrite(void)
{
	unsigned char cmd_buf[20480] = {0};
	unsigned char line[100] = {0}, *str_start, *str_end;
	cmd[cmd_len]='\0';//mark_patch
	if( config_from_local(cmd_buf) < 0 ) {
		printf("Read config from %s failed\n","/etc/Wireless/realtekap.conf");
		return -1;
	}

	str_start = cmd_buf;
	str_end = strchr(cmd_buf,'\n');
	while( str_end ){
		if( *str_start != '#' && *str_start != ' ' && *str_start != '\n' ){
			parse_then_set(str_start,str_end);
		}
		str_start = str_end+1;
		str_end = strchr(str_start,'\n');
	}

	free(cmd_buf);
	return 0;
}


static int _get_mib(char *cmd , int cmd_len)
{
	char *param;	
	int ret = RET_OK,access_flag=ACCESS_MIB_GET | ACCESS_MIB_BY_NAME;
	char *intf, *tmp;
	cmd[cmd_len]='\0';//mark_patch
	intf = cmd;
	param = strchr(intf,'_')+1;
	if(param<=1)
	{
		DEBUG_ERR("Invalid mib name!\n");
		return -1;
	}
		
	intf[param-intf-1] = '\0';
	//printf(">>> read %s from %s\n",param,intf);
	ret = access_config_mib(access_flag,param,cmd,intf);  //return value in cmd

	//free(intf);
	return ret;
}

static int _sysinit(char *cmd , int cmd_len)
{
	//now , only support init all
	cmd[cmd_len]='\0';//mark_patch	
	if(!strcmp(cmd, "all"))
		init_system(INIT_ALL);		
	else if(!strcmp(cmd, "lan"))		
		init_system(INIT_ALL);
	else if(!strcmp(cmd, "wlan"))
		init_system(INIT_ALL);
	else
		return -1;

	return 0;
}
//sta_info frame , {sta_num}{sta_info1}{sta_info2}........... 
//first byre will be total sta_info numer in this reply
static int _getstainfo(char *cmd , int cmd_len)
{
	char *buff,*tmpInfo;	
	WLAN_STA_INFO_Tp pInfo;
	int sta_num =0,i,ret;
	cmd[cmd_len]='\0';//mark_patch
	buff = calloc(1, sizeof(WLAN_STA_INFO_T) * (MAX_STA_NUM+1));
	tmpInfo = cmd +1 ; // first byte reserve for sta_info num

	if ( buff == 0 ) {
		printf("Allocate buffer failed!\n");
		return -1;
	}

	if ( get_wlan_stainfo(cmd,  (WLAN_STA_INFO_Tp)buff ) < 0 ) {
		printf("Read wlan sta info failed!\n");

		return -1;
	}

	for (i=1; i<=MAX_STA_NUM; i++) {
		pInfo = (WLAN_STA_INFO_Tp)&buff[i*sizeof(WLAN_STA_INFO_T)];
		if (pInfo->aid && (pInfo->flag & STA_INFO_FLAG_ASOC)) {//sta exist
			memcpy(tmpInfo+ (sta_num*sizeof(WLAN_STA_INFO_T)),(char *)pInfo, sizeof(WLAN_STA_INFO_T));
			sta_num++;
		}		
	}	

	cmd[0]= (unsigned char)(sta_num&0xff);
	ret = sta_num*sizeof(WLAN_STA_INFO_T) + 1;		
	
	return ret;
}
static int _getassostanum(char *cmd , int cmd_len)
{
	int num=0;
	cmd[cmd_len]='\0';//mark_patch
	if (get_wlan_stanum(cmd, &num) < 0)
		return -1;

	cmd[0]=(unsigned char)(num&0xff);
	cmd[1]= '\0';

	return 1; // return len=1  to show sta num
	
}
static int _getbssinfo(char *cmd , int cmd_len)
{	
	WLAN_BSS_INFO_T bss;
	int bss_len=sizeof(WLAN_BSS_INFO_T);
	cmd[cmd_len]='\0';//mark_patch
	if ( get_wlan_bssinfo(cmd, &bss) < 0)
			return -1;

	memcpy(cmd,(char *)&bss,bss_len);
	
	return bss_len;
}

int get_lanport_stats(int portnum,struct port_statistics *port_stats)
{
	struct ifreq ifr;
	 int sockfd;
	 char *name="eth0";	 
	 struct port_statistics stats;
	 unsigned int *args;	

	 if(portnum > 5)
	 	return -1;	 	
	 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
 	{
      		printf("fatal error socket\n");
      		return -3;
       }
	args = (unsigned int *)&stats;
	((unsigned int *)(&ifr.ifr_data))[0] =(struct port_statistics *)&stats;
	*args = portnum;
	
	strcpy((char*)&ifr.ifr_name, name);       

    if (ioctl(sockfd, RTL819X_IOCTL_READ_PORT_STATS, &ifr)<0)
    {
      		printf("device ioctl:");
      		close(sockfd);
     		 return -1;
     }
     close(sockfd);   	     
     memcpy(port_stats,(char *)&stats,sizeof(struct port_statistics));

    return 0;	 

}

int get_lanport_status(int portnum,struct lan_port_status *port_status)
{
	struct ifreq ifr;
	 int sockfd;
	 char *name="eth0";	 
	 struct lan_port_status status;
	 unsigned int *args;	

	 if(portnum > 5)
	 	return -1;	 	
	 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
 	{
      		printf("fatal error socket\n");
      		return -3;
    }
	args = (unsigned int *)&status;
	((unsigned int *)(&ifr.ifr_data))[0] =(struct lan_port_status *)&status;
	*args = portnum;
	
	strcpy((char*)&ifr.ifr_name, name);       

    if (ioctl(sockfd, RTL819X_IOCTL_READ_PORT_STATUS, &ifr)<0)
    {
      		printf("device ioctl:");
      		close(sockfd);
     		 return -1;
     }
     close(sockfd);   	
     memcpy((char *)port_status,(char *)&status,sizeof(struct lan_port_status));

    return 0;	 

}
int get_lanport_rate(int portnum,struct port_rate *portRate)
{
#if 1
	

 	FILE *stream;
  	char buffer[512];
	char* ptr;
	int port=-1;
	unsigned int rx_rate=0,tx_rate=0;
	int index=0;
	struct port_rate *portRateInfo=NULL;
	unsigned int total_rx_rate=0;
	unsigned int total_tx_rate;
	int ret=0;
	//printf("portnum:%x,[%s]:[%d].\n",portnum,__FUNCTION__,__LINE__);
	stream = fopen (_RTL_PORT_RATE, "r" );
	if ( stream != NULL ) { 	
		while(fgets(buffer, sizeof(buffer), stream))
		{

			ptr = strstr(buffer, "port");
			if (ptr) 
			{
				ptr = ptr + 4;
				
				if (ptr)
				{	
					sscanf( ptr, "%d", &port );   
				#if 0
					printf("port:%x [%s]:[%d].\n",port,__FUNCTION__,__LINE__);
				#endif
				}
			}
			ptr = strstr(buffer, "rx:");
			if (ptr) 
			{
				ptr = ptr + 3;
				if (ptr)
				{
					
					sscanf(ptr, "%d", &rx_rate); 
					
					//printf("rx_rate:%d,[%s]:[%d].\n",rx_rate,__FUNCTION__,__LINE__);
				}
			}
			
			ptr = strstr(buffer, "tx:");
			if (ptr) 
			{
				ptr = ptr + 3;
				if (ptr)
				{
					
					sscanf(ptr, "%d", &tx_rate); 
					
					//printf("tx_rate:%d,[%s]:[%d].\n",tx_rate,__FUNCTION__,__LINE__);
				}
			}
			total_rx_rate+=rx_rate;
			total_tx_rate+=tx_rate;
			
			
			if(portnum >=0&&portnum<4)
			{
				if(port==portnum)
				{
					portRateInfo=portRate;
					if(portRateInfo==NULL){
						ret=-1;
						goto out;
					}		
					if((index<4)&&portRateInfo){
						portRateInfo->port_id =port;
						portRateInfo->rx_rate =rx_rate;
						portRateInfo->tx_rate =tx_rate;
						//printf("portnum:%x,%d,[%s]:[%d].\n",portnum,portRateInfo->port_id,
						//	portRateInfo->rx_rate,portRateInfo->tx_rate,__FUNCTION__,__LINE__);

						
					}
					
					break;
				}	
				
			}
			else if (portnum==0xff)
			{
				portRateInfo=portRate+index;
				if(portRateInfo==NULL){
					ret=-1;
					goto out;
				}	
				if((index<4)&&portRateInfo)
				{
					portRateInfo->port_id =port;
					portRateInfo->rx_rate =rx_rate;
					portRateInfo->tx_rate =tx_rate;
					//printf("[index]:%x,%d,[%s]:[%d].\n",portnum,portRateInfo->port_id,
					//	portRateInfo->rx_rate,portRateInfo->tx_rate,__FUNCTION__,__LINE__);
				}	
			}
			
			if(port!=-1)
				index++;
			
			port=-1;
			rx_rate=0;
			tx_rate=0;
			portRateInfo =NULL;
		}
		for(index =0;index<4;index++)
		{
			
			if(portnum >=0&&portnum<4)
			{
				if(port==portnum)
				{
					if(total_rx_rate)
					portRateInfo->rx_percent=portRateInfo->rx_rate/total_rx_rate*100;
					if(total_tx_rate)
					portRateInfo->tx_percent=portRateInfo->tx_rate/total_tx_rate*100;
					
				}
				
			}
			else if (portnum==0xff)
			{
				portRateInfo=portRate+index;
				if(portRateInfo)
				{
					if(total_rx_rate)
					portRateInfo->rx_percent=portRateInfo->rx_rate/total_rx_rate*100;
					if(total_tx_rate)
					portRateInfo->tx_percent=portRateInfo->tx_rate/total_tx_rate*100;
				}	
			}
		}
		
	}
	else
	{
		ret =-1;
	}

#endif
out:
    return ret;	 

}

static int get_dev_fields(int type, char *bp, struct user_net_device_stats *pStats)
{
    switch (type) {
    case 3:
	sscanf(bp,
	"%Lu %Lu %lu %lu %lu %lu %lu %lu %Lu %Lu %lu %lu %lu %lu %lu %lu",
	       &pStats->rx_bytes,
	       &pStats->rx_packets,
	       &pStats->rx_errors,
	       &pStats->rx_dropped,
	       &pStats->rx_fifo_errors,
	       &pStats->rx_frame_errors,
	       &pStats->rx_compressed,
	       &pStats->rx_multicast,

	       &pStats->tx_bytes,
	       &pStats->tx_packets,
	       &pStats->tx_errors,
	       &pStats->tx_dropped,
	       &pStats->tx_fifo_errors,
	       &pStats->collisions,
	       &pStats->tx_carrier_errors,
	       &pStats->tx_compressed);
	break;

    case 2:
	sscanf(bp, "%Lu %Lu %lu %lu %lu %lu %Lu %Lu %lu %lu %lu %lu %lu",
	       &pStats->rx_bytes,
	       &pStats->rx_packets,
	       &pStats->rx_errors,
	       &pStats->rx_dropped,
	       &pStats->rx_fifo_errors,
	       &pStats->rx_frame_errors,

	       &pStats->tx_bytes,
	       &pStats->tx_packets,
	       &pStats->tx_errors,
	       &pStats->tx_dropped,
	       &pStats->tx_fifo_errors,
	       &pStats->collisions,
	       &pStats->tx_carrier_errors);
	pStats->rx_multicast = 0;
	break;

    case 1:
	sscanf(bp, "%Lu %lu %lu %lu %lu %Lu %lu %lu %lu %lu %lu",
	       &pStats->rx_packets,
	       &pStats->rx_errors,
	       &pStats->rx_dropped,
	       &pStats->rx_fifo_errors,
	       &pStats->rx_frame_errors,

	       &pStats->tx_packets,
	       &pStats->tx_errors,
	       &pStats->tx_dropped,
	       &pStats->tx_fifo_errors,
	       &pStats->collisions,
	       &pStats->tx_carrier_errors);
	pStats->rx_bytes = 0;
	pStats->tx_bytes = 0;
	pStats->rx_multicast = 0;
	break;
    }
    return 0;
}

static char *get_name(char *name, char *p)
{
    while (isspace(*p))
	p++;
    while (*p) {
	if (isspace(*p))
	    break;
	if (*p == ':') {	/* could be an alias */
	    char *dot = p, *dotname = name;
	    *name++ = *p++;
	    while (isdigit(*p))
		*name++ = *p++;
	    if (*p != ':') {	/* it wasn't, backup */
		p = dot;
		name = dotname;
	    }
	    if (*p == '\0')
		return NULL;
	    p++;
	    break;
	}
	*name++ = *p++;
    }
    *name++ = '\0';
    return p;
}


void fill_statistics(struct user_net_device_stats *pStats, struct port_statistics *statistics)
{
	statistics->rx_bropkts = 0;
	statistics->rx_bytes = pStats->rx_bytes;
	statistics->rx_discard = pStats->rx_dropped;
	statistics->rx_error = pStats->rx_errors;
	statistics->rx_mulpkts = pStats->rx_multicast;
	statistics->rx_unipkts = pStats->rx_packets-pStats->rx_multicast;
	statistics->tx_bropkts = 0;
	statistics->tx_bytes = pStats->tx_bytes;
	statistics->tx_discard = pStats->tx_dropped;
	statistics->tx_error = pStats->tx_errors;
	statistics->tx_mulpkts = 0;
	statistics->tx_unipkts = pStats->tx_packets;
}


int get_wlan_stats(char *intf, struct port_statistics *statistic)
{
 	FILE *fh;
  	char buf[512];
	int type;
	struct user_net_device_stats pStats;

	fh = fopen(_PATH_PROCNET_DEV, "r");
	if (!fh) {
		printf("Warning: cannot open %s\n",_PATH_PROCNET_DEV);
		return -1;
	}
	fgets(buf, sizeof buf, fh);	/* eat line */
	fgets(buf, sizeof buf, fh);

  	if (strstr(buf, "compressed"))
		type = 3;
	else if (strstr(buf, "bytes"))
		type = 2;
	else
		type = 1;

	while (fgets(buf, sizeof buf, fh)) {
		char *s, name[40];
		s = get_name(name, buf);
		if ( strcmp(intf, name))
			continue;
		get_dev_fields(type, s, &pStats);
		fill_statistics(&pStats,statistic);
		fclose(fh);
		return 0;
    }
	fclose(fh);
	return -1;
}


int portname_to_num(char *name)
{
	int portnum=0;
	if(!strncmp(name,"p0",2)) 
		portnum=0;
	else if(!strncmp(name,"p1",2)) 
		portnum=1;
	else if(!strncmp(name,"p2",2)) 
		portnum=2;
	else if(!strncmp(name,"p3",2)) 
		portnum=3;
	else if(!strncmp(name,"p4",2)) 
		portnum=4;
	else if(!strncmp(name,"p5",2)) 
		portnum=5;
	else if (!strncmp(name,"all",2)) 
		portnum=0xff;

	return portnum;
}

static int _getlanstatus(char *cmd , int cmd_len)
{
	struct lan_port_status port_status;
	int len=sizeof(struct lan_port_status);
	int portnum;
	cmd[cmd_len]='\0';//mark_patch
	portnum = portname_to_num(cmd);
	
	if ( get_lanport_status(portnum, &port_status) < 0)
			return -1;
	
	memcpy(cmd,(char *)&port_status,len);
	
	return len;
}
static int _getlanRate(char *cmd , int cmd_len)
{
	struct port_rate port_rate[4]={0};
	int len1,len;
	unsigned char portnum;
	int portId;
	char *tmpInfo = cmd +1 ;
	cmd[cmd_len]='\0';
	
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	
	portId = portname_to_num(cmd);
	
	if ( get_lanport_rate(portId, &port_rate[0]) < 0)
		return -1;
	
	if(portId==0xff){	
		portnum=4;
	}	
	else
		portnum=1;
	
	len1=sizeof(struct port_rate)*portnum;
	len=len1+1;
	
	memset(cmd,0,len);
	cmd[0]= portnum;
	
	memcpy(tmpInfo,(char *)(&port_rate[0]),len);
	
	return len;
}

static int _setlanBandwidth(char *cmd, int cmd_len)
{
	#define RTL_UPLINK_BW	0x2
	#define RTL_DOWNLINK_BW	0x1
	int portId=-1;
	char *tmpInfo = NULL;
	char *ptr;
	int flag=0;
	int ret=-1;
	int index=0;
	unsigned long bandwidth;
	unsigned long bw_value;
	char cmd_buf[100]={0};
	cmd[cmd_len]='\0';
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	portId = portname_to_num(cmd);
	#if 0
	if(portId =0xff)
		tmpInfo =cmd+4;
	else 
		tmpInfo =cmd+3;
	
	if(tmpInfo,"up",2)
		flag =RTL_UPLINK_BW;
	else if(tmpInfo,"down",4)
		flag =RTL_DOWNLINK_BW;
	else if(tmpInfo,"all",3)
		flag =RTL_DOWNLINK_BW|RTL_UPLINK_BW;
	#endif
	
	//printf("portId:%x,[%s]:[%d].\n",portId,__FUNCTION__,__LINE__);
	flag =RTL_DOWNLINK_BW;
	if (portId==-1||flag==0)
		goto out;
	ptr = strstr(cmd, "=");
	if (ptr) 
	{
		ptr = ptr + 1;
		
		if (ptr)
		{	
			sscanf( ptr, "%d", &bandwidth );   
		#if 0
			printf("bandwidth:%x [%s]:[%d].\n",bandwidth,__FUNCTION__,__LINE__);
		#endif
		}
	}
	bw_value = bandwidth ;
	#if 0
	if(portId==0xFF)
	{
		for(index=0;index<4;index++)
		{
			if(flag&RTL_DOWNLINK_BW)
			{
				//sprintf(cmd_buf,"ifconfig %s down",cmd);
				//system(cmd_buf);
			}
			if (flag&RTL_UPLINK_BW)
			{
				
			}
		}
	}
	else 
	#endif	
	if(portId>=0&&portId<4)
	{
		if(bw_value)
		{
			
			printf("bandwidth:%d %x [%s]:[%d].\n",bandwidth,bw_value,__FUNCTION__,__LINE__);
			sprintf(cmd_buf,"echo port %d engress bw %d > %s", portId,bw_value,_RTL_PORT_BANDWIDTH);
			system(cmd_buf);
		}
	}
	ret =0;
	
out:	
	return ret;
	
}
static int _getstats(char *cmd , int cmd_len)
{
	struct port_statistics statistics;
	int len=sizeof(struct port_statistics);
	int portnum;
	char *intf, *tmp;
	cmd[cmd_len]='\0';//mark_patch
	//printf(">>> %s\n",cmd);
	
	if(!strncmp(cmd,"p",1)) {
		portnum = portname_to_num(cmd);
		
		if ( get_lanport_stats(portnum, &statistics) < 0)
				return -1;		
		memcpy(cmd,(char *)&statistics,len);
	} else {
		//get statistics of wlan
		if ( get_wlan_stats(cmd, &statistics) < 0)
				return -1;

		memcpy(cmd,(char *)&statistics,len);
	}
	
	return len;
}

/* winfred_wang, RGB LED blink releate function */

/* rgb blink mode function */

#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
static int _set_RGBLed_blinkStart(char *cmd , int cmd_len)
{
	int ret;

	ret = rgb_led_blink_param_set(cmd,cmd_len);
	if(ret < 0)
	{
		printf("param content is not correct\n");
		return -1;
	}

	rgb_led_blink_init();
	return 0;
}

static int _set_RGBLed_blinkEnd(char *cmd , int cmd_len)
{	
	rgb_led_blink_exit();	
	return 0;
}
#endif

//-------------------------------------------------------------------
void set_11ac_txrate(WLAN_STA_INFO_Tp pInfo,char* txrate)
{
	char channelWidth=0;//20M 0,40M 1,80M 2
	char shortGi=0;
	char rate_idx=pInfo->txOperaRates-0x90;
	if(!txrate)return;
/*
	TX_USE_40M_MODE		= BIT(0),
	TX_USE_SHORT_GI		= BIT(1),
	TX_USE_80M_MODE		= BIT(2)
*/
	if(pInfo->ht_info & 0x4)
		channelWidth=2;
	else if(pInfo->ht_info & 0x1)
		channelWidth=1;
	else
		channelWidth=0;
	if(pInfo->ht_info & 0x2)
		shortGi=1;

	sprintf(txrate, "%d", VHT_MCS_DATA_RATE[channelWidth][shortGi][rate_idx]>>1);
}

void print_stainfo(char *stainfo_rsp)
{
	WLAN_STA_INFO_Tp pInfo;
	int i=0,rateid=0,sta_num=stainfo_rsp[0];
	char mode_buf[20],txrate[20];


	if(sta_num <= 0)
		printf("No Associated  station now!!!!\n ",i+1);
	for(i=0;i<sta_num;i++)
	{
		pInfo = (WLAN_STA_INFO_T *)&(stainfo_rsp[i*sizeof(WLAN_STA_INFO_T)+1]);
		printf("-----------------------------------------------\n");
		printf("station No.%d info\n",i+1);
		printf("MAC address : %02x:%02x:%02x:%02x:%02x:%02x \n",pInfo->addr[0],pInfo->addr[1],pInfo->addr[2],pInfo->addr[3],pInfo->addr[4],pInfo->addr[5]);

		if(pInfo->network & BAND_11N)
			sprintf(mode_buf, "%s", " 11n");
		else if (pInfo->network & BAND_11G)
			sprintf(mode_buf,"%s",  " 11g");	
		else if (pInfo->network & BAND_11B)
			sprintf(mode_buf, "%s", " 11b");
		else if (pInfo->network& BAND_11A)
			sprintf(mode_buf, "%s", " 11a");
		else if (pInfo->network& BAND_11AC)
			sprintf(mode_buf, "%s", " 11ac");
		else
			sprintf(mode_buf, "%s", " ---");	

		printf("Mode:%s \n",mode_buf);

		printf("TX packets:%d , RX packets:%d \n",pInfo->tx_packets, pInfo->rx_packets);
		if(pInfo->txOperaRates >= 0x90) {
			//sprintf(txrate, "%d", pInfo->acTxOperaRate); 
			set_11ac_txrate(pInfo, txrate);
		}else if((pInfo->txOperaRates & 0x80) != 0x80){	
			if(pInfo->txOperaRates%2){
				sprintf(txrate, "%d%s",pInfo->txOperaRates/2, ".5"); 
			}else{
				sprintf(txrate, "%d",pInfo->txOperaRates/2); 
			}
		}else{
			if((pInfo->ht_info & 0x1)==0){ //20M
				if((pInfo->ht_info & 0x2)==0){//long
					for(rateid=0; rateid<16;rateid++){
						if(rate_11n_table_20M_LONG[rateid].id == pInfo->txOperaRates){
							sprintf(txrate, "%s", rate_11n_table_20M_LONG[rateid].rate);
							break;
						}
					}
				}else if((pInfo->ht_info & 0x2)==0x2){//short
					for(rateid=0; rateid<16;rateid++){
						if(rate_11n_table_20M_SHORT[rateid].id == pInfo->txOperaRates){
							sprintf(txrate, "%s", rate_11n_table_20M_SHORT[rateid].rate);
							break;
						}
					}
				}
			}else if((pInfo->ht_info & 0x1)==0x1){//40M
				if((pInfo->ht_info & 0x2)==0){//long
					
					for(rateid=0; rateid<16;rateid++){
						if(rate_11n_table_40M_LONG[rateid].id == pInfo->txOperaRates){
							sprintf(txrate, "%s", rate_11n_table_40M_LONG[rateid].rate);
							break;
						}
					}
				}else if((pInfo->ht_info & 0x2)==0x2){//short
					for(rateid=0; rateid<16;rateid++){
						if(rate_11n_table_40M_SHORT[rateid].id == pInfo->txOperaRates){
							sprintf(txrate, "%s", rate_11n_table_40M_SHORT[rateid].rate);
							break;
						}
					}
				}
			}
		   }
		printf("TX Rate : %s \n",txrate);
		printf("Sleep : %s \n",( (pInfo->flag & STA_INFO_FLAG_ASLEEP) ? "yes" : "no"));
		printf("Expired time : %d seconds \n",pInfo->expired_time/100);

	}

}

void print_bssinfo(char *bssinfo_rsp)
{
	WLAN_BSS_INFO_Tp bssInfo;
	char *pMsg;
	
	bssInfo = (WLAN_BSS_INFO_T *)bssinfo_rsp;
	printf("BSSID : %02x:%02x:%02x:%02x:%02x:%02x \n",bssInfo->bssid[0],bssInfo->bssid[1],bssInfo->bssid[2],
													bssInfo->bssid[3],bssInfo->bssid[4],bssInfo->bssid[5]);
	printf("SSID : %s \n",bssInfo->ssid);

	switch (bssInfo->state) {
		case STATE_DISABLED:
			pMsg = "Disabled";
			break;
		case STATE_IDLE:
			pMsg = "Idle";
			break;
		case STATE_STARTED:
			pMsg = "Started";
			break;
		case STATE_CONNECTED:
			pMsg = "Connected";
			break;
		case STATE_WAITFORKEY:
			pMsg = "Waiting for keys";
			break;
		case STATE_SCANNING:
			pMsg = "Scanning";
			break;
		default:
			pMsg=NULL;
		}

	printf("State : %s \n",pMsg);

	printf("Channel : %d \n",bssInfo->channel);		
	
}

void print_port_status(char *status)
{
	struct lan_port_status *port_status;
	
	port_status = (struct lan_port_status *)status;
	
	printf("Link = %s\n",lan_link_spec[port_status->link]);	
	printf("Speed = %s\n",lan_speed_spec[port_status->speed]);
	printf("Nway mode = %s\n",enable_spec[port_status->nway]);	
	printf("Duplex = %s\n",enable_spec[port_status->duplex]);
		
}
void print_port_stats(char *stats)
{
	struct port_statistics *port_stats;
	port_stats = (struct port_statistics *)stats;

	printf("rx bytes=%d\n",port_stats->rx_bytes);
	printf("rx unicast packets =%d\n",port_stats->rx_unipkts);
	printf("rx multicast packets =%d\n",port_stats->rx_mulpkts);
	printf("rx brocast packets =%d\n",port_stats->rx_bropkts);
	printf("rx discard packets  =%d\n",port_stats->rx_discard);
	printf("rx error packets  =%d\n",port_stats->rx_error);
	printf("tx bytes=%d\n",port_stats->tx_bytes);
	printf("tx unicast packets =%d\n",port_stats->tx_unipkts);
	printf("tx multicast packets =%d\n",port_stats->tx_mulpkts);
	printf("tx brocast packets =%d\n",port_stats->tx_bropkts);
	printf("tx discard packets  =%d\n",port_stats->tx_discard);
	printf("tx error packets  =%d\n",port_stats->tx_error);	
}

void print_port_rate(char *rate,int cmd_len)
{
	struct port_rate *portRate;
	portRate = (struct port_rate *)rate;
	int index=0;
	for(index =0;index<cmd_len;index++)
	{
		portRate =rate+index;
		printf("[%d] port%d: rx rate=%d(%d)  tx rate=%d(%d)\n",index,portRate->port_id,
			portRate->rx_rate,portRate->rx_percent,portRate->tx_rate,portRate->tx_percent);
	}
	return;
}



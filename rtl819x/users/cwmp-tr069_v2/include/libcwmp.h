#ifndef _LIBCWMP_H_
#define _LIBCWMP_H_

#include "parameter_api.h"
#include "cwmp_utility.h"
#include "cwmp_download.h"
#include "apmib.h"


#define CWMP_QUEUE_SUPPORT
#ifdef  CWMP_QUEUE_SUPPORT
#define CWMP_STORE_CONFIG_PATH         "/dev/mtdblock2" // flash mtd block to store tr069 queued requests even across reboot, size: 0x4000=16kB, according to the define in linux-2.6.30/drivers/mtd/maps/rtl819x_flash.c
#define CWMP_QUEUE_SHM_KEY             0x12345676       // share memory key for storing queued requests temporarily, can be MODIFIED to any value
#define CWMP_QUEUE_INFO_OFFSET         0x1000           // 4kB, can be MODIFIED to 0~0x4000, make sure there has enough space between OFFSET~0x4000 to store queued requests
#define CWMP_QUEUE_MAX_REQS            3                // queue at most 3 requests for EACH of upload queuing and download queuing, can be MODIFIED to support more requests
#define CWMP_QUEUE_MAX_DELAY_SEC       3600

// bellow for upload/download information
#define CWMP_UPDOWNLOAD_UPTIME         4       // start of each flash mtdblock section
#define CWMP_UPDOWNLOAD_START_TIME     4       // start of one upload/download control
#define CWMP_UPDOWNLOAD_COMPLETE_TIME  4
#define CWMP_UPDOWNLOAD_FAULT_CODE     4
#define CWMP_UPDOWNLOAD_COMMAND_KEY    32      // start of one upload/download contents
#define CWMP_UPDOWNLOAD_FILE_TYPE      64
#define CWMP_UPDOWNLOAD_URL            256
#define CWMP_UPDOWNLOAD_USERNAME       256
#define CWMP_UPDOWNLOAD_PASSWORD       256
#define CWMP_UPDOWNLOAD_DELAY_SEC      4
#define CWMP_UPDOWNLOAD_STATUS         8
#define CWMP_DOWNLOAD_FILE_SIZE        4       // extra contents for download
#define CWMP_DOWNLOAD_FILE_NAME        256
#define CWMP_DOWNLOAD_SUCCESS_URL      256
#define CWMP_DOWNLOAD_FAILURE_URL      256
#define CWMP_UPDOWNLOAD_CHECK_SUM      4       // end of each flash mtdblock section

#define CWMP_UPDOWNLOAD_UPTIME_OFFSET		0
#define CWMP_UPDOWNLOAD_STARTTIME_OFFSET	(CWMP_UPDOWNLOAD_UPTIME_OFFSET + CWMP_UPDOWNLOAD_UPTIME)              // 4
#define CWMP_UPDOWNLOAD_COMPLETETIME_OFFSET	(CWMP_UPDOWNLOAD_STARTTIME_OFFSET + CWMP_UPDOWNLOAD_START_TIME)       // 8
#define CWMP_UPDOWNLOAD_FAULTCODE_OFFSET	(CWMP_UPDOWNLOAD_COMPLETETIME_OFFSET + CWMP_UPDOWNLOAD_COMPLETE_TIME) // 12
#define CWMP_UPDOWNLOAD_COMMANDKEY_OFFSET	(CWMP_UPDOWNLOAD_FAULTCODE_OFFSET + CWMP_UPDOWNLOAD_FAULT_CODE)       // 16
#define CWMP_UPDOWNLOAD_FILETYPE_OFFSET		(CWMP_UPDOWNLOAD_COMMANDKEY_OFFSET + CWMP_UPDOWNLOAD_COMMAND_KEY)     // 48
#define CWMP_UPDOWNLOAD_URL_OFFSET			(CWMP_UPDOWNLOAD_FILETYPE_OFFSET + CWMP_UPDOWNLOAD_FILE_TYPE)         // 112
#define CWMP_UPDOWNLOAD_USERNAME_OFFSET		(CWMP_UPDOWNLOAD_URL_OFFSET +CWMP_UPDOWNLOAD_URL)                     // 368
#define CWMP_UPDOWNLOAD_PASSWORD_OFFSET		(CWMP_UPDOWNLOAD_USERNAME_OFFSET + CWMP_UPDOWNLOAD_USERNAME)          // 624
#define CWMP_UPDOWNLOAD_DELAYSEC_OFFSET		(CWMP_UPDOWNLOAD_PASSWORD_OFFSET + CWMP_UPDOWNLOAD_PASSWORD)          // 880
#define CWMP_UPDOWNLOAD_STATUS_OFFSET		(CWMP_UPDOWNLOAD_DELAYSEC_OFFSET + CWMP_UPDOWNLOAD_DELAY_SEC)         // 884

#define CWMP_DOWNLOAD_FILESIZE_OFFSET		(CWMP_UPDOWNLOAD_STATUS_OFFSET + CWMP_UPDOWNLOAD_STATUS)              // 892
#define CWMP_DOWNLOAD_FILENAME_OFFSET		(CWMP_DOWNLOAD_FILESIZE_OFFSET + CWMP_DOWNLOAD_FILE_SIZE)             // 896
#define CWMP_DOWNLOAD_SUCCESS_OFFSET		(CWMP_DOWNLOAD_FILENAME_OFFSET + CWMP_DOWNLOAD_FILE_NAME)             // 1152
#define CWMP_DOWNLOAD_FAILURE_OFFSET		(CWMP_DOWNLOAD_SUCCESS_OFFSET + CWMP_DOWNLOAD_SUCCESS_URL)            // 1408

#define CWMP_UPLOAD_CHECKSUM_OFFSET			(CWMP_UPDOWNLOAD_STATUS_OFFSET + CWMP_UPDOWNLOAD_STATUS)              // 892
#define CWMP_DOWNLOAD_CHECKSUM_OFFSET		(CWMP_DOWNLOAD_FAILURE_OFFSET + CWMP_DOWNLOAD_FAILURE_URL)            // 1664

#define CWMP_UPLOAD_SIZE                    (CWMP_UPLOAD_CHECKSUM_OFFSET + CWMP_UPDOWNLOAD_CHECK_SUM)	          // 896
#define CWMP_DOWNLOAD_SIZE                  (CWMP_DOWNLOAD_CHECKSUM_OFFSET + CWMP_UPDOWNLOAD_CHECK_SUM)           // 1668
	
#define CWMP_QUEUE_SHM_UPSIZE               (CWMP_UPLOAD_SIZE)*(CWMP_QUEUE_MAX_REQS)                              // 2688
#define CWMP_QUEUE_SHM_DOWNSIZE             (CWMP_DOWNLOAD_SIZE)*(CWMP_QUEUE_MAX_REQS)                            // 5004
#define CWMP_QUEUE_SHM_SIZE                 (CWMP_QUEUE_SHM_UPSIZE + CWMP_QUEUE_SHM_DOWNSIZE)                     // 7692

#define QUEUE_DBG
#ifdef QUEUE_DBG
#define DBG_PRINT(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define DBG_PRINT(fmt, ...)
#endif
#endif


#define _USE_FILE_FOR_OUTPUT_
#ifdef _USE_FILE_FOR_OUTPUT_
#define OUTXMLFILENAME 		"/tmp/cwmp.xml"
#define OUTXMLFILENAME_BK	"/tmp/cwmp_bk.xml"
#endif


struct cwmp_userdata
{
	//relative to SOAP header
	unsigned int		ID;
	unsigned int		HoldRequests;
	unsigned int		NoMoreRequests;
	unsigned int		CPE_MaxEnvelopes;
	unsigned int		ACS_MaxEnvelopes;
	
	//cwmp:fault
	int			FaultCode;
	
	//download/upload
#ifdef CWMP_QUEUE_SUPPORT
    int             IsExecuting;
	char 		    ExcuteIdx;
	// for upload/download control: the former 3 for upload, the latter 3 for download
	int 		    Pid[CWMP_QUEUE_MAX_REQS*2];
	char 		    NeedExecuted[CWMP_QUEUE_MAX_REQS*2];
	int			    DownloadState[CWMP_QUEUE_MAX_REQS*2];
	int			    DownloadWay[CWMP_QUEUE_MAX_REQS*2];
	char			*DLCommandKey[CWMP_QUEUE_MAX_REQS*2];
	time_t			DLStartTime[CWMP_QUEUE_MAX_REQS*2];
	time_t			DLCompleteTime[CWMP_QUEUE_MAX_REQS*2];
	unsigned int	DLFaultCode[CWMP_QUEUE_MAX_REQS*2];
	// upload/download information
	DownloadInfo_T	DownloadInfo[CWMP_QUEUE_MAX_REQS*2];
#else
	int			    DownloadState;
	int			    DownloadWay;
	char			*DLCommandKey;
	time_t			DLStartTime;
	time_t			DLCompleteTime;
	unsigned int	DLFaultCode;
	DownloadInfo_T	DownloadInfo;
#endif
        	
	//inform
	unsigned int		InformInterval; //PeriodicInformInterval
	time_t			InformTime; //PeriodicInformTime
	int			    PeriodicInform;
	unsigned int	EventCode;
	struct node		*NotifyParameter;
	unsigned int	InformIntervalCnt; 
	
	//ScheduleInform
	unsigned int	ScheduleInformCnt; //for scheduleInform RPC Method, save the DelaySeconds
	char			*SI_CommandKey;

	//Reboot
	char			*RB_CommandKey;	//reboot's commandkey
	int			    Reboot; // reboot flag
	
	//FactoryReset
	int			    FactoryReset;

	// andrew. 
	char 			*url;	// ACS URL
	char 			*username; // username used to auth us to ACS.
	char 			*password; // passwrd used to auth us to ACS.
	char 			*conreq_username;
	char 			*conreq_password;
	char 			*realm;
	int			    server_port;
	char			*server_path;
	void *			machine;
	
	char			*redirect_url;
	int			    redirect_count;
	
	//certificate
	char			*cert_passwd;
	char			*cert_path;
	char			*ca_cert;

	char			*notify_filename;
	
	int			    url_changed;
	
	int			    inform_ct_ext;
	int			    PingConfigState_ct_ext;

	unsigned int	retryMinWaitInterval;   // for TR181 CWMPRetryMinimumWaitInterval
	unsigned int	retryIntervalMutiplier; // for TR181 CWMPRetryIntervalMultiplier
};


int cwmp_main( struct CWMP_NODE troot[] );

#endif /*#ifndef _LIBCWMP_H_*/

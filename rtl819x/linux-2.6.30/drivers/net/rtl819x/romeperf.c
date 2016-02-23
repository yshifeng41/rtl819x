/*
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
* Program : Performance Profiling for ROME Driver
* Abstract :
* Author : Yung-Chieh Lo (yjlou@realtek.com.tw)
* $Id: romeperf.c,v 1.1 2007-12-21 10:28:22 davidhsu Exp $
*/

#include "romeperf.h"
#include <net/rtl/rtl_glue.h>
#define KERNEL_SYSeALLS
#include <asm/unistd.h>
#include <asm/processor.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>

#define rtlglue_malloc(size)	kmalloc(size, 0x1f0)
#define rtlglue_free(p)	kfree(p)
#ifndef rtlglue_printf
#define rtlglue_printf printk
#endif
#include <asm/rtl865x/regdef.h>
#define init_CP3_counter()    \
{    \
    __asm__ __volatile__(      \
    	"mfc0 	$8, $12\n"         \
    	"la 	$9, 0x80000000\n"         \
		"or 	$8, $9\n"         \
		"mtc0 	$8, $12\n"         \
		"nop\n"         \
    	);         \
}

#define clear_CP3_counter()    \
{    \
    __asm__ __volatile__(    \
        "ctc3 $0, $0  \n"  \
        "mtc3 $0, $8  \n"  \
        "mtc3 $0, $9  \n"  \
        "mtc3 $0, $10 \n"  \
        "mtc3 $0, $11 \n"  \
        "mtc3 $0, $12 \n"  \
        "mtc3 $0, $13 \n"  \
        "mtc3 $0, $14 \n"  \
        "mtc3 $0, $15 \n"  \
		"nop  \n"    \
		);      \
}

#if defined(CONFIG_ARCH_CPU_RLX5281)
/* set counter controls:  0   	  1 	 2 	    3
** counter0 cycles	     (0x08) (0x08) (0x08) (0x08)
** counter1 ifetches 	 (0x01) (0x01) (0x01)	
** counter1 dmiss busy			       (0x0b)
** counter2 ld+stores 	  (0x06)	       (0x06)
** counter2 icache misses	 (0x02) (0x02)
** counter3 dcache misses (0x0a) (0x0a)        (0x0a)
** counter3 imiss busy	 		(0x03)
*/
/*cnt_mode: counter mode 0 ~ 3*/
#define  set_and_start_counter(cnt_mode)    \
{    \
     __asm__ __volatile__(    \
	 "li $11, %0 \n"     \
     "la $9, 0x0a060108 \n"   \
     "beq $11, $0, 1f #cntmode == 0 \n"  \
     "addiu $11, -1 \n"  \
     "la $9, 0x0a020108 \n"  \
     "beq $11, $0, 1f #cntmode == 1 \n"  \
     "addiu $11, -1 \n"  \
     "la $9, 0x03020108 \n"  \
     "beq $11, $0, 1f #cntmode == 2 \n"  \
     "addiu $11, -1 \n"  \
     "la $9, 0x0a060b08 \n"  \
     "beq $11, $0, 1f #cntmode == 3 \n"  \
     "nop \n"  \
     "1: \n"  \
	 "ctc3 $9, $0 \n"  \
	 "nop \n"  \
     :      \
     :"i"(cnt_mode)    \
	 );    \
}
#elif defined(CONFIG_ARCH_CPU_RLX4181)
/* set counter controls:  0	 1	 2	 3
** counter0 cycles 	  (0x10) (0x10) (0x10) (0x10)
** counter1 ifetches   	  (0x11) (0x11) (0x11)
** counter1 dmiss busy    		       (0x1b)
** counter2 ld+stores	  (0x16) 	       (0x16)
** counter2 icache misses	 (0x12) (0x12)
** counter3 dcache misses (0x1a) (0x1a)        (0x1a)
** counter3 imiss busy   	        (0x13)
*/
#define  set_and_start_counter(cnt_mode)    \
{    \
     __asm__ __volatile__(    \
	 "li $11, %0 \n"     \
     "la $9, 0x1a161110\n"   \
     "beq $11, $0, 1f #cntmode == 0 \n"  \
     "addiu $11, -1 \n"  \
     "la $9, 0x1a121110\n"  \
     "beq $11, $0, 1f #cntmode == 1 \n"  \
     "addiu $11, -1 \n"  \
     "la $9, 0x13121110\n"  \
     "beq $11, $0, 1f #cntmode == 2 \n"  \
     "addiu $11, -1 \n"  \
     "la $9, 0x1a161b10\n"  \
     "beq $11, $0, 1f #cntmode == 3 \n"  \
     "nop \n"  \
     "1: \n"  \
	 "ctc3 $9, $0 \n"  \
	 "nop \n"  \
     :      \
     :"i"(cnt_mode)    \
	 );    \
}
#else
#error " this romeperf only support RLX4181 and RLX 5281 cpu type"
#endif

#define stop_counter()    \
{    \
     __asm__ __volatile__(    \
     "ctc3 $0, $0 \n"  \
     "nop   \n"     \
	 "nop  \n"     \
     );   \
}

#define save_counter(addr)     \
{    \
     __asm__ __volatile__(    \
     "mfc3 $8, $9 #counter0[48:32]   \n"   \
     "nop   \n"   \
     "sw $8, 0(%0)   \n"   \
     "mfc3 $8, $8 #counter0[31:0]   \n"   \
     "nop   \n"   \
     "nop   \n"   \
     "sw $8, 4(%0)   \n"   \
     "mfc3 $8, $11 #counter1[48:32]   \n"   \
     "nop   \n"   \
     "sw $8, 8(%0)   \n"   \
     "mfc3 $8, $10 #counter1[31:0]   \n"   \
     "nop   \n"   \
     "nop   \n"   \
     "sw $8, 12(%0)   \n"   \
     "mfc3 $8, $13 #counter2[48:32]   \n"   \
     "nop   \n"   \
     "sw $8, 16(%0)   \n"   \
     "mfc3 $8, $12 #counter2[31:0]   \n"   \
     "nop   \n"   \
     "nop   \n"   \
     "sw $8, 20(%0)   \n"   \
     "mfc3 $8, $15 #counter3[48:32]   \n"   \
     "nop   \n"   \
     "sw $8, 24(%0)   \n"   \
     "mfc3 $8, $14 #counter3[31:0]   \n"   \
     "nop   \n"   \
     "nop   \n"   \
     "sw $8, 28(%0)   \n"   \
     "nop   \n"   \
     :     \
     :"r" (addr)   \
     );    \
}

#undef __IRAM
#define __IRAM __IRAM_FWD

enum CP3_COUNTER
{
	CP3CNT_CYCLES = 0,
	CP3CNT_NEW_INST_FECTH,
	CP3CNT_NEW_INST_FETCH_CACHE_MISS,
	CP3CNT_NEW_INST_MISS_BUSY_CYCLE,
	CP3CNT_DATA_STORE_INST,
	CP3CNT_DATA_LOAD_INST,
	CP3CNT_DATA_LOAD_OR_STORE_INST,
	CP3CNT_EXACT_RETIRED_INST,
	CP3CNT_RETIRED_INST_FOR_PIPE_A,
	CP3CNT_RETIRED_INST_FOR_PIPE_B,
	CP3CNT_DATA_LOAD_OR_STORE_CACHE_MISS,
	CP3CNT_DATA_LOAD_OR_STORE_MISS_BUSY_CYCLE,
	CP3CNT_RESERVED12,
	CP3CNT_RESERVED13,
	CP3CNT_RESERVED14,
	CP3CNT_RESERVED15,
};

static uint64 currCnt[4];

/* Global variables */
//uint64 cnt1, cnt2;
rtl8651_romeperf_stat_t romePerfStat[ROMEPERF_INDEX_MAX];
rtl8651_romeperf_stat_t statSnapShot[ROMEPERF_INDEX_MAX];
uint32 rtl8651_romeperf_inited = 0;
uint32 rtl8651_romeperf_enable = TRUE;

__IRAM static void CP3_COUNTER0_INIT( void )
{
	init_CP3_counter();
}

__IRAM static void CP3_COUNTER0_START( void )
{
	set_and_start_counter(2);
}

__IRAM static void CP3_COUNTER0_STOP( void )
{
	stop_counter();
}

__IRAM static void CP3_COUNTER0_GET_ALL( void )
{
	uint64 *addr0 = &currCnt[0];
	save_counter(addr0);
}

int32 rtl8651_romeperfInit()
{

	unsigned long flags = 0;
	local_irq_save(flags);

	CP3_COUNTER0_INIT();
	CP3_COUNTER0_START();

	rtl8651_romeperf_inited = TRUE;
	rtl8651_romeperf_enable = TRUE;
	memset( &romePerfStat, 0, sizeof( romePerfStat ) );

	romePerfStat[ROMEPERF_INDEX_NAPT_ADD].desc = "NAPT add_all";
	romePerfStat[ROMEPERF_INDEX_NAPT_ADD_1].desc = "NAPT add_checkIntIP";
	romePerfStat[ROMEPERF_INDEX_NAPT_ADD_2].desc = "NAPT add_localServer";
	romePerfStat[ROMEPERF_INDEX_NAPT_ADD_3].desc = "NAPT add_checkExtIp";
	romePerfStat[ROMEPERF_INDEX_NAPT_ADD_4].desc = "NAPT add_dupCheck1";
	romePerfStat[ROMEPERF_INDEX_NAPT_ADD_5].desc = "NAPT add_dupCheck2";
	romePerfStat[ROMEPERF_INDEX_NAPT_ADD_6].desc = "NAPT add_bPortReused";
	romePerfStat[ROMEPERF_INDEX_NAPT_ADD_7].desc = "NAPT add_routeCache";
	romePerfStat[ROMEPERF_INDEX_NAPT_ADD_8].desc = "NAPT add_tooManyConn";
	romePerfStat[ROMEPERF_INDEX_NAPT_ADD_9].desc = "NAPT add_initConn";
	romePerfStat[ROMEPERF_INDEX_NAPT_ADD_10].desc = "NAPT add_decisionFlo";
	romePerfStat[ROMEPERF_INDEX_NAPT_ADD_11].desc = "NAPT add_ambiguous";
	romePerfStat[ROMEPERF_INDEX_NAPT_DEL].desc = "NAPT del";
	romePerfStat[ROMEPERF_INDEX_NAPT_FIND_OUTBOUND].desc = "NATP outbound";
	romePerfStat[ROMEPERF_INDEX_NAPT_FIND_INBOUND].desc = "NAPT inbound";
	romePerfStat[ROMEPERF_INDEX_NAPT_UPDATE].desc = "NAPT update";
	romePerfStat[ROMEPERF_INDEX_UNTIL_RXTHREAD].desc = "IntDispatch-RxThread";
	romePerfStat[ROMEPERF_INDEX_RECVLOOP].desc = "RecvLoop-FwdInput";
	romePerfStat[ROMEPERF_INDEX_FWDENG_INPUT].desc = "FwdEng_Input()";
	romePerfStat[ROMEPERF_INDEX_BEFORE_CRYPTO_ENCAP].desc = "FwdInput-Crypto(En)";
	romePerfStat[ROMEPERF_INDEX_ENCAP].desc = "IPSEC Encap";
	romePerfStat[ROMEPERF_INDEX_ENCAP_CRYPTO_ENGINE].desc = "Encap Crypto";
	romePerfStat[ROMEPERF_INDEX_ENCAP_AUTH_ENGINE].desc = "Encap Authtication";
	romePerfStat[ROMEPERF_INDEX_BEFORE_CRYPTO_DECAP].desc = "FwdInput-Crypto(De)";
	romePerfStat[ROMEPERF_INDEX_DECAP].desc = "IPSEC Decap";
	romePerfStat[ROMEPERF_INDEX_DECAP_CRYPTO_ENGINE].desc = "Decap Crypto";
	romePerfStat[ROMEPERF_INDEX_DECAP_AUTH_ENGINE].desc = "Decap Authtication";
	romePerfStat[ROMEPERF_INDEX_FASTPATH].desc = "Fast Path";
	romePerfStat[ROMEPERF_INDEX_SLOWPATH].desc = "Slow Path";
	romePerfStat[ROMEPERF_INDEX_FWDENG_SEND].desc = "FwdEngSend()";
	romePerfStat[ROMEPERF_INDEX_UNTIL_ACLDB].desc = "FwdInput() Until ACLDB";
	romePerfStat[ROMEPERF_INDEX_GET_MTU_AND_SOURCE_MAC].desc = "L3Route_MTU_srcMAC";
	romePerfStat[ROMEPERF_INDEX_PPTPL2TP_1].desc = "L3Route_PPTPL2TP_1";
	romePerfStat[ROMEPERF_INDEX_PPPOE_ARP_CACHE].desc = "L3Route_PPPoE_ArpCache";
	romePerfStat[ROMEPERF_INDEX_PPTPL2TP_SEND].desc = "L3Route_PptpL2tpSend()";
	romePerfStat[ROMEPERF_INDEX_FRAG].desc = "L3Route_Fragment";
	romePerfStat[ROMEPERF_INDEX_EGRESS_ACL].desc = "FwdSend_EgressACL";
	romePerfStat[ROMEPERF_INDEX_PPTPL2TP_ENCAP].desc = "FwdSend_PPTP/L2TP_Encap";
	romePerfStat[ROMEPERF_INDEX_FROM_PS].desc = "FwdSend_FromPS";
	romePerfStat[ROMEPERF_INDEX_EXTDEV_SEND].desc = "FwdSend_ExtDevSend()";
	romePerfStat[ROMEPERF_INDEX_FRAG_2ND_HALF].desc = "FwdSend_Frag_2ndHalf()";
	romePerfStat[ROMEPERF_INDEX_TXPKTPOST].desc = "rtl8651_txPktPostProcessing()";
	romePerfStat[ROMEPERF_INDEX_MBUFPAD].desc = "mBuf_padding()";
	romePerfStat[ROMEPERF_INDEX_TXALIGN].desc = "_swNic_txAlign";
	romePerfStat[ROMEPERF_INDEX_ISRTXRECYCLE].desc = "_swNic_isrTxRecycle";
	romePerfStat[ROMEPERF_INDEX_16].desc = "FwdEng_temp_16";
	romePerfStat[ROMEPERF_INDEX_17].desc = "FwdEng_temp_17";
	romePerfStat[ROMEPERF_INDEX_18].desc = "FwdEng_temp_18";
        romePerfStat[ROMEPERF_INDEX_19].desc = "FwdEng_temp_19";
        romePerfStat[ROMEPERF_INDEX_20].desc = "FwdEng_temp_20";
        romePerfStat[ROMEPERF_INDEX_21].desc = "FwdEng_temp_21";
        romePerfStat[ROMEPERF_INDEX_22].desc = "FwdEng_temp_22";
        romePerfStat[ROMEPERF_INDEX_23].desc = "FwdEng_temp_23";
        romePerfStat[ROMEPERF_INDEX_24].desc = "FwdEng_temp_24";
        romePerfStat[ROMEPERF_INDEX_25].desc = "FwdEng_temp_25";
        romePerfStat[ROMEPERF_INDEX_FLUSHDCACHE].desc = "rtlglue_flushDCache";
        romePerfStat[ROMEPERF_INDEX_IRAM_1].desc = "IRAM Cached within IRAM";
        romePerfStat[ROMEPERF_INDEX_IRAM_2].desc = "IRAM Uncached within IRAM";
        romePerfStat[ROMEPERF_INDEX_IRAM_3].desc = "test ICACHE  (1024*100)";
        romePerfStat[ROMEPERF_INDEX_IRAM_4].desc = "test Uncached (1024*10)";
        romePerfStat[ROMEPERF_INDEX_DRAM_1].desc = "DRAM Cached within DRAM";
        romePerfStat[ROMEPERF_INDEX_DRAM_2].desc = "DRAM Uncached within DRAM";
        romePerfStat[ROMEPERF_INDEX_DRAM_3].desc = "test DCACHE  (1024*100)";
        romePerfStat[ROMEPERF_INDEX_DRAM_4].desc = "test Uncached (1024*10)";
        romePerfStat[ROMEPERF_INDEX_BMP].desc = "KMP Algorithm";
        romePerfStat[ROMEPERF_INDEX_MDCMDIO].desc = "MDCMDIO PHY Register ACCESS";
        romePerfStat[ROMEPERF_INDEX_TEST0].desc = "TEST 0";
        romePerfStat[ROMEPERF_INDEX_TEST1].desc = "TEST 1";
        romePerfStat[ROMEPERF_INDEX_TEST2].desc = "TEST 2";
        romePerfStat[ROMEPERF_INDEX_TEST3].desc = "TEST 3";
        romePerfStat[ROMEPERF_INDEX_TEST4].desc = "TEST 4";
	 romePerfStat[ROMEPERF_INDEX_PCIE_IRQ].desc = "PCIE_IRQ";
	romePerfStat[ROMEPERF_INDEX_RX_DSR].desc = "RX DSR";
	romePerfStat[ROMEPERF_INDEX_RX_ISR].desc = "RX ISR";
	romePerfStat[ROMEPERF_INDEX_validate_mpdu].desc = "validate mpdu";
	romePerfStat[ROMEPERF_INDEX_swNic_receive].desc = "swNic_receive";
	romePerfStat[ROMEPERF_INDEX_rtl_processReceivedInfo].desc = "rtl_processReceivedInfo";
	romePerfStat[ROMEPERF_INDEX_rtl_decideRxDevice].desc = "rtl_decideRxDevice";
	romePerfStat[ROMEPERF_INDEX_rtl_processRxFrame].desc = "rtl_processRxFrame";
	romePerfStat[ROMEPERF_INDEX_rtl_processBridgeShortCut].desc = "rtl_processBridgeShortCut";
	romePerfStat[ROMEPERF_INDEX_rtl_processRxToProtcolStack].desc = "rtl_processRxToProtcolStack";
	romePerfStat[ROMEPERF_INDEX_netif_receive_skb].desc = "netif_receive_skb";
	romePerfStat[ROMEPERF_INDEX_FastPath_Enter].desc = "FastPath_Enter";
	romePerfStat[ROMEPERF_INDEX_checksum_in_fastpath].desc = "checksum_in_fastpath";
	romePerfStat[ROMEPERF_INDEX_enter_fast_path].desc = "enter_fast_path";
	romePerfStat[ROMEPERF_INDEX_ip_finish_output3].desc = "ip_finish_output3";
	romePerfStat[ROMEPERF_INDEX_ndo_start_xmit].desc = "ndo_start_xmit";
	romePerfStat[ROMEPERF_INDEX_rtl_preProcess_xmit].desc = "rtl_preProcess_xmit";
	romePerfStat[ROMEPERF_INDEX_rtl_fill_txInfo].desc = "rtl_fill_txInfo";
	romePerfStat[ROMEPERF_INDEX_swNic_send].desc = "swNic_send";
	romePerfStat[ROMEPERF_INDEX_swNic_txDone].desc = "swNic_txDone";
	romePerfStat[ROMEPERF_INDEX_dev_kfree_skb_any].desc = "dev_kfree_skb_any"	;
	romePerfStat[ROMEPERF_INDEX_fastpath_1].desc = "fastpath_1";
	romePerfStat[ROMEPERF_INDEX_fastpath_2].desc = "fastpath_2";
	romePerfStat[ROMEPERF_INDEX_fastpath_3].desc = "fastpath_3";


	local_irq_restore(flags);
        return SUCCESS;
}

int32 rtl8651_romeperfReset()
{
	rtl8651_romeperfInit();

	return SUCCESS;
}

int32 rtl8651_romeperfPause( void )
{
	if ( rtl8651_romeperf_inited == FALSE ) return FAILED;

	rtl8651_romeperf_enable = FALSE;

	/* Louis patch: someone will disable CP3 in somewhere. */
	CP3_COUNTER0_INIT();

	CP3_COUNTER0_STOP();

	return SUCCESS;
}

int32 rtl8651_romeperfResume( void )
{
	if ( rtl8651_romeperf_inited == FALSE ) return FAILED;

	rtl8651_romeperf_enable = TRUE;

	/* Louis patch: someone will disable CP3 in somewhere. */
	CP3_COUNTER0_INIT();

	CP3_COUNTER0_START();

	return SUCCESS;
}

__IRAM int rtl8651_romeperfEnterPoint( int index )
{
	if ( rtl8651_romeperf_inited == FALSE ||
	     rtl8651_romeperf_enable == FALSE ) return FAILED;
	if ( index >= (sizeof(romePerfStat)/sizeof(rtl8651_romeperf_stat_t)) )
		return FAILED;

	/* Louis patch: someone will disable CP3 in somewhere. */
	CP3_COUNTER0_INIT();

	CP3_COUNTER0_STOP();
	CP3_COUNTER0_GET_ALL();
	romePerfStat[index].tempCycle[0] = currCnt[0];
	romePerfStat[index].tempCycle[1] = currCnt[1];
	romePerfStat[index].tempCycle[2] = currCnt[2];
	romePerfStat[index].tempCycle[3] = currCnt[3];
	romePerfStat[index].hasTempCycle = TRUE;
	CP3_COUNTER0_START();

	return SUCCESS;
}

__IRAM int rtl8651_romeperfExitPoint(int index )
{
	if ( rtl8651_romeperf_inited == FALSE ||
	     rtl8651_romeperf_enable == FALSE ) return FAILED;
	if ( index >= (sizeof(romePerfStat)/sizeof(rtl8651_romeperf_stat_t)) )
		return FAILED;
	if ( romePerfStat[index].hasTempCycle == FALSE )
		return FAILED;

	/* Louis patch: someone will disable CP3 in somewhere. */
	CP3_COUNTER0_INIT();

	CP3_COUNTER0_STOP();
	CP3_COUNTER0_GET_ALL();
	romePerfStat[index].accCycle[0] += currCnt[0]-romePerfStat[index].tempCycle[0];
	romePerfStat[index].accCycle[1] += currCnt[1]-romePerfStat[index].tempCycle[1];
	romePerfStat[index].accCycle[2] += currCnt[2]-romePerfStat[index].tempCycle[2];
	romePerfStat[index].accCycle[3] += currCnt[3]-romePerfStat[index].tempCycle[3];
	romePerfStat[index].hasTempCycle = FALSE;
	romePerfStat[index].executedNum++;
	CP3_COUNTER0_START();

	return SUCCESS;
}

int32 rtl8651_romeperfDump( int start, int end )
{
	int i;

	if( statSnapShot == NULL )
	{
		rtlglue_printf("statSnapShot mem alloc failed\n");
		return FAILED;
	}

	memset( &statSnapShot , 0, sizeof( statSnapShot ) );

	rtlglue_printf( "index %30s %12s %8s %10s\n", "description", "accCycle", "totalNum", "Average" );

	for( i = start; i <= end; i++ )
	{
		int j;
		for( j =0; j < sizeof(romePerfStat[i].accCycle)/sizeof(romePerfStat[i].accCycle[0]); j++ )
		{
			statSnapShot[i].accCycle[j]  = romePerfStat[i].accCycle[j];
			statSnapShot[i].tempCycle[j] = romePerfStat[i].tempCycle[j];
		}
		statSnapShot[i].executedNum  = romePerfStat[i].executedNum;
		statSnapShot[i].hasTempCycle = romePerfStat[i].hasTempCycle;
	}

	for( i = start; i <= end; i++ )
	{
		if ( statSnapShot[i].executedNum == 0 )
		{
			rtlglue_printf( "[%3d] %30s %12s %8s %10s\n", i, romePerfStat[i].desc, "--", "--", "--" );
		}
		else
		{
			int j;
			rtlglue_printf( "[%3d] %30s ", i, romePerfStat[i].desc );
			for( j =0; j < sizeof(statSnapShot[i].accCycle)/sizeof(statSnapShot[i].accCycle[0]); j++ )
			{
				uint32 *pAccCycle = (uint32*)&statSnapShot[i].accCycle[j];
				uint32 avrgCycle = /* Hi-word */ (pAccCycle[0]*(0xffffffff/statSnapShot[i].executedNum)) +
				                   /* Low-word */(pAccCycle[1]/statSnapShot[i].executedNum);
				rtlglue_printf( "%12llu %8u %10u\n",
				statSnapShot[i].accCycle[j],
				statSnapShot[i].executedNum,
				avrgCycle );
				rtlglue_printf( " %3s  %30s ", "", "" );
			}
			rtlglue_printf( "\r" );
		}
	}

	return SUCCESS;
}

int rtl865x_perf_proc_read( char *page, char **start, off_t off, int count, int *eof, void *data )
{
	unsigned long flags = 0;
	local_irq_save(flags);
	rtl8651_romeperfDump(79, 80);
	local_irq_restore(flags);
    return 0;
}

int rtl865x_perf_proc_write(struct file *file, const char *buffer,
              unsigned long count, void *data)
{
	char  tmpbuf[32];
	int start, end, num;
	unsigned long x;
	spin_lock_irq(x);
	if (buffer && !copy_from_user(tmpbuf, buffer, count))
	{
		tmpbuf[count] = '\0';
		if(!memcmp(tmpbuf, "flush", 5)){
			rtl8651_romeperfReset();
		}else{
			num = sscanf(tmpbuf, "%d %d", &start, &end);
			if (num !=  2) {
				printk("invalid romeperf parameter!\n");
				goto OUT;
			}

			if((start<0) ||(start>99)||(end<0) ||(end>99) ||(start>end)){
				printk("Bad index!\n");
				goto OUT;
			}

			rtl8651_romeperfDump(start, end);
		}
	}
OUT:
    spin_unlock_irq(x);
    return count;
}

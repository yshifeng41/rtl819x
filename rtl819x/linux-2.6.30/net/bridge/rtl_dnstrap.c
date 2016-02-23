#include <linux/version.h>

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/inetdevice.h>
#include <net/checksum.h>
#include <net/udp.h>
#include <linux/ctype.h>

#include <net/rtl/rtl_dnstrap.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
#define CONFIG_RTL_PROC_NEW
extern struct proc_dir_entry proc_root;
#endif

struct proc_dir_entry *dnstrap_proc_root = NULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
static struct proc_dir_entry *proc_domain = NULL;
static struct proc_dir_entry *proc_enable = NULL;
#endif
#define PROC_ROOT "rtl_dnstrap"
#define PROC_DOMAIN_NAME "domain_name"
#define PROC_ENABLE "enable"

unsigned char domain_name[80];
unsigned char dns_answer[] = { 0xC0, 0x0C, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x03, 0x84, 0x00, 0x04 };
#if 0
unsigned char dns_answer[] = { 0xC0, 0x0C, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04 };
#endif
int dns_filter_enable = 1;
#define DEFAULT_HOSTNAME "wifi.example.com"
void str_to_lower(char *s)
{
	if(s!=NULL)
	{
    	while (*s != '\0') 
		{
        	*s = __tolower(*s);
        	++s;
    	}
	}
}
int br_dns_packet_recap(struct sk_buff *skb)
{
	struct iphdr *iph;
	struct udphdr *udph;
	struct net_device *br0_dev; 
	struct in_device *br0_in_dev;
	dnsheader_t *dns_pkt;
	unsigned char mac[ETH_ALEN];
	unsigned int ip;
	unsigned short port;
	unsigned char *ptr = NULL;

	br0_dev = dev_get_by_name(&init_net,"br0"); 
	br0_in_dev = in_dev_get(br0_dev);

	if(!br0_dev || !br0_in_dev)	
	{
		if(br0_in_dev)
			in_dev_put(br0_in_dev);
		if(br0_dev)
			dev_put(br0_dev);
		return -1;
	}

	iph = ip_hdr(skb);
	udph = (void *)iph + iph->ihl*4;
	dns_pkt = (void *)udph + sizeof(struct udphdr);
	ptr = (void *)udph + udph->len;

	skb_put(skb, 16);

	/* swap mac address */
	memcpy(mac, eth_hdr(skb)->h_dest, ETH_ALEN);
	memcpy(eth_hdr(skb)->h_dest, eth_hdr(skb)->h_source, ETH_ALEN);
	memcpy(eth_hdr(skb)->h_source, mac, ETH_ALEN);

	/*swap ip address */
	ip = iph->saddr;
	iph->saddr = iph->daddr;
	iph->daddr = ip;
	iph->tot_len = htons(iph->tot_len+16);

	/* swap udp port */
	port = udph->source;
	udph->source = udph->dest;
	udph->dest = port;
	udph->len = htons(udph->len+16);
	dns_pkt->u = 0x8180;
	dns_pkt->qdcount = 1;
	dns_pkt->ancount = 1;
	dns_pkt->nscount = 0;
	dns_pkt->arcount = 0;

	/* pad Answers */
	memcpy(ptr, dns_answer, 12);
	memcpy(ptr+12, (unsigned char *)&br0_in_dev->ifa_list->ifa_address, 4);

	/* ip checksum */
	skb->ip_summed = CHECKSUM_NONE;
	iph->check = 0;
	iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);

	/* udp checksum */
	udph->check = 0;
	udph->check = csum_tcpudp_magic(iph->saddr, iph->daddr,
					udph->len, IPPROTO_UDP,
					csum_partial((char *)udph,
					             udph->len, 0));
    if(br0_in_dev)
        in_dev_put(br0_in_dev);
    if(br0_dev)
        dev_put(br0_dev);

	return 1;
}

static int get_domain_name(unsigned char* dns_body,char* domain_name,int body_len)
{
	int len = 0,offset = 0,token_len = 0;
	char token[64] = {0};
	char domain[128] = {0};
	if(!dns_body || !domain_name || body_len <= 0){
		return -1;
	}
	while(body_len){
		memset(token,0,sizeof(token));
		token_len = dns_body[offset];
		if(token_len){
			strncpy(token,dns_body+offset+1,token_len);
			if(!domain[0]){
				strcpy(domain,token);
			}
			else{
				strcat(domain,".");
				strcat(domain,token);
			}
		}
		token_len +=1;
		body_len -= token_len;
		offset += token_len;
	}
	strcpy(domain_name,domain);
	return 0;
}
static int is_domain_name_equal(char *domain_name1, char * domain_name2)
{
	char temp1[128];
	char temp2[128];
	if(!domain_name1 || !domain_name2)
	{
		return 0;
	}
	str_to_lower(domain_name1);
	str_to_lower(domain_name2);
	if(!strncmp(domain_name1,"www.",4)){
		strcpy(temp1,domain_name1+4);
	}
	else{
		strcpy(temp1,domain_name1);
	}
	if(!strncmp(domain_name2,"www.",4)){
		strcpy(temp2,domain_name2+4);
	}
	else{
		strcpy(temp2,domain_name2);
	}
	if(strcmp(temp1,temp2))
		return 0;
	else
		return 1;
}
int br_dns_filter_enter(struct sk_buff *skb)
{
	struct iphdr *iph;
	struct udphdr *udph;
	unsigned char *body = NULL;
	int len = 0;
	char domain[128] = {0};
	iph = (struct iphdr *)skb_network_header(skb);
	udph = (void *)iph + iph->ihl*4;
	if (iph->protocol==IPPROTO_UDP && udph->dest == 53&& ((iph->frag_off & htons(0x3FFF))==0)&&(iph->tot_len-udph->len>=20)) {
		//DBGP_DNS_TRAP("[%s:%d]DNS packet\n",__FUNCTION__,__LINE__);
		len = udph->len - sizeof(struct udphdr) - sizeof(dnsheader_t) - 4;
		if(len > 63)
		{
			return 1;
		}

		body = (void *)udph + sizeof(struct udphdr) + sizeof(dnsheader_t);
		//DBGP_DNS_TRAP("[%s:%d]DNS packet urlis[%s]\n",__FUNCTION__,__LINE__,body);
		get_domain_name(body,domain,len);
		DBGP_DNS_TRAP("[%s:%d]domain_name is %s\n",__FUNCTION__,__LINE__,domain);
		if(is_domain_name_equal(domain,domain_name)){
			DBGP_DNS_TRAP("[%s:%d]%s matched!!!\n",__FUNCTION__,__LINE__,domain);
			br_dns_packet_recap(skb);
		}
		return 1;
	}

	return 0;
}

int is_dns_packet(struct sk_buff *skb)
{
	struct iphdr *iph;
	struct udphdr *udph;
	iph = (void *)skb->data;
	if (iph) {
		udph = (void *)iph + iph->ihl*4;
		if (iph->protocol==IPPROTO_UDP && (udph->dest == 53 || udph->source == 53)) {
			return 1;
		}
	}
	return 0;
}
#if defined(CONFIG_RTL_PROC_NEW)
static int dnstrap_en_read(struct seq_file *s, void *v)
{
	seq_printf(s,"%d\n",dns_filter_enable);
	return 0;
}
#else
static int dnstrap_en_read(char *page, char **start, off_t off,
		     int count, int *eof, void *data)
{

	int len=0;
	len = sprintf(page, "%d\n", dns_filter_enable);
	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;

	return len;
}
#endif
static int dnstrap_en_write(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	char tmpbuf[80];

	if (count < 2)
		return -EFAULT;
	
	if (buffer && !copy_from_user(tmpbuf, buffer, count))  {
		tmpbuf[count] = '\0';
		if (tmpbuf[0] == '0')
			dns_filter_enable = 0;
		else if (tmpbuf[0] == '1')
			dns_filter_enable = 1;
		return count;
	}
	return -EFAULT;
}
#ifdef CONFIG_RTL_PROC_NEW
int dnstrap_en_proc_open(struct inode *inode, struct file *file)
{
	return(single_open(file, dnstrap_en_read,NULL));
}
int dnstrap_en_proc_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return dnstrap_en_write(file,userbuf,count,off);
}

struct file_operations dnstrap_en_proc_fops= {
        .open           = dnstrap_en_proc_open,
        .write		    = dnstrap_en_proc_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif

#if defined(CONFIG_RTL_PROC_NEW)
static int dnstrap_domain_read(struct seq_file *s, void *v)
{
	seq_printf(s,"%s\n", domain_name);
	return 0;
}
#else
static int dnstrap_domain_read(char *page, char **start, off_t off,
		     int count, int *eof, void *data)
{

	int len=0;
	len = sprintf(page, "%s\n", domain_name);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;

	return len;
}
#endif
static int dnstrap_domain_write(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	if (count < 2)
		return -EFAULT;

	if (buffer && !copy_from_user(domain_name, buffer, 80)) {
		domain_name[count-1] = 0;
		str_to_lower(domain_name);
		return count;
	}

	return -EFAULT;
}
#ifdef CONFIG_RTL_PROC_NEW
int dnstrap_domain_proc_open(struct inode *inode, struct file *file)
{
	return(single_open(file, dnstrap_domain_read,NULL));
}
int dnstrap_domain_proc_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return dnstrap_domain_write(file,userbuf,count,off);
}

struct file_operations dnstrap_domain_proc_fops= {
        .open           = dnstrap_domain_proc_open,
        .write		    = dnstrap_domain_proc_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif
#if defined(CONFIG_PROC_FS)
static void dnstrap_create_proc(void)
{
#if defined(CONFIG_RTL_PROC_NEW)
	dnstrap_proc_root = proc_mkdir(PROC_ROOT,&proc_root);
	if(dnstrap_proc_root){
		proc_create_data(PROC_DOMAIN_NAME,0,dnstrap_proc_root,&dnstrap_domain_proc_fops,NULL);
		proc_create_data(PROC_ENABLE,0,dnstrap_proc_root,&dnstrap_en_proc_fops,NULL);
	}
#else
	dnstrap_proc_root = proc_mkdir(PROC_ROOT, NULL);
	if (!dnstrap_proc_root){
		printk("create folder fail\n");
		return;
	}
	proc_enable = create_proc_entry(PROC_ENABLE, 0, dnstrap_proc_root);
	if (proc_enable) {
		proc_enable->read_proc = dnstrap_en_read;
		proc_enable->write_proc = dnstrap_en_write;
	}
	proc_domain = create_proc_entry(PROC_DOMAIN_NAME, 0, dnstrap_proc_root);
	if (proc_domain) {
		proc_domain->read_proc = dnstrap_domain_read;
		proc_domain->write_proc = dnstrap_domain_write;
	}
#endif
}
static void dnstrap_destroy_proc(void)
{
#if defined(CONFIG_RTL_PROC_NEW)
	if(dnstrap_proc_root){
		remove_proc_entry(PROC_DOMAIN_NAME, &dnstrap_proc_root);		
		remove_proc_entry(PROC_ENABLE, &dnstrap_proc_root);		
		remove_proc_entry(PROC_ROOT, &proc_root);		
	}
#else
	if(dnstrap_proc_root){
		if (proc_enable) {
			remove_proc_entry(PROC_ENABLE, dnstrap_proc_root);
			proc_enable = NULL;
		}

		if (proc_domain) {
			remove_proc_entry(PROC_DOMAIN_NAME, dnstrap_proc_root);
			proc_domain = NULL;
		}

		remove_proc_entry(PROC_ROOT, NULL);
		dnstrap_proc_root = NULL;
	}
#endif
}
#endif
int __init br_dns_filter_init(void)
{
#if defined(CONFIG_PROC_FS)
	dnstrap_create_proc();
#endif
	memset(domain_name,0,sizeof(domain_name));
	strcpy(domain_name,DEFAULT_HOSTNAME);
	str_to_lower(domain_name);
	return 0;
}

void __exit br_dns_filter_exit(void)
{
#if defined(CONFIG_PROC_FS)
	dnstrap_destroy_proc();
#endif
}


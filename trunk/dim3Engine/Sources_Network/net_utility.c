/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Network Utility Code

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#ifdef D3_OS_MAC
	#include <ifaddrs.h>
#endif

#include "network.h"

extern setup_type			setup;
extern hud_type				hud;

int							net_proj_hash;
char						*net_news;

/* =======================================================

      Network Initialize/Shutdown
      
======================================================= */

void net_initialize(void)
{
#ifdef D3_OS_WINDOWS
	WSADATA				wsaData;

	WSAStartup(MAKEWORD(2,2),&wsaData);
#endif

		// hash and news not loaded yet
		
	net_proj_hash=0;
	net_news=NULL;
}

void net_shutdown(void)
{
		// free news
		
	if (net_news!=NULL) free(net_news);
	
#ifdef D3_OS_WINDOWS
	WSACleanup();
#endif
}

/* =======================================================

      Project Hash and News
      
======================================================= */

void net_create_project_hash(void)
{
		// skip if already loaded
		
	if (net_proj_hash!=0) return;
	
		// scan data directory to build hash
		
	net_proj_hash=file_paths_project_hash(setup.file_path_setup.path_data);
}

int net_get_project_hash(void)
{
	return(net_proj_hash);
}

void net_load_news(void)
{
	char		err_str[256];
	
		// any news to load?
		
	if (hud.net_news.host[0]==0x0) return;
	
		// skip if already loaded
		
	if (net_news!=NULL) return;
	
		// load net news
		
	net_news=net_get_http_file(hud.net_news.host,hud.net_news.port,hud.net_news.url,err_str);
	if (net_news!=NULL) return;
	
		// build error message
		
	net_news=malloc(512);
	if (net_news==NULL) return;
	
	sprintf(net_news,"News Read Failure\n%s",err_str);
}

char* net_get_news(void)
{
	return(net_news);
}

/* =======================================================

      Host Names and IPs
      
======================================================= */

void net_get_host_name(char *name)
{
	char			*c;
	
	gethostname(name,256);
	name[255]=0x0;
	
	c=strchr(name,'.');
	if (c!=NULL) *c=0x0;
}

void net_get_host_ip(char *ip_name,char *ip_resolve)
{
	int					addr[4];
	unsigned long		ul;
	bool				resolved;
	struct hostent		*hent;

#ifdef D3_OS_MAC
	struct ifaddrs		*ifap_get,*ifap;
	struct sockaddr_in	sockin;
#endif

		// get host name

	gethostname(ip_name,256);
	ip_name[255]=0x0;

		// resolve host name

		// first try to use gethostbyname.  There's some
		// obscure OS X bug(?) that won't resolve when on
		// a wireless card, so if the name can't be looked
		// up in the hosts file, then find it by searching
		// all the network interfaces.  If all that fails,
		// just set it to localhost.

	ul=0;
	resolved=FALSE;

	hent=gethostbyname(ip_name);
	if (hent!=NULL) {
		memmove(&ul,hent->h_addr,sizeof(unsigned long));
		resolved=TRUE;
	}

	if (!resolved) {

#ifdef D3_OS_MAC
	
		if (getifaddrs(&ifap_get)==0) {
		
			ifap=ifap_get;
			
			while (TRUE) {
				if (ifap->ifa_addr!=NULL) {
					if (ifap->ifa_addr->sa_family==AF_INET) {
						if (memcmp(ifap->ifa_name,"en",2)==0) {
							memmove(&sockin,ifap->ifa_addr,sizeof(sockin));
							memmove(&ul,&sockin.sin_addr,sizeof(unsigned long));
							resolved=TRUE;
							if (strcmp(ifap->ifa_name,"en0")==0) break;		// if en0, break, otherwise get last enX
						}
					}
				}
				
				ifap=ifap->ifa_next;
				if (ifap==NULL) break;
			}
			
			freeifaddrs(ifap_get);
		}

#endif

	}

		// get textual resolved name
	
	if (resolved) {

#ifdef __BIG_ENDIAN__
		addr[0]=(ul>>24)&0xFF;
		addr[1]=(ul>>16)&0xFF;
		addr[2]=(ul>>8)&0xFF;
		addr[3]=ul&0xFF;
#else
		addr[3]=(ul>>24)&0xFF;
		addr[2]=(ul>>16)&0xFF;
		addr[1]=(ul>>8)&0xFF;
		addr[0]=ul&0xFF;
#endif

		sprintf(ip_resolve,"%d.%d.%d.%d",addr[0],addr[1],addr[2],addr[3]);
	}
	else {
		strcpy(ip_resolve,"127.0.0.1");
	}
}

/* =======================================================

      Local IP Checks
      
======================================================= */

bool net_ip_local(char *ip)
{
	bool			local;
	
	local=FALSE;
	local=local||(memcmp(ip,"10.",3)==0);
	local=local||(memcmp(ip,"172.16.",7)==0);
	local=local||(memcmp(ip,"172.17.",7)==0);
	local=local||(memcmp(ip,"172.18.",7)==0);
	local=local||(memcmp(ip,"172.19.",7)==0);
	local=local||(memcmp(ip,"172.20.",7)==0);
	local=local||(memcmp(ip,"172.21.",7)==0);
	local=local||(memcmp(ip,"172.22.",7)==0);
	local=local||(memcmp(ip,"172.23.",7)==0);
	local=local||(memcmp(ip,"172.24.",7)==0);
	local=local||(memcmp(ip,"172.25.",7)==0);
	local=local||(memcmp(ip,"172.26.",7)==0);
	local=local||(memcmp(ip,"172.27.",7)==0);
	local=local||(memcmp(ip,"172.28.",7)==0);
	local=local||(memcmp(ip,"172.29.",7)==0);
	local=local||(memcmp(ip,"172.30.",7)==0);
	local=local||(memcmp(ip,"172.31.",7)==0);
	local=local||(memcmp(ip,"172.32.",7)==0);
	local=local||(memcmp(ip,"192.168.",8)==0);
	
	return(local);
}

void net_ip_local_broadcast(char *ip,char *broad_ip)
{
	char			*c;
	
	if (!net_ip_local(ip)) {
		strcpy(broad_ip,ip);
		return;
	}
	
		// replace last hex byte with 255
		
	strcpy(broad_ip,ip);
	
	c=strrchr(broad_ip,'.');
	if (c==NULL) return;

	c++;
	strcpy(c,"255");
}



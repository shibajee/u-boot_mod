/*
 *	Copyright 1994, 1995, 2000 Neil Russell.
 *	(See License)
 *	Copyright 2000, 2001 DENX Software Engineering, Wolfgang Denk, wd@denx.de
 */

#include <common.h>
#include <command.h>
#include <net.h>
#include <gpio.h>
#include <asm/byteorder.h>

#if defined(CFG_CMD_HTTPD)
#include "httpd.h"

#include "../httpd/uipopt.h"
#include "../httpd/uip.h"
#include "../httpd/uip_arp.h"

static int arptimer = 0;

void HttpdHandler(void){
	int i;
	struct uip_eth_hdr *eth_hdr = (struct uip_eth_hdr *)uip_buf;
	if (uip_len == 0) {
		for(i = 0; i < UIP_CONNS; i++){
			uip_periodic(i);
			if(uip_len > 0){
				uip_arp_out();
				NetSendHttpd();
			}
		}

		if(++arptimer == 20){
			uip_arp_timer();
			arptimer = 0;
		}
	} else {
		if (eth_hdr->type == htons(UIP_ETHTYPE_IP)) {
			uip_arp_ipin();
			uip_input();
			if(uip_len > 0){
				uip_arp_out();
				NetSendHttpd();
			}
		} else if (eth_hdr->type == htons(UIP_ETHTYPE_ARP)) {
			uip_arp_arpin();
			if(uip_len > 0){
				NetSendHttpd();
			}
		}
	}
}

// start http daemon
void HttpdStart(void){
	uip_init();
	httpd_init();
}

int flash_erase_write(char *buf, unsigned int offs, int count) {
	int result;
#if defined (CFG_ENV_IS_IN_NAND)
	result = ranand_erase_write(buf, offs, count);
#elif defined (CFG_ENV_IS_IN_SPI)
	result = raspi_erase_write(buf, offs, count);
#else
	unsigned long e_end = CFG_FLASH_BASE + offs + count - 1;
	if (get_addr_boundary(&e_end) != 0)
		return -1;

	flash_sect_erase(CFG_FLASH_BASE + offs, e_end);
	result = flash_write(buf, CFG_FLASH_BASE + offs, count);
#endif
	return result;
}

int do_http_upgrade(const ulong size, const int upgrade_type){
	if(upgrade_type == WEBFAILSAFE_UPGRADE_TYPE_UBOOT){
		printf("\n\n****************************\n*     U-BOOT UPGRADING     *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
		return( flash_erase_write((char *) WEBFAILSAFE_UPLOAD_RAM_ADDRESS, WEBFAILSAFE_UPLOAD_UBOOT_ADDRESS - CFG_FLASH_BASE, WEBFAILSAFE_UPLOAD_UBOOT_SIZE_IN_BYTES ) );
	} else if(upgrade_type == WEBFAILSAFE_UPGRADE_TYPE_FIRMWARE){
		printf("\n\n****************************\n*    FIRMWARE UPGRADING    *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
		return( flash_erase_write((char *) WEBFAILSAFE_UPLOAD_RAM_ADDRESS, WEBFAILSAFE_UPLOAD_KERNEL_ADDRESS - CFG_FLASH_BASE, size ) );
	} else if(upgrade_type == WEBFAILSAFE_UPGRADE_TYPE_FACTORY){
		printf("\n\n****************************\n*    FACTORY  UPGRADING    *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
		return( flash_erase_write((char *) WEBFAILSAFE_UPLOAD_RAM_ADDRESS, WEBFAILSAFE_UPLOAD_FACTORY_ADDRESS - CFG_FLASH_BASE, WEBFAILSAFE_UPLOAD_FACTORY_SIZE_IN_BYTES ) );
	} else {
		return(-1);
	}
	return(-1);
}

// info about current progress of failsafe mode
int do_http_progress(const int state){
	unsigned char i = 0;

	/* toggle LED's here */
	switch(state){
		case WEBFAILSAFE_PROGRESS_START:

			// blink LED fast 10 times
			for(i = 0; i < 10; ++i){
				LED_ALERT_ON();
				milisecdelay(25);
				LED_ALERT_OFF();
				milisecdelay(25);
			}

			printf("HTTP server is ready!\n\n");
			break;

		case WEBFAILSAFE_PROGRESS_TIMEOUT:
			//printf("Waiting for request...\n");
			break;

		case WEBFAILSAFE_PROGRESS_UPLOAD_READY:
			printf("HTTP upload is done! Upgrading...\n");
			break;

		case WEBFAILSAFE_PROGRESS_UPGRADE_READY:
			printf("HTTP ugrade is done! Rebooting...\n\n");
			break;

		case WEBFAILSAFE_PROGRESS_UPGRADE_FAILED:
			printf("*** ERROR: HTTP ugrade failed!\n\n");

			// blink LED fast for 4 sec
			for(i = 0; i < 80; ++i){
				LED_ALERT_ON();
				milisecdelay(25);
				LED_ALERT_OFF();
				milisecdelay(25);
			}

			// wait 1 sec
			milisecdelay(1000);

			break;
	}

	return(0);
}
#endif /* CFG_CMD_HTTPD */

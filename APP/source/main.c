#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "brahma.h"
#include "hid.h"
#include "menus.h"
#include "sochlp.h"

u8 isN3DS = 0;
int type;

s32 quick_boot_firm (s32 load_from_disk) {
	if (load_from_disk)
	if (load_arm9_payload("/3ds/ASCFW/ASCFW.bin") == 1)firm_reboot();	
}

int checkSystemVersion()
{
	//FIRSTLY, CHECK NEW 3DS
	APT_CheckNew3DS(NULL, &isN3DS);
	if (isN3DS) {
		type = 4;
	}
	else
	{
		//Now we really check system version, based off kversion
		unsigned int kversion = *(unsigned int *)0x1FF80000;
		if (kversion == 0x02220000) {
			type = 1;
		}
		else if (kversion == 0x022C0600){
			type = 2;
		}
		else if (kversion == 0x022E0000) {
			type = 3;
		}
		else {
			type = 0;
		}
	}
	if (type == 0)return 0;
	else return 1;
}

s32 main (void) {
	// Initialize services
	srvInit();
	aptInit();
	hidInit(NULL);
	gfxInitDefault();
	fsInit();
	sdmcInit();
	hbInit();
	qtmInit();
	
	consoleInit(GFX_BOTTOM, NULL);
	if (brahma_init()) {
		hidScanInput();
		u32 kHeld = hidKeysHeld();
	 
		if (kHeld & KEY_L)
		{
			/* reboot only */
			quick_boot_firm(0);
		}
		else 
		{
            /* load default payload from dosk and run exploit */
			//Check kernel version first
			if (checkSystemVersion() == 0)
			{
				printf("ERROR: System version not supported. \n");
				wait_any_key();
				quick_boot_firm(0);
			}
			else
			{
				FILE *f = fopen("/3ds/ASCFW/system.txt", "w");
				fprintf(f, "%d", type);
				fclose(f);
				quick_boot_firm(1);
				printf("ERROR: Something went wrong. \n");
				wait_any_key();
				quick_boot_firm(0);
			}
		}
		brahma_exit();

	} else {
		printf("ERROR: Not enough memory. \n");
		wait_any_key();
		quick_boot_firm(0);
	}

	hbExit();
	sdmcExit();
	fsExit();
	gfxExit();
	hidExit();
	aptExit();
	srvExit();
	// Return to hbmenu
	return 0;
}

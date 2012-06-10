// LvivGOO
// Emulator of soviet computer LVIV PK-01
// Copyright (C) Alexander Timoshenko 2012

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <dingoo/slcd.h>
#include <dingoo/cache.h>
#include <jz4740/cpu.h>
#include <sml/control.h>
#include <sml/timer.h>
#include <sml/graphics.h>
#include <sml/display.h>

#include "I8080.h"            /* I8080 CPU emulation             */
#include "I8255.h"
#include "ram.h"
#include "rom.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define SCANLINE_MAX 256

I8080  		CPU;
I8255		PPI[2];
uint8_t		VRAM[0x4000];
uint8_t 	screen[256][256];
uint8_t 	KEY_BASE[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t	 	KEY_EXT[4] = {0xFF,0xFF,0xFF,0xFF};

display*        appDisplay  = NULL;
gfx_texture*    appImage    = NULL;
char			tempFileName[256];
bool            error = false;
bool            appRunning = true;
timer*       	appTimer = NULL;

bool			ROM_ON;

int ref = 1;
//extern int _sys_judge_event(void *);

uint16_t	ScanLine = 0;

// Calculates color value from palette port
uint16_t COL(uint8_t color)
{
	uint16_t RED = 0xF800;
	uint16_t GREEN = 0x07E0;
	uint16_t BLUE = 0x001F;
	uint16_t Result = 0;

	uint8_t port = PPI[0].Rout[1];
	if ((port & 0x40) != 0) Result ^= BLUE;
	if ((port & 0x20) != 0) Result ^= GREEN;
	if ((port & 0x10) != 0) Result ^= RED;
	switch (color)
	{
		case 0:
			if ((port & 0x08) == 0) Result ^= RED;
			if ((port & 0x04) == 0) Result ^= BLUE;
			break;
		case 1:
			Result ^= BLUE;
			if ((port & 0x01) == 0) Result ^= RED;
			break;
		case 2:
			Result ^= GREEN;
			break;
		case 3:
			Result ^= RED;
			if ((port & 0x02) == 0) Result ^= GREEN;
			break;
	}
	return Result;
}

/*
void framebuffer(void)
{
	uint16_t x,y;
	uint32_t c;
	uint16_t* tempPtr = (uint16_t*)(gfx_render_target->address) + 32;

	for (y = 0; y < 256; y++)
	{
		c = (y * gfx_render_target->width);

		for (x = 0; x < 256; x++)
		{
			tempPtr[c + x] = COL(screen[x][y]);
		}
	}
}
*/

void linebuffer(word y)
{
	uint16_t x;
    uint16_t* tempPtr = (uint16_t*)(gfx_render_target->address) + (y * gfx_render_target->width) + 32;

    for (x = 0; x < 256; x++)
   	{
    	tempPtr[x] = COL(screen[x][y]);
   	}
}

void ResetComp(void)
{
	ROM_ON = TRUE;

	Reset8255(&PPI[0]);
	Reset8255(&PPI[1]);

	CPU.IAutoReset = 1;
	CPU.IPeriod = 192;

	ScanLine = 0;

	ResetCPU();
}

/** WrCPU() **************************************************/
/** CPU emulation calls this function to write byte V to    **/
/** address A of CPU address space.                         **/
/*************************************************************/
void WrCPU(word A, byte V)
{
	if (A < 0xC000)
	{
		if (PPI[0].Rout[2] & 0x02)	// 1 - Close Video RAM
		{
			RAM[A] = V;
		}
		else
		{
			if (A >= 0x4000)
			{
				if (A < 0x8000)
				{
					VRAM[A & 0x3FFF] = V;

					// update framebuffer
					uint8_t y = (A >> 6);
					uint8_t x = (A << 2);
					uint8_t c;
					for (c=0; c<4; c++)
					{
						screen[x++][y] = ((V & 0x80) >> 6) | ((V & 0x08) >> 3);
						V <<= 1;
					}
				}
				else
				{
					RAM[A] = V;
				}
			}
		}
	}
}

/** RdCPU() **************************************************/
/** CPU emulation calls this function to read a byte from   **/
/** address A of CPU address space.                         **/
/*************************************************************/
byte RdCPU(word A)
{
	if ((ROM_ON) && (A < 0x4000))
	{
		return ROM[A];
	}
	else
	if (PPI[0].Rout[2] & 0x02)	// 1 - Close Video RAM (default)
	{
		if (A < 0xC000)
		{
			return RAM[A];
		}
		else
		{
			return ROM[A & 0x3FFF];
		}
	}
	else
	{
		if (A >= 0xC000)
		{
			return ROM[A & 0x3FFF];
		}
		else
		{
			if (A < 0x4000)
			{
				return 0;
			}
			else
			{
				if (A >= 0x8000)
				{
					return RAM[A];
				}
				else
				{
					return VRAM[A & 0x3FFF];
				}
			}
		}
	}
}

/** OutCPU() *************************************************/
/** CPU emulation calls this function to write byte V to a  **/
/** given I/O port.                                         **/
/*************************************************************/
void OutCPU(word Port,byte Value)
{
	uint8_t KK;

	Write8255(&PPI[(Port & 0x10) >> 4],Port & 0x03,Value);
	switch (Port)
	{
		// Port A (D1)
		case 0xD0:
			KK = 0xFF;
			if ((PPI[1].Rout[0] & 0x80) == 0) KK &= KEY_BASE[7];
			if ((PPI[1].Rout[0] & 0x40) == 0) KK &= KEY_BASE[6];
			if ((PPI[1].Rout[0] & 0x20) == 0) KK &= KEY_BASE[5];
			if ((PPI[1].Rout[0] & 0x10) == 0) KK &= KEY_BASE[4];
			if ((PPI[1].Rout[0] & 0x08) == 0) KK &= KEY_BASE[3];
			if ((PPI[1].Rout[0] & 0x04) == 0) KK &= KEY_BASE[2];
			if ((PPI[1].Rout[0] & 0x02) == 0) KK &= KEY_BASE[1];
			if ((PPI[1].Rout[0] & 0x01) == 0) KK &= KEY_BASE[0];
			PPI[1].Rin[1] = KK;
			break;
		// Port C (D1)
		case 0xD2:
			KK = 0xFF;
			if ((PPI[1].Rout[2] & 0x08) == 0) KK &= KEY_EXT[3];
			if ((PPI[1].Rout[2] & 0x04) == 0) KK &= KEY_EXT[2];
			if ((PPI[1].Rout[2] & 0x02) == 0) KK &= KEY_EXT[1];
			if ((PPI[1].Rout[2] & 0x01) == 0) KK &= KEY_EXT[0];
			PPI[1].Rin[2] = KK;
			break;
	}
	ROM_ON = FALSE;
}

/** InCPU() **************************************************/
/** CPU emulation calls this function to read a byte from   **/
/** a given I/O port.                                       **/
/*************************************************************/
byte InCPU(word Port)
{
	return Read8255(&PPI[(Port & 0x10) >> 4],Port & 0x03);
}

void gameScreenshot() {
	char tempString[256];
	unsigned long int tempNumber = 0;
	FILE* tempFile;
	while(true) {
		sprintf(tempString, "lviv%lu.tga", tempNumber);
		tempFile = fopen(tempString, "rb");
		if(tempFile == NULL)
			break;
		fclose(tempFile);
		tempNumber++;
	}
	gfx_tex_save_tga(tempString, gfx_render_target);
}

void update_keys(void)
{
	control_poll();
	// BUTTON SELECT PRESSED
	if ((control_check(CONTROL_BUTTON_SELECT).pressed) && (control_check(CONTROL_BUTTON_START).pressed))
  	{
		appRunning = FALSE;
  	}
  	else
  	{
	  	// "up-left"
	  	//if (control_check(CONTROL_TRIGGER_LEFT).pressed)	KEY[1] &= 0xFE; else KEY[0] |= 0x01;
	  	// "ctr"
	  	//if (control_check(CONTROL_TRIGGER_RIGHT).pressed)	KEY[1] &= 0xFD; else KEY[0] |= 0x02;
	  	// "left"
	  	if (control_check(CONTROL_DPAD_LEFT).pressed)		KEY_EXT[3] &= 0xBF; else KEY_EXT[3] |= 0x40;
	  	// "right"
	  	if (control_check(CONTROL_DPAD_RIGHT).pressed)		KEY_EXT[3] &= 0xEF; else KEY_EXT[3] |= 0x10;
	  	// "up"
	  	if (control_check(CONTROL_DPAD_UP).pressed)			KEY_EXT[3] &= 0xDF; else KEY_EXT[3] |= 0x20;
	  	// "down"
	  	if (control_check(CONTROL_DPAD_DOWN).pressed)		KEY_EXT[3] &= 0x7F; else KEY_EXT[3] |= 0x80;
	  	// "space"
  	  	if (control_check(CONTROL_BUTTON_A).pressed)		KEY_BASE[3] &= 0xFE; else KEY_BASE[3] |= 0x01;
  	  	// "enter"
  	  	if (control_check(CONTROL_BUTTON_B).pressed)		KEY_BASE[1] &= 0xF7; else KEY_BASE[1] |= 0x08;
  	  	// N
  	  	//if (control_check(CONTROL_BUTTON_X).pressed)		KEY[5] &= 0xBF; else KEY[5] |= 0x40;
  	  	// Y / D
/*  	  	if (control_check(CONTROL_BUTTON_Y).pressed)
  	  	{
  	  		KEY[7] &= 0xFD;		// Y
  	  		KEY[4] &= 0xEF;		// D
  	  	}
  	  	else
  	  	{
  	  		KEY[7] |= 0x02;		// Y
  	  		KEY[4] |= 0x10;		// D
  	  	}*/
  	}
}

/** LoopCPU() ************************************************/
/** Refresh screen, check keyboard and sprites. Call this   **/
/** function on each interrupt.                             **/
/*************************************************************/
word LoopCPU()
{
	static byte frame_skip;
//	int sysref;
//	sysref = _sys_judge_event(NULL);
//	if (sysref < 0)
//	{
//			//word ref = sysref;
//			return 0;
//	}

	linebuffer(ScanLine++);
	if (ScanLine == SCANLINE_MAX)
	{
  	  	if (frame_skip == 1)
  	  	{
  	  		display_flip(appDisplay);
  	  		frame_skip = 0;
  	  	}
  	  	else
  	  	{
  	  		frame_skip++;
  	  	}
  	  	ScanLine = 0;
  	  	update_keys();
	}

  	if (appRunning)
  	{
  		CPU.IRequest = INT_NONE;
  	}
  	else
  	{
  		CPU.IRequest = INT_QUIT;
  	}
  	return(CPU.IRequest);
}

int rnd( int max ) {
    return (rand() % max) + 1;
  }

//
int main(int argc, char** argv)
{
	FILE* fp;

	appDisplay = display_create(320, SCANLINE_MAX, 320, (DISPLAY_FORMAT_RGB565 | DISPLAY_STRETCH | DISPLAY_FILTER_LINEAR), NULL, NULL);
	//appDisplay = display_create(320, SCANLINE_MAX, 320, (DISPLAY_FORMAT_RGB565 | DISPLAY_STRETCH), NULL, NULL);
	//appDisplay = display_create(320, 240, 320, (DISPLAY_FORMAT_RGB565), NULL, NULL);
	if(appDisplay == NULL)
	{
		return ref;
	}

	//cpu_clock_set(400000000);

	control_init();
	control_lock(timer_resolution / 4);

	gfx_init(appDisplay);

	// clear screen (I)
	display_clear(appDisplay);
	// clear screen (II)
	//gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
	//display_flip(appDisplay);

	uint16_t i;
	int c;

	appTimer = timer_create();

	ResetComp();

	// load RAM
	sprintf(tempFileName, "%s", "LVIV.RAM");
	fp = fopen(tempFileName, "rb");
	if (fp != 0)
	{
		fread(&RAM, 0xC000, 1, fp);
		fclose(fp);
	}
	// load ROM
	sprintf(tempFileName, "%s", "LVIV.ROM");
	fp = fopen(tempFileName, "rb");
	if (fp != 0)
	{
		fread(&ROM, 0x4000, 1, fp);
		fclose(fp);
	}
	// load LVT/SAV
	sprintf(tempFileName, "%s", argv[0]); // In SIM files argv[0] contains the target to be loaded

	for (i = 0; i < strlen(tempFileName); i++)
	{
		tempFileName[i] = toupper(tempFileName[i]);
	}

	fp = fopen(tempFileName, "rb");
	if ((fp != 0) && (strstr(tempFileName,".LVT") || strstr(tempFileName,".SAV")))
	{
		struct header {
		  char file_hdr[9];
		  byte file_type;
		  char file_name[6];
		  word addr_begin;
		  word addr_end;
		  word addr_start;
		} hdr;

		fread(&hdr, sizeof(struct header), 1, fp);

		switch (hdr.file_type)
		{
			// SAV dump
			case 0x2F:
				fseek(fp, 17, SEEK_SET);
				fread(&RAM, 0xC000, 1, fp);
				fread(&ROM, 0x4000, 1, fp);
				fread(&VRAM, 0x4000, 1, fp);

				// VRAM refresh
				OutCPU(0xC3,0x88);
				OutCPU(0xC2,0x00);
				for(i=0; i<0x4000; i++) WrCPU(0x4000+i,VRAM[i]);

				fseek(fp, 0x140D4, SEEK_SET);
				OutCPU(0xC3,fgetc(fp));
				fseek(fp, 0x140E4, SEEK_SET);
				OutCPU(0xD3,fgetc(fp));
				fseek(fp, 0x140D1, SEEK_SET);
				for(i=0; i<3; i++) OutCPU(0xC0+i,fgetc(fp));
				fseek(fp, 0x140E1, SEEK_SET);
				for(i=0; i<3; i++) OutCPU(0xD0+i,fgetc(fp));
				fseek(fp, 0x14111, SEEK_SET);
				CPU.BC.B.h = fgetc(fp);
				CPU.BC.B.l = fgetc(fp);
				CPU.DE.B.h = fgetc(fp);
				CPU.DE.B.l = fgetc(fp);
				CPU.HL.B.h = fgetc(fp);
				CPU.HL.B.l = fgetc(fp);
				CPU.AF.B.h = fgetc(fp);
				CPU.AF.B.l = fgetc(fp);
				CPU.SP.B.l = fgetc(fp);
				CPU.SP.B.h = fgetc(fp);
				CPU.PC.B.l = fgetc(fp);
				CPU.PC.B.h = fgetc(fp);
				break;
			// LVT code
			case 0xD0:
				fread(&RAM[hdr.addr_begin], hdr.addr_end-hdr.addr_begin+1, 1, fp);
				OutCPU(0xC3,0x88);
				OutCPU(0xD3,0x8A);
				OutCPU(0xC2,0x02);
				OutCPU(0xC1,0x8F);
				CPU.SP.W = 0xBF99;
				WrCPU(0xBFEC,hdr.addr_start);
				WrCPU(0xBFED,hdr.addr_start >> 8);
				CPU.PC.W = hdr.addr_start;
				break;
			// LVT basic
			case 0xD3:
				OutCPU(0xC3,0x88);
				OutCPU(0xD3,0x8A);
				OutCPU(0xC2,0x12);
				OutCPU(0xC1,0x8F);
				//CPU.SP.W = 0xAFC1;						// BasicStack = 0xAFC1
				//WrCPU(0xAFC1,0xFD);						// BasicHotEntry = 0x02FD;
				//WrCPU(0xAFC2,0x02);
				//CPU.PC.W = 0x02FD;					// BasicHotEntry = 0x02FD;
				//WrCPU(0xAFC1,0xAB);
				//WrCPU(0xAFC2,0x06);
				//CPU.PC.W = 0x06AB;
				CPU.AF.W = 0x0054;
				CPU.BC.W = 0x06AB;
				CPU.DE.W = 0x00A2;
				CPU.HL.W = 0x0109;
				CPU.SP.W = 0xAFC9;
				CPU.PC.W = 0x06AB;

				i = RAM[0x0243] + (RAM[0x0244]*256);	// BasicProgBegin = 0x0243;
				fseek(fp, 16, SEEK_SET);
				while ((c = getc(fp)) != EOF) RAM[i++] = (c);
				WrCPU(0x0245,i);						// BasicProgEnd = 0x0245;
				WrCPU(0x0246,(i >> 8));
				break;
		}
	}
	fclose(fp);

	RunCPU();

	// save MEMORY
	if (control_check(CONTROL_TRIGGER_LEFT).pressed)
	{
		fp = fopen("LVIV.MEM", "wb");
		fwrite(&RAM, sizeof(RAM), 1, fp);
		//fwrite(&ROM, sizeof(ROM), 1, fp);
		fclose(fp);
	}

	timer_delete(appTimer);
	gfx_term();
	display_delete(appDisplay);
	control_term();

	return ref;
}

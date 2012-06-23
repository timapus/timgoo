// LvivGOO
// Emulator of soviet computer LVIV PK-01
// Copyright (C) Alexander Timoshenko 2012

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dingoo/slcd.h>
#include <dingoo/cache.h>
#include <jz4740/cpu.h>
#include <sml/control.h>
#include <sml/timer.h>
#include <sml/graphics.h>
#include <sml/display.h>

#include "font.h"

#include "I8080.h"            /* I8080 CPU emulation             */
#include "I8255.h"
#include "ram.h"
#include "rom.h"

#define SCANLINE_MAX	256

I8080  			CPU;
I8255			PPI[2];
//uint8_t		RAM[0xC000];
uint8_t			VRAM[0x4000];
uint16_t		PALETTE[4] = { 0,0,0,0 };
uint8_t 		screen[256][256];
uint8_t 		KEY_BASE[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t	 		KEY_EXT[4] = {0xFF,0xFF,0xFF,0xFF};
bool			ROM_CS;

display*        appDisplay  = NULL;
gfx_texture*    appImage    = NULL;
char			tempFileName[256];
char			tempFileName2[256];
bool            error = false;
bool            appRunning = true;

gfx_font*		appFont     = NULL;


int ref = 1;
//extern int _sys_judge_event(void *);

uint16_t	ScanLine = 0;

// Calculates color value from palette port
uint16_t COL(uint8_t color)
{
	uint16_t Result = 0;

	uint8_t port = PPI[0].Rout[1];
	if ((port & 0x40) != 0) Result ^= COLOR_BLUE;
	if ((port & 0x20) != 0) Result ^= COLOR_GREEN;
	if ((port & 0x10) != 0) Result ^= COLOR_RED;
	switch (color)
	{
		case 0:
			if ((port & 0x08) == 0) Result ^= COLOR_RED;
			if ((port & 0x04) == 0) Result ^= COLOR_BLUE;
			break;
		case 1:
			Result ^= COLOR_BLUE;
			if ((port & 0x01) == 0) Result ^= COLOR_RED;
			break;
		case 2:
			Result ^= COLOR_GREEN;
			break;
		case 3:
			Result ^= COLOR_RED;
			if ((port & 0x02) == 0) Result ^= COLOR_GREEN;
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

unsigned char remap_y[256] = {
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 14,
 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29,
 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 44,
 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 59,
 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 74,
 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 89,
 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,104,
105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,119,
120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,134,
135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,149,
150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,164,
165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,179,
180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,194,
195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,209,
210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,224,
225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,239
};

uint8_t*  _display_lut_blend6_2 = NULL;

bool _display_lut_create_blend6_2() {
	if(_display_lut_blend6_2 != NULL)
		free(_display_lut_blend6_2);
	_display_lut_blend6_2 = (uint8_t*)malloc(65536);
	if(_display_lut_blend6_2 == NULL)
		return false;
	uintptr_t i, j, k, l, ik, jk;
	for(l = 0, k = 0, ik = 16; k < 16; k++, ik--) {
		for(j = 0; j < 64; j++) {
			jk = j * k;
			for(i = 0; i < 64; i++, l++) {
				_display_lut_blend6_2[l] = (((i * ik) + jk) >> 4);
			}
		}
	}
	return true;
}

void linebuffer(word y)
{
	if ((y & 0x0F) != 0x0F)
	{
		uint8_t* tempLutX = &_display_lut_blend6_2[0];
		uint8_t* tempLutY = &_display_lut_blend6_2[(y<<12) & 0x0000F000];

		uint16_t inColor0,inColor2,tempR0,tempR1,tempG0,tempG1,tempB0,tempB1;

		uint16_t x;
		uint16_t* tempPtr = (uint16_t*)(gfx_render_target->address) + (remap_y[y] * gfx_render_target->width) + 32;

		for (x = 0; x < 256; x++)
		{
			inColor0 = PALETTE[screen[x][y]];
			inColor2 = PALETTE[screen[x][y+1]];

			tempR0 = tempLutX[((inColor0 & 0xF800) >> 10)];
			tempG0 = tempLutX[((inColor0 & 0x07E0) >> 5)];
			tempB0 = tempLutX[((inColor0 & 0x001F) << 1)];

			tempR1 = tempLutX[((inColor2 & 0xF800) >> 10)];
			tempG1 = tempLutX[((inColor2 & 0x07E0) >> 5)];
			tempB1 = tempLutX[((inColor2 & 0x001F) << 1)];

			tempR0 = tempLutY[tempR0 | (tempR1 << 6)];
			tempG0 = tempLutY[tempG0 | (tempG1 << 6)];
			tempB0 = tempLutY[tempB0 | (tempB1 << 6)];

			tempPtr[x] = (((tempR0 << 10) & 0xF800) | (tempG0 << 5) | (tempB0 >> 1));
    	}
   	}
}

void ResetComp(void)
{
	ROM_CS = TRUE;

	Reset8255(&PPI[0]);
	Reset8255(&PPI[1]);

	CPU.IAutoReset = 1;
	CPU.IPeriod = 192;

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
//					uint8_t c;
//					for (c=4; c>0; c--)
//					{
//						//screen[x++][y] = ((V & 0x80) >> 6) | ((V & 0x08) >> 3);	// (1)
//						screen[x++][y] = ((((V & 0x80) >> 3) | (V & 0x08)) >> 3);	// (2)
//						V <<= 1;
//					}
					screen[x++][y] = ((((V & 0x80) >> 3) | (V & 0x08)) >> 3);		// (3)
					screen[x++][y] = ((((V & 0x40) >> 3) | (V & 0x04)) >> 2);
					screen[x++][y] = ((((V & 0x20) >> 3) | (V & 0x02)) >> 1);
					screen[x][y] = (((V & 0x10) >> 3) | (V & 0x01));

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
	if ((ROM_CS) && (A < 0x4000))
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
				//return RAM[A];		// hardware upgrade
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

	switch (Port)
	{
		case 0xC0:
		case 0xC2:
		case 0xC3:
		case 0xD1:
		case 0xD3:
			Write8255(&PPI[(Port & 0x10) >> 4],Port & 0x03,Value);
			break;
		// Port B (D30)
		case 0xC1:
			Write8255(&PPI[0],1,Value);
			for (KK = 0; KK < 4; KK++) PALETTE[KK] = COL(KK);
			break;
		// Port A (D1)
		case 0xD0:
			Write8255(&PPI[1],0,Value);
			KK = 0xFF;
			//if ((PPI[1].Rout[0] & 0x80) == 0) KK &= KEY_BASE[7];
			//if ((PPI[1].Rout[0] & 0x40) == 0) KK &= KEY_BASE[6];
			//if ((PPI[1].Rout[0] & 0x20) == 0) KK &= KEY_BASE[5];
			//if ((PPI[1].Rout[0] & 0x10) == 0) KK &= KEY_BASE[4];
			if ((PPI[1].Rout[0] & 0x08) == 0) KK &= KEY_BASE[3];
			//if ((PPI[1].Rout[0] & 0x04) == 0) KK &= KEY_BASE[2];
			if ((PPI[1].Rout[0] & 0x02) == 0) KK &= KEY_BASE[1];
			//if ((PPI[1].Rout[0] & 0x01) == 0) KK &= KEY_BASE[0];
			PPI[1].Rin[1] = KK;
			break;
		// Port C (D1)
		case 0xD2:
			Write8255(&PPI[1],2,Value);
			KK = 0xFF;
			if ((PPI[1].Rout[2] & 0x08) == 0) KK &= KEY_EXT[3];
			//if ((PPI[1].Rout[2] & 0x04) == 0) KK &= KEY_EXT[2];
			//if ((PPI[1].Rout[2] & 0x02) == 0) KK &= KEY_EXT[1];
			//if ((PPI[1].Rout[2] & 0x01) == 0) KK &= KEY_EXT[0];
			PPI[1].Rin[2] = KK;
			break;
	}
	ROM_CS = FALSE;
}

/** InCPU() **************************************************/
/** CPU emulation calls this function to read a byte from   **/
/** a given I/O port.                                       **/
/*************************************************************/
byte InCPU(word Port)
{
	switch (Port)
	{
		case 0xC0:
		case 0xC1:
		case 0xC2:
		case 0xC3:
		case 0xD0:
		case 0xD1:
		case 0xD2:
		case 0xD3:
			return Read8255(&PPI[(Port & 0x10) >> 4],Port & 0x03);
			break;
		default:
			return 0xFF;
			break;
	}
}

/** gameScreenshot() *****************************************/
/** User calls this function to get a screenshot            **/
/*************************************************************/
void gameScreenshot(void)
{
	char tempString[256];
	unsigned long int tempNumber = 0;
	FILE* tempFile;
	while(true)
	{
		sprintf(tempString, "%s%lu.tga", tempFileName, tempNumber);
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

	if (control_check(CONTROL_POWER).pressed)
	{
		while (control_still_pressed(CONTROL_POWER)) control_poll();
		ResetComp();
		// WARM RESET
		if (control_check(CONTROL_TRIGGER_RIGHT).pressed) KEY_BASE[5] &= 0x7F;
	}


	if (control_check(CONTROL_BUTTON_START).pressed)
	{
		// EXIT
		if (control_check(CONTROL_BUTTON_SELECT).pressed)
	  	{
			appRunning = FALSE;
			//while (control_still_pressed(CONTROL_BUTTON_SELECT)) control_poll();
	  	}
		// SCREENSHOT
		if (control_check(CONTROL_TRIGGER_LEFT).pressed)
		{
			gameScreenshot();
			while (control_still_pressed(CONTROL_TRIGGER_LEFT)) control_poll();
		}
		// RESET
		if (control_check(CONTROL_TRIGGER_RIGHT).pressed)
		{
			while (control_still_pressed(CONTROL_TRIGGER_RIGHT)) control_poll();
			ResetComp();
			// WARM RESET
			if (control_check(CONTROL_BUTTON_B).pressed) KEY_BASE[5] &= 0x7F;
		}
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
  	  	if (control_check(CONTROL_BUTTON_B).pressed)
  	  	{
  	  		if (KEY_BASE[5] & 0x80)
  	  		{
  	  			KEY_BASE[1] &= 0xF7;
  	  		}
  	  	}
  	  	else
  	  	{
  	  		KEY_BASE[1] |= 0x08;
  	  		KEY_BASE[5] |= 0x80;
  	  	}
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
//uint32_t _time = 0;
//uint32_t _ticks = 0;
//uint32_t _last = 0;

word LoopCPU()
{
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
//		uint32_t tempTime = OSTimeGet();
// 	  	uint32_t tempTicks = tempTime - _last;
// 	  	_last = tempTime;
// 	  	char tempString[256];
//	  	sprintf(tempString, "%04d", tempTicks);
//	  	gfx_font_print(32,0,appFont,tempString);

	  	display_flip(appDisplay);
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

//
int main(int argc, char** argv)
{
	FILE* fp;
	int i,c;

	appDisplay = display_create(320, 240, 320, (DISPLAY_FORMAT_RGB565|DISPLAY_BUFFER_STATIC), NULL, NULL);
	if(appDisplay == NULL)
	{
		return ref;
	}

	cpu_clock_set(400000000);

	control_init();
	control_lock(timer_resolution / 4);

	gfx_init(appDisplay);

	// clear screen (I)
	display_clear(appDisplay);
	// clear screen (II)
	//gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
	//display_flip(appDisplay);
	// load font
	appFont = gfx_font_load_from_buffer(font, fontSize, COLOR_WHITE);
	gfx_font_color_set(appFont, COLOR_BLACK);

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
	fp = fopen(tempFileName, "rb");
	if (fp != 0)
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
				OutCPU(0xC3,fgetc(fp) | 0x80);
				fseek(fp, 0x140E4, SEEK_SET);
				OutCPU(0xD3,fgetc(fp) | 0x80);
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
				OutCPU(0xD2,0x0F);
				OutCPU(0xD0,0xFF);
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
				CPU.AF.W = 0x0000;
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

	tempFileName[strlen(tempFileName) - 4] = '\0';

	_display_lut_create_blend6_2();

	RunCPU();

	// save MEMORY
//	if (control_check(CONTROL_TRIGGER_LEFT).pressed)
//	{
//		strcat(tempFileName, ".info");
//		fp = fopen(tempFileName, "wb");
		//fwrite(&RAM, sizeof(RAM), 1, fp);
		//fwrite(&ROM, sizeof(ROM), 1, fp);
//		fclose(fp);
//	}

	gfx_font_delete(appFont);
	gfx_term();
	display_delete(appDisplay);
	control_term();

	return ref;
}

/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                          I8255.c                        **/
/**                                                         **/
/** This file contains emulation for the i8255 parallel     **/
/** port interface (PPI) chip from Intel. See I8255.h for   **/
/** declarations.                                           **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 2001-2008                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/**                                                         **/
/** Update: Timoshenko Alexander 2012                       **/
/*************************************************************/
#include "I8255.h"

/** Reset8255 ************************************************/
/** Reset the chip. Set all data to 0x00. Set all ports to  **/
/** "input" mode.                                           **/
/*************************************************************/
void Reset8255(register I8255 *D)
{
  /* Initialize all registers and ports */
  D->R[0]=D->Rout[0]=D->Rin[0]=0x00;
  D->R[1]=D->Rout[1]=D->Rin[1]=0x00;
  D->R[2]=D->Rout[2]=D->Rin[2]=0x00;
  D->R[3]=0x88;
}

/** Write8255 ************************************************/
/** Write value V into i8255 register A. Returns 0 when A   **/
/** is out of range, 1 otherwise.                           **/
/*************************************************************/
byte Write8255(register I8255 *D,register byte A,register byte V)
{
	switch(A)
	{
		case 0:
			D->R[0]=V;
			D->Rout[0] = D->R[3]&0x10? 0x00:D->R[0];
			break;
		case 1:
			D->R[1]=V;
			D->Rout[1] = D->R[3]&0x02? 0x00:D->R[1];
			break;
		case 2:
			D->R[2]=V;
			D->Rout[2] = ((D->R[3]&0x01? 0x00:D->R[2])&0x0F) | ((D->R[3]&0x08? 0x00:D->R[2])&0xF0);
			break;
		case 3:
			/* Control register */
			if (V&0x80)
			{
				D->R[3]=V;
				/* 8255 resets latches when mode set */
				D->R[0] = D->R[1] = D->R[2] = 0;
				/* Set output ports */
				D->Rout[0] = D->R[3]&0x10? 0x00:D->R[0];
				D->Rout[1] = D->R[3]&0x02? 0x00:D->R[1];
				D->Rout[2] = ((D->R[3]&0x01? 0x00:D->R[2])&0x0F) | ((D->R[3]&0x08? 0x00:D->R[2])&0xF0);
			}
			else
			{
				A=1<<((V&0x0E)>>1);
				if(V&0x01) D->R[2]|=A; else D->R[2]&=~A;
				D->Rout[2] = ((D->R[3]&0x01? 0x00:D->R[2])&0x0F) | ((D->R[3]&0x08? 0x00:D->R[2])&0xF0);
			}
			break;
		default:
			/* Invalid register */
			return(0);
  }
  /* Done */
  return(1);
}

/** Read8255 *************************************************/
/** Read value from an i8255 register A. Returns 0 when A   **/
/** is out of range.                                        **/
/*************************************************************/
byte Read8255(register I8255 *D,register byte A)
{
  switch(A)
  {
    case 0: return(D->R[3]&0x10? D->Rin[0]:D->R[0]);
    case 1: return(D->R[3]&0x02? D->Rin[1]:D->R[1]);
    case 2: return
            (
              ((D->R[3]&0x01? D->Rin[2]:D->R[2])&0x0F)|
              ((D->R[3]&0x08? D->Rin[2]:D->R[2])&0xF0)
            );
    case 3: return(0x00);//return(D->R[3]);
  }

  /* Invalid address */
  return(0x00);
}

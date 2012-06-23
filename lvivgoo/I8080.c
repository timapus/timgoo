/** I8080: portable I8080 emulator ***************************/
/**                                                         **/
/**                           I8080.c                       **/
/**                                                         **/
/** This file contains implementation for I8080 CPU. Don't  **/
/** forget to provide RdCPU(), WrCPU(), InCPU(), OutCPU(),  **/
/** LoopCPU(), functions to accomodate the emulated         **/
/** machine's architecture.                                 **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2003                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/   
/**     changes to this file.                               **/
/**                                                         **/
/** Change to I8080: Timoshenko Alexander 2012              **/
/*************************************************************/

#include "I8080.h"
#include <stdio.h>

/** INLINE ***************************************************/
/** Different compilers inline C functions differently.     **/
/*************************************************************/
#ifdef __GNUC__
#define INLINE inline
#else
#define INLINE static
#endif

I8080 CPU; /* I8080 CPU state and regs */

#define S(Fl)        CPU.AF.B.l|=Fl
#define R(Fl)        CPU.AF.B.l&=~(Fl)

#define M_RLC(Rg)      \
		Rg = (Rg<<1) | (Rg>>7); \
		CPU.AF.B.l = (CPU.AF.B.l & ~C_FLAG) | (Rg & C_FLAG)

#define M_RRC(Rg)      \
		CPU.AF.B.l = (CPU.AF.B.l & ~C_FLAG) | (Rg & C_FLAG); \
		Rg = (Rg>>1) | (Rg<<7)

#define M_RL(Rg)       \
  if(Rg&0x80)          \
  {                    \
    Rg=(Rg<<1)|(CPU.AF.B.l & C_FLAG); \
    CPU.AF.B.l = (CPU.AF.B.l & ~C_FLAG) | C_FLAG; \
  }                    \
  else                 \
  {                    \
    Rg=(Rg<<1)|(CPU.AF.B.l & C_FLAG); \
    CPU.AF.B.l = (CPU.AF.B.l & ~C_FLAG); \
  }

#define M_RR(Rg)       \
  if(Rg&0x01)          \
  {                    \
    Rg=(Rg>>1)|(CPU.AF.B.l<<7);     \
    CPU.AF.B.l = (CPU.AF.B.l & ~C_FLAG) | C_FLAG; \
  }                    \
  else                 \
  {                    \
    Rg=(Rg>>1)|(CPU.AF.B.l<<7);     \
    CPU.AF.B.l = (CPU.AF.B.l & ~C_FLAG); \
  }

#define M_POP(Rg) CPU.Rg.B.l=RdCPU(CPU.SP.W++);CPU.Rg.B.h=RdCPU(CPU.SP.W++)
#define M_PUSH(Rg) WrCPU(--CPU.SP.W,CPU.Rg.B.h);WrCPU(--CPU.SP.W,CPU.Rg.B.l)
#define M_CALL         \
  J.B.l=RdCPU(CPU.PC.W++);J.B.h=RdCPU(CPU.PC.W++);         \
  WrCPU(--CPU.SP.W,CPU.PC.B.h);WrCPU(--CPU.SP.W,CPU.PC.B.l); \
  CPU.PC.W=J.W
#define M_JP J.B.l=RdCPU(CPU.PC.W++);J.B.h=RdCPU(CPU.PC.W);CPU.PC.W=J.W
#define M_JR CPU.PC.W+=(offset)RdCPU(CPU.PC.W)+1
#define M_RET CPU.PC.B.l=RdCPU(CPU.SP.W++);CPU.PC.B.h=RdCPU(CPU.SP.W++)
#define M_RST(Ad) WrCPU(--CPU.SP.W,CPU.PC.B.h);WrCPU(--CPU.SP.W,CPU.PC.B.l);CPU.PC.W=Ad
#define M_LDWORD(Rg) CPU.Rg.B.l=RdCPU(CPU.PC.W++);CPU.Rg.B.h=RdCPU(CPU.PC.W++)

#define M_ADD(Rg)      \
  J.W=CPU.AF.B.h+Rg;     \
  CPU.AF.B.l=PZSTable[J.B.l]|J.B.h&C_FLAG|((CPU.AF.B.h^J.W^Rg)&H_FLAG); \
  CPU.AF.B.h=J.B.l

#define M_ADC(Rg)      \
  J.W=CPU.AF.B.h+Rg+(CPU.AF.B.l&C_FLAG); \
  CPU.AF.B.l=PZSTable[J.B.l]|J.B.h&C_FLAG|((CPU.AF.B.h^J.W^Rg)&H_FLAG); \
  CPU.AF.B.h=J.B.l

#define M_SUB(Rg)      \
  J.W=CPU.AF.B.h-Rg;    \
  CPU.AF.B.l=PZSTable[J.B.l]|J.B.h&C_FLAG|((CPU.AF.B.h^J.W^Rg)&H_FLAG); \
  CPU.AF.B.l ^=H_FLAG; \
  CPU.AF.B.h=J.B.l

#define M_SBC(Rg)      \
  J.W=CPU.AF.B.h-Rg-(CPU.AF.B.l&C_FLAG); \
  CPU.AF.B.l=PZSTable[J.B.l]|J.B.h&C_FLAG|((CPU.AF.B.h^J.W^Rg)&H_FLAG); \
  CPU.AF.B.l ^=H_FLAG; \
  CPU.AF.B.h=J.B.l

#define M_CP(Rg)       \
  J.W=CPU.AF.B.h-Rg;    \
  CPU.AF.B.l=PZSTable[J.B.l]|J.B.h&C_FLAG|((CPU.AF.B.h^J.W^Rg)&H_FLAG); \
  CPU.AF.B.l ^=H_FLAG; \

#define M_AND(Rg) \
  J.W=CPU.AF.B.h&Rg;    \
  CPU.AF.B.l=PZSTable[J.B.l]|((CPU.AF.B.h|Rg)<<1)&H_FLAG; \
  CPU.AF.B.h=J.B.l

#define M_OR(Rg)  CPU.AF.B.h|=Rg; CPU.AF.B.l=PZSTable[CPU.AF.B.h]
#define M_XOR(Rg) CPU.AF.B.h^=Rg; CPU.AF.B.l=PZSTable[CPU.AF.B.h]

#define M_INC(Rg)       \
  Rg++;                 \
  CPU.AF.B.l=(CPU.AF.B.l&C_FLAG)|PZSTable[Rg]|(Rg&0x0F? 0:H_FLAG)

#define M_DEC(Rg)       \
  Rg--;                 \
  CPU.AF.B.l=(CPU.AF.B.l&C_FLAG)|PZSTable[Rg]|((Rg&0x0F)==0x0F? 0:H_FLAG)

#define M_ADDW(Rg1,Rg2) \
  J.W=(CPU.Rg1.W+CPU.Rg2.W)&0xFFFF;                        \
  CPU.AF.B.l=(CPU.AF.B.l&~C_FLAG)|(((long)CPU.Rg1.W+(long)CPU.Rg2.W)&0x10000? C_FLAG:0); \
  CPU.Rg1.W=J.W

enum Codes
{
  NOP,LD_BC_WORD,LD_xBC_A,INC_BC,INC_B,DEC_B,LD_B_BYTE,RLCA,
  NOP1,ADD_HL_BC,LD_A_xBC,DEC_BC,INC_C,DEC_C,LD_C_BYTE,RRCA,
  NOP2,LD_DE_WORD,LD_xDE_A,INC_DE,INC_D,DEC_D,LD_D_BYTE,RLA,
  NOP3,ADD_HL_DE,LD_A_xDE,DEC_DE,INC_E,DEC_E,LD_E_BYTE,RRA,
  NOP4,LD_HL_WORD,LD_xWORD_HL,INC_HL,INC_H,DEC_H,LD_H_BYTE,DAA,
  NOP5,ADD_HL_HL,LD_HL_xWORD,DEC_HL,INC_L,DEC_L,LD_L_BYTE,CPL,
  NOP6,LD_SP_WORD,LD_xWORD_A,INC_SP,INC_xHL,DEC_xHL,LD_xHL_BYTE,SCF,
  NOP7,ADD_HL_SP,LD_A_xWORD,DEC_SP,INC_A,DEC_A,LD_A_BYTE,CCF,
  LD_B_B,LD_B_C,LD_B_D,LD_B_E,LD_B_H,LD_B_L,LD_B_xHL,LD_B_A,
  LD_C_B,LD_C_C,LD_C_D,LD_C_E,LD_C_H,LD_C_L,LD_C_xHL,LD_C_A,
  LD_D_B,LD_D_C,LD_D_D,LD_D_E,LD_D_H,LD_D_L,LD_D_xHL,LD_D_A,
  LD_E_B,LD_E_C,LD_E_D,LD_E_E,LD_E_H,LD_E_L,LD_E_xHL,LD_E_A,
  LD_H_B,LD_H_C,LD_H_D,LD_H_E,LD_H_H,LD_H_L,LD_H_xHL,LD_H_A,
  LD_L_B,LD_L_C,LD_L_D,LD_L_E,LD_L_H,LD_L_L,LD_L_xHL,LD_L_A,
  LD_xHL_B,LD_xHL_C,LD_xHL_D,LD_xHL_E,LD_xHL_H,LD_xHL_L,HALT,LD_xHL_A,
  LD_A_B,LD_A_C,LD_A_D,LD_A_E,LD_A_H,LD_A_L,LD_A_xHL,LD_A_A,
  ADD_B,ADD_C,ADD_D,ADD_E,ADD_H,ADD_L,ADD_xHL,ADD_A,
  ADC_B,ADC_C,ADC_D,ADC_E,ADC_H,ADC_L,ADC_xHL,ADC_A,
  SUB_B,SUB_C,SUB_D,SUB_E,SUB_H,SUB_L,SUB_xHL,SUB_A,
  SBC_B,SBC_C,SBC_D,SBC_E,SBC_H,SBC_L,SBC_xHL,SBC_A,
  AND_B,AND_C,AND_D,AND_E,AND_H,AND_L,AND_xHL,AND_A,
  XOR_B,XOR_C,XOR_D,XOR_E,XOR_H,XOR_L,XOR_xHL,XOR_A,
  OR_B,OR_C,OR_D,OR_E,OR_H,OR_L,OR_xHL,OR_A,
  CP_B,CP_C,CP_D,CP_E,CP_H,CP_L,CP_xHL,CP_A,
  RET_NZ,POP_BC,JP_NZ,JP,CALL_NZ,PUSH_BC,ADD_BYTE,RST00,
  RET_Z,RET,JP_Z,JP_1,CALL_Z,CALL,ADC_BYTE,RST08,
  RET_NC,POP_DE,JP_NC,OUTA,CALL_NC,PUSH_DE,SUB_BYTE,RST10,
  RET_C,RET_1,JP_C,INA,CALL_C,CALL_1,SBC_BYTE,RST18,
  RET_PO,POP_HL,JP_PO,EX_HL_xSP,CALL_PO,PUSH_HL,AND_BYTE,RST20,
  RET_PE,LD_PC_HL,JP_PE,EX_DE_HL,CALL_PE,CALL_2,XOR_BYTE,RST28,
  RET_P,POP_AF,JP_P,DI,CALL_P,PUSH_AF,OR_BYTE,RST30,
  RET_M,LD_SP_HL,JP_M,EI,CALL_M,CALL_3,CP_BYTE,RST38
};

/** ResetCPU() ***********************************************/
/** This function can be used to reset the register struct  **/
/** before starting execution with CPU(). It sets the       **/
/** registers to their supposed initial values.             **/
/*************************************************************/
void ResetCPU()
{
  CPU.PC.W     = 0x0000;
  CPU.I        = 0x00;
  CPU.IFF      = 0x00;
  CPU.ICount   = CPU.IPeriod;
  CPU.IRequest = INT_NONE;
  CPU.STACK	   = 0;
}

/** ExecCPU() ************************************************/
/** This function will execute a single CPU opcode. It will **/
/** then return next PC, and current register values in R.  **/
/*************************************************************/
word ExecCPU()
{
  register byte I;
  register pair J;

  I=RdCPU(CPU.PC.W++);
  CPU.ICount-=Cycles[I];

  switch(I)
  {
#include "Codes.h"
  }

  /* We are done */
  return(CPU.PC.W);
}

/** IntCPU() *************************************************/
/** This function will generate interrupt of given vector.  **/
/*************************************************************/
void IntCPU(word Vector)
{
  if((CPU.IFF&IFF_1)||(Vector==INT_NMI))
  {
    /* If HALTed, take CPU off HALT instruction */
    if(CPU.IFF&IFF_HALT) { CPU.PC.W++;CPU.IFF&=~IFF_HALT; }

    /* Save PC on stack */
    M_PUSH(PC);

    /* Automatically reset IRequest if needed */
    if(CPU.IAutoReset&&(Vector==CPU.IRequest)) CPU.IRequest=INT_NONE;

    /* If it is NMI... */
    if(Vector==INT_NMI)
    {
      /* Copy IFF1 to IFF2 */
      if(CPU.IFF&IFF_1) CPU.IFF|=IFF_2; else CPU.IFF&=~IFF_2;
      /* Clear IFF1 */
      CPU.IFF&=~(IFF_1|IFF_EI);
      /* Jump to hardwired NMI vector */
      CPU.PC.W=0x0066;
      /* Done */
      return;
    }

    /* Further interrupts off */
    CPU.IFF&=~(IFF_1|IFF_2|IFF_EI);

    /* If in IM2 mode... */
    if(CPU.IFF&IFF_IM2)
    {
      /* Make up the vector address */
      Vector=(Vector&0xFF)|((word)(CPU.I)<<8);
      /* Read the vector */
      CPU.PC.B.l=RdCPU(Vector++);
      CPU.PC.B.h=RdCPU(Vector);
      /* Done */
      return;
    }

    /* If in IM1 mode, just jump to hardwired IRQ vector */
    if(CPU.IFF&IFF_IM1) { CPU.PC.W=0x0038;return; }

    /* If in IM0 mode... */

    /* Jump to a vector */
    switch(Vector)
    {
      case INT_RST00: CPU.PC.W=0x0000;break;
      case INT_RST08: CPU.PC.W=0x0008;break;
      case INT_RST10: CPU.PC.W=0x0010;break;
      case INT_RST18: CPU.PC.W=0x0018;break;
      case INT_RST20: CPU.PC.W=0x0020;break;
      case INT_RST28: CPU.PC.W=0x0028;break;
      case INT_RST30: CPU.PC.W=0x0030;break;
      case INT_RST38: CPU.PC.W=0x0038;break;
    }
  }
}

/** RunCPU() *************************************************/
/** This function will run CPU code until an LoopCPU() call **/
/** returns INT_QUIT. It will return the PC at which        **/
/** emulation stopped, and current register values in R.    **/
/*************************************************************/
word RunCPU()
{
  register byte I;
  register pair J;
  word k;

  for(;;)
  {
	//if ((CPU.PC.W == 0xD226) || (CPU.Trace))
//	if ((CPU.PC.W == 0xD387) || (CPU.Trace))
//	{
		//CPU.Trace = 1;
		//J.W=LoopCPU();
	//}

	I=RdCPU(CPU.PC.W++);
    CPU.ICount-=Cycles[I];

    switch(I)
    {
#include "Codes.h"
    }

    /* If cycle counter expired... */
    if(CPU.ICount<=0)
    {
      /* If we have come after EI, get address from IRequest */
      /* Otherwise, get it from the loop handler             */
      if(CPU.IFF&IFF_EI)
      {
        CPU.IFF=(CPU.IFF&~IFF_EI)|IFF_1; // Done with AfterEI state
        //CPU.ICount+=CPU.IBackup-1;       // Restore the ICount   //***1

        // Call periodic handler or set pending IRQ
        if(CPU.ICount>0) J.W=CPU.IRequest;
        else
        {
          J.W=LoopCPU();        // Call periodic handler
          CPU.ICount+=CPU.IPeriod; // Reset the cycle counter
          if(J.W==INT_NONE) J.W=CPU.IRequest;  // Pending IRQ
        }
      }
      else
      {
        J.W=LoopCPU();          // Call periodic handler
        CPU.ICount+=CPU.IPeriod;   // Reset the cycle counter
        if(J.W==INT_NONE) J.W=CPU.IRequest;    // Pending IRQ
      }

      if(J.W==INT_QUIT) return(CPU.PC.W); /* Exit if INT_QUIT */
      if(J.W!=INT_NONE) IntCPU(J.W);   /* Int-pt if needed */
    }
  }

  /* Execution stopped */
  return(CPU.PC.W);
}

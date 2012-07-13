/** I8080: portable I8080 emulator ***************************/
/**                                                         **/
/**                           I8080.h                       **/
/**                                                         **/
/** This file contains declarations relevant to emulation   **/
/** of I8080 CPU.                                           **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2003                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/   
/**     changes to this file.                               **/
/**                                                         **/
/** Change to I8080: Timoshenko Alexander 2012              **/
/*************************************************************/
#ifndef I8080_H
#define I8080_H

#include <stdint.h>

                               /* Compilation options:       */
/* #define DEBUG */            /* Compile debugging version  */
 #define LSB_FIRST         /* Compile for low-endian CPU */
/* #define MSB_FIRST */        /* Compile for hi-endian CPU  */

                               /* LoopCPU() may return:      */
#define INT_RST00   0x00C7     /* RST 00h                    */
#define INT_RST08   0x00CF     /* RST 08h                    */
#define INT_RST10   0x00D7     /* RST 10h                    */
#define INT_RST18   0x00DF     /* RST 18h                    */
#define INT_RST20   0x00E7     /* RST 20h                    */
#define INT_RST28   0x00EF     /* RST 28h                    */
#define INT_RST30   0x00F7     /* RST 30h                    */
#define INT_RST38   0x00FF     /* RST 38h                    */
#define INT_IRQ     INT_RST38  /* Default IRQ opcode is FFh  */
#define INT_NMI     0xFFFD     /* Non-maskable interrupt     */
#define INT_NONE    0xFFFF     /* No interrupt required      */
#define INT_QUIT    0xFFFE     /* Exit the emulation         */

                               /* Bits in Z80 F register:    */
#define S_FLAG      0x80       /* 1: Result negative         */
#define Z_FLAG      0x40       /* 1: Result is zero          */
#define H_FLAG      0x10       /* 1: Halfcarry/Halfborrow    */
#define P_FLAG      0x04       /* 1: Result is even          */
#define C_FLAG      0x01       /* 1: Carry/Borrow occured    */

                               /* Bits in IFF flip-flops:    */
#define IFF_1       0x01       /* IFF1 flip-flop             */
#define IFF_IM1     0x02       /* 1: IM1 mode                */
#define IFF_IM2     0x04       /* 1: IM2 mode                */
#define IFF_2       0x08       /* IFF2 flip-flop             */
#define IFF_EI      0x20       /* 1: EI pending              */
#define IFF_HALT    0x80       /* 1: CPU HALTed              */

/** Simple Datatypes *****************************************/
/** NOTICE: sizeof(byte)=1 and sizeof(word)=2               **/
/*************************************************************/
#ifndef BYTE_TYPE_DEFINED
#define BYTE_TYPE_DEFINED
typedef unsigned char byte;
#endif
#ifndef WORD_TYPE_DEFINED
#define WORD_TYPE_DEFINED
typedef unsigned short word;
#endif
typedef signed char offset;

/** Structured Datatypes *************************************/
/** NOTICE: #define LSB_FIRST for machines where least      **/
/**         signifcant byte goes first.                     **/
/*************************************************************/
typedef union
{
#ifdef LSB_FIRST
  struct { byte l,h; } B;
#else
  struct { byte h,l; } B;
#endif
  word W;
} pair;

typedef struct
{
  pair AF,BC,DE,HL,PC,SP;       /* Main registers      */
  byte IFF,I;                   /* Interrupt registers */

  word IPeriod,ICount; /* Set IPeriod to number of CPU cycles */
                      /* between calls to LoopCPU()          */
  int IBackup;        /* Private, don't touch                */
  word IRequest;      /* Set to address of pending IRQ       */
  byte IAutoReset;    /* Set to 1 to autom. reset IRequest   */
  byte TrapBadOps;    /* Set to 1 to warn of illegal opcodes */
  word Trap;          /* Set Trap to address to trace from   */
  byte Trace;         /* Set Trace=1 to start tracing        */
  void *User;         /* Arbitrary user data (ID,RAM*,etc.)  */
  byte STACK;
} I8080;

/** ResetCPU() ***********************************************/
/** This function can be used to reset the registers before **/
/** starting execution with RunCPU(). It sets registers to  **/
/** their initial values.                                   **/
/*************************************************************/
void ResetCPU(uint16_t ADDR);

/** ExecCPU() ************************************************/
/** This function will execute a single CPU opcode. It will **/
/** then return next PC, and current register values in R.  **/
/*************************************************************/
uint16_t ExecCPU();

/** IntCPU() *************************************************/
/** This function will generate interrupt of given vector.  **/
/*************************************************************/
void IntCPU(register uint16_t Vector);

/** RunCPU() *************************************************/
/** This function will run CPU code until an LoopCPU() call **/
/** returns INT_QUIT. It will return the PC at which        **/
/** emulation stopped, and current register values in R.    **/
/*************************************************************/
word RunCPU();

/** RdCPU()/WrCPU() ******************************************/
/** These functions are called when access to RAM occurs.   **/
/** They allow to control memory access.                    **/
/************************************ TO BE WRITTEN BY USER **/
void WrCPU(register uint16_t Addr, register uint8_t Value);
uint8_t RdCPU(register uint16_t Addr);

/** InCPU()/OutCPU() *****************************************/
/** CPU emulation calls these functions to read/write from  **/
/** I/O ports. There can be 65536 I/O ports, but only first **/
/** 256 are usually used.                                   **/
/************************************ TO BE WRITTEN BY USER **/
void OutCPU(register uint16_t Port, register uint8_t Value);
uint8_t InCPU(register uint16_t Port);

static const byte Cycles[256] =
{
   4,10, 7, 5, 5, 5, 7, 4, 4,10, 7, 5, 5, 5, 7, 4,
   4,10, 7, 5, 5, 5, 7, 4, 4,10, 7, 5, 5, 5, 7, 4,
   4,10,16, 5, 5, 5, 7, 4, 4,10,16, 5, 5, 5, 7, 4,
   4,10,13, 5,10,10,10, 4, 4,10,13, 5, 5, 5, 7, 4,
   5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
   5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
   5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
   7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
   4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
   4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
   4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
   4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
   5,10,10,10,10,11, 7,11, 5,10,10, 0,10,17, 7,11,
   5,10,10,10,10,11, 7,11, 5, 4,10,10,10, 0, 7,11,
   5,10,10,18,10,11, 7,11, 5, 5,10, 4,10, 0, 7,11,
   5,10,10, 4,10,11, 7,11, 5, 5,10, 4,10, 0, 7,11
};

static const byte PZSTable[256] =
{
  Z_FLAG|P_FLAG|2,2,2,P_FLAG|2,2,P_FLAG|2,P_FLAG|2,2,2,P_FLAG|2,P_FLAG|2,2,P_FLAG|2,2,2,P_FLAG|2,
  2,P_FLAG|2,P_FLAG|2,2,P_FLAG|2,2,2,P_FLAG|2,P_FLAG|2,2,2,P_FLAG|2,2,P_FLAG|2,P_FLAG|2,2,
  2,P_FLAG|2,P_FLAG|2,2,P_FLAG|2,2,2,P_FLAG|2,P_FLAG|2,2,2,P_FLAG|2,2,P_FLAG|2,P_FLAG|2,2,
  P_FLAG|2,2,2,P_FLAG|2,2,P_FLAG|2,P_FLAG|2,2,2,P_FLAG|2,P_FLAG|2,2,P_FLAG|2,2,2,P_FLAG|2,
  2,P_FLAG|2,P_FLAG|2,2,P_FLAG|2,2,2,P_FLAG|2,P_FLAG|2,2,2,P_FLAG|2,2,P_FLAG|2,P_FLAG|2,2,
  P_FLAG|2,2,2,P_FLAG|2,2,P_FLAG|2,P_FLAG|2,2,2,P_FLAG|2,P_FLAG|2,2,P_FLAG|2,2,2,P_FLAG|2,
  P_FLAG|2,2,2,P_FLAG|2,2,P_FLAG|2,P_FLAG|2,2,2,P_FLAG|2,P_FLAG|2,2,P_FLAG|2,2,2,P_FLAG|2,
  2,P_FLAG|2,P_FLAG|2,2,P_FLAG|2,2,2,P_FLAG|2,P_FLAG|2,2,2,P_FLAG|2,2,P_FLAG|2,P_FLAG|2,2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|P_FLAG|2,S_FLAG|2,
  S_FLAG|P_FLAG|2,S_FLAG|2,S_FLAG|2,S_FLAG|P_FLAG|2
};

static const word DAATable[1024] =
{
  0x0046,0x0102,0x0202,0x0306,0x0402,0x0506,0x0606,0x0702,
  0x0802,0x0906,0x1012,0x1116,0x1216,0x1312,0x1416,0x1512,
  0x1002,0x1106,0x1206,0x1302,0x1406,0x1502,0x1602,0x1706,
  0x1806,0x1902,0x2012,0x2116,0x2216,0x2312,0x2416,0x2512,
  0x2002,0x2106,0x2206,0x2302,0x2406,0x2502,0x2602,0x2706,
  0x2806,0x2902,0x3016,0x3112,0x3212,0x3316,0x3412,0x3516,
  0x3006,0x3102,0x3202,0x3306,0x3402,0x3506,0x3606,0x3702,
  0x3802,0x3906,0x4012,0x4116,0x4216,0x4312,0x4416,0x4512,
  0x4002,0x4106,0x4206,0x4302,0x4406,0x4502,0x4602,0x4706,
  0x4806,0x4902,0x5016,0x5112,0x5212,0x5316,0x5412,0x5516,
  0x5006,0x5102,0x5202,0x5306,0x5402,0x5506,0x5606,0x5702,
  0x5802,0x5906,0x6016,0x6112,0x6212,0x6316,0x6412,0x6516,
  0x6006,0x6102,0x6202,0x6306,0x6402,0x6506,0x6606,0x6702,
  0x6802,0x6906,0x7012,0x7116,0x7216,0x7312,0x7416,0x7512,
  0x7002,0x7106,0x7206,0x7302,0x7406,0x7502,0x7602,0x7706,
  0x7806,0x7902,0x8092,0x8196,0x8296,0x8392,0x8496,0x8592,
  0x8082,0x8186,0x8286,0x8382,0x8486,0x8582,0x8682,0x8786,
  0x8886,0x8982,0x9096,0x9192,0x9292,0x9396,0x9492,0x9596,
  0x9086,0x9182,0x9282,0x9386,0x9482,0x9586,0x9686,0x9782,
  0x9882,0x9986,0x0057,0x0113,0x0213,0x0317,0x0413,0x0517,
  0x0047,0x0103,0x0203,0x0307,0x0403,0x0507,0x0607,0x0703,
  0x0803,0x0907,0x1013,0x1117,0x1217,0x1313,0x1417,0x1513,
  0x1003,0x1107,0x1207,0x1303,0x1407,0x1503,0x1603,0x1707,
  0x1807,0x1903,0x2013,0x2117,0x2217,0x2313,0x2417,0x2513,
  0x2003,0x2107,0x2207,0x2303,0x2407,0x2503,0x2603,0x2707,
  0x2807,0x2903,0x3017,0x3113,0x3213,0x3317,0x3413,0x3517,
  0x3007,0x3103,0x3203,0x3307,0x3403,0x3507,0x3607,0x3703,
  0x3803,0x3907,0x4013,0x4117,0x4217,0x4313,0x4417,0x4513,
  0x4003,0x4107,0x4207,0x4303,0x4407,0x4503,0x4603,0x4707,
  0x4807,0x4903,0x5017,0x5113,0x5213,0x5317,0x5413,0x5517,
  0x5007,0x5103,0x5203,0x5307,0x5403,0x5507,0x5607,0x5703,
  0x5803,0x5907,0x6017,0x6113,0x6213,0x6317,0x6413,0x6517,
  0x6007,0x6103,0x6203,0x6307,0x6403,0x6507,0x6607,0x6703,
  0x6803,0x6907,0x7013,0x7117,0x7217,0x7313,0x7417,0x7513,
  0x7003,0x7107,0x7207,0x7303,0x7407,0x7503,0x7603,0x7707,
  0x7807,0x7903,0x8093,0x8197,0x8297,0x8393,0x8497,0x8593,
  0x8083,0x8187,0x8287,0x8383,0x8487,0x8583,0x8683,0x8787,
  0x8887,0x8983,0x9097,0x9193,0x9293,0x9397,0x9493,0x9597,
  0x9087,0x9183,0x9283,0x9387,0x9483,0x9587,0x9687,0x9783,
  0x9883,0x9987,0xA097,0xA193,0xA293,0xA397,0xA493,0xA597,
  0xA087,0xA183,0xA283,0xA387,0xA483,0xA587,0xA687,0xA783,
  0xA883,0xA987,0xB093,0xB197,0xB297,0xB393,0xB497,0xB593,
  0xB083,0xB187,0xB287,0xB383,0xB487,0xB583,0xB683,0xB787,
  0xB887,0xB983,0xC097,0xC193,0xC293,0xC397,0xC493,0xC597,
  0xC087,0xC183,0xC283,0xC387,0xC483,0xC587,0xC687,0xC783,
  0xC883,0xC987,0xD093,0xD197,0xD297,0xD393,0xD497,0xD593,
  0xD083,0xD187,0xD287,0xD383,0xD487,0xD583,0xD683,0xD787,
  0xD887,0xD983,0xE093,0xE197,0xE297,0xE393,0xE497,0xE593,
  0xE083,0xE187,0xE287,0xE383,0xE487,0xE583,0xE683,0xE787,
  0xE887,0xE983,0xF097,0xF193,0xF293,0xF397,0xF493,0xF597,
  0xF087,0xF183,0xF283,0xF387,0xF483,0xF587,0xF687,0xF783,
  0xF883,0xF987,0x0057,0x0113,0x0213,0x0317,0x0413,0x0517,
  0x0047,0x0103,0x0203,0x0307,0x0403,0x0507,0x0607,0x0703,
  0x0803,0x0907,0x1013,0x1117,0x1217,0x1313,0x1417,0x1513,
  0x1003,0x1107,0x1207,0x1303,0x1407,0x1503,0x1603,0x1707,
  0x1807,0x1903,0x2013,0x2117,0x2217,0x2313,0x2417,0x2513,
  0x2003,0x2107,0x2207,0x2303,0x2407,0x2503,0x2603,0x2707,
  0x2807,0x2903,0x3017,0x3113,0x3213,0x3317,0x3413,0x3517,
  0x3007,0x3103,0x3203,0x3307,0x3403,0x3507,0x3607,0x3703,
  0x3803,0x3907,0x4013,0x4117,0x4217,0x4313,0x4417,0x4513,
  0x4003,0x4107,0x4207,0x4303,0x4407,0x4503,0x4603,0x4707,
  0x4807,0x4903,0x5017,0x5113,0x5213,0x5317,0x5413,0x5517,
  0x5007,0x5103,0x5203,0x5307,0x5403,0x5507,0x5607,0x5703,
  0x5803,0x5907,0x6017,0x6113,0x6213,0x6317,0x6413,0x6517,
  0x0606,0x0702,0x0802,0x0906,0x0A06,0x0B02,0x0C06,0x0D02,
  0x0E02,0x0F06,0x1012,0x1116,0x1216,0x1312,0x1416,0x1512,
  0x1602,0x1706,0x1806,0x1902,0x1A02,0x1B06,0x1C02,0x1D06,
  0x1E06,0x1F02,0x2012,0x2116,0x2216,0x2312,0x2416,0x2512,
  0x2602,0x2706,0x2806,0x2902,0x2A02,0x2B06,0x2C02,0x2D06,
  0x2E06,0x2F02,0x3016,0x3112,0x3212,0x3316,0x3412,0x3516,
  0x3606,0x3702,0x3802,0x3906,0x3A06,0x3B02,0x3C06,0x3D02,
  0x3E02,0x3F06,0x4012,0x4116,0x4216,0x4312,0x4416,0x4512,
  0x4602,0x4706,0x4806,0x4902,0x4A02,0x4B06,0x4C02,0x4D06,
  0x4E06,0x4F02,0x5016,0x5112,0x5212,0x5316,0x5412,0x5516,
  0x5606,0x5702,0x5802,0x5906,0x5A06,0x5B02,0x5C06,0x5D02,
  0x5E02,0x5F06,0x6016,0x6112,0x6212,0x6316,0x6412,0x6516,
  0x6606,0x6702,0x6802,0x6906,0x6A06,0x6B02,0x6C06,0x6D02,
  0x6E02,0x6F06,0x7012,0x7116,0x7216,0x7312,0x7416,0x7512,
  0x7602,0x7706,0x7806,0x7902,0x7A02,0x7B06,0x7C02,0x7D06,
  0x7E06,0x7F02,0x8092,0x8196,0x8296,0x8392,0x8496,0x8592,
  0x8682,0x8786,0x8886,0x8982,0x8A82,0x8B86,0x8C82,0x8D86,
  0x8E86,0x8F82,0x9096,0x9192,0x9292,0x9396,0x9492,0x9596,
  0x9686,0x9782,0x9882,0x9986,0x9A86,0x9B82,0x9C86,0x9D82,
  0x9E82,0x9F86,0x0057,0x0113,0x0213,0x0317,0x0413,0x0517,
  0x0607,0x0703,0x0803,0x0907,0x0A07,0x0B03,0x0C07,0x0D03,
  0x0E03,0x0F07,0x1013,0x1117,0x1217,0x1313,0x1417,0x1513,
  0x1603,0x1707,0x1807,0x1903,0x1A03,0x1B07,0x1C03,0x1D07,
  0x1E07,0x1F03,0x2013,0x2117,0x2217,0x2313,0x2417,0x2513,
  0x2603,0x2707,0x2807,0x2903,0x2A03,0x2B07,0x2C03,0x2D07,
  0x2E07,0x2F03,0x3017,0x3113,0x3213,0x3317,0x3413,0x3517,
  0x3607,0x3703,0x3803,0x3907,0x3A07,0x3B03,0x3C07,0x3D03,
  0x3E03,0x3F07,0x4013,0x4117,0x4217,0x4313,0x4417,0x4513,
  0x4603,0x4707,0x4807,0x4903,0x4A03,0x4B07,0x4C03,0x4D07,
  0x4E07,0x4F03,0x5017,0x5113,0x5213,0x5317,0x5413,0x5517,
  0x5607,0x5703,0x5803,0x5907,0x5A07,0x5B03,0x5C07,0x5D03,
  0x5E03,0x5F07,0x6017,0x6113,0x6213,0x6317,0x6413,0x6517,
  0x6607,0x6703,0x6803,0x6907,0x6A07,0x6B03,0x6C07,0x6D03,
  0x6E03,0x6F07,0x7013,0x7117,0x7217,0x7313,0x7417,0x7513,
  0x7603,0x7707,0x7807,0x7903,0x7A03,0x7B07,0x7C03,0x7D07,
  0x7E07,0x7F03,0x8093,0x8197,0x8297,0x8393,0x8497,0x8593,
  0x8683,0x8787,0x8887,0x8983,0x8A83,0x8B87,0x8C83,0x8D87,
  0x8E87,0x8F83,0x9097,0x9193,0x9293,0x9397,0x9493,0x9597,
  0x9687,0x9783,0x9883,0x9987,0x9A87,0x9B83,0x9C87,0x9D83,
  0x9E83,0x9F87,0xA097,0xA193,0xA293,0xA397,0xA493,0xA597,
  0xA687,0xA783,0xA883,0xA987,0xAA87,0xAB83,0xAC87,0xAD83,
  0xAE83,0xAF87,0xB093,0xB197,0xB297,0xB393,0xB497,0xB593,
  0xB683,0xB787,0xB887,0xB983,0xBA83,0xBB87,0xBC83,0xBD87,
  0xBE87,0xBF83,0xC097,0xC193,0xC293,0xC397,0xC493,0xC597,
  0xC687,0xC783,0xC883,0xC987,0xCA87,0xCB83,0xCC87,0xCD83,
  0xCE83,0xCF87,0xD093,0xD197,0xD297,0xD393,0xD497,0xD593,
  0xD683,0xD787,0xD887,0xD983,0xDA83,0xDB87,0xDC83,0xDD87,
  0xDE87,0xDF83,0xE093,0xE197,0xE297,0xE393,0xE497,0xE593,
  0xE683,0xE787,0xE887,0xE983,0xEA83,0xEB87,0xEC83,0xED87,
  0xEE87,0xEF83,0xF097,0xF193,0xF293,0xF397,0xF493,0xF597,
  0xF687,0xF783,0xF883,0xF987,0xFA87,0xFB83,0xFC87,0xFD83,
  0xFE83,0xFF87,0x0057,0x0113,0x0213,0x0317,0x0413,0x0517,
  0x0607,0x0703,0x0803,0x0907,0x0A07,0x0B03,0x0C07,0x0D03,
  0x0E03,0x0F07,0x1013,0x1117,0x1217,0x1313,0x1417,0x1513,
  0x1603,0x1707,0x1807,0x1903,0x1A03,0x1B07,0x1C03,0x1D07,
  0x1E07,0x1F03,0x2013,0x2117,0x2217,0x2313,0x2417,0x2513,
  0x2603,0x2707,0x2807,0x2903,0x2A03,0x2B07,0x2C03,0x2D07,
  0x2E07,0x2F03,0x3017,0x3113,0x3213,0x3317,0x3413,0x3517,
  0x3607,0x3703,0x3803,0x3907,0x3A07,0x3B03,0x3C07,0x3D03,
  0x3E03,0x3F07,0x4013,0x4117,0x4217,0x4313,0x4417,0x4513,
  0x4603,0x4707,0x4807,0x4903,0x4A03,0x4B07,0x4C03,0x4D07,
  0x4E07,0x4F03,0x5017,0x5113,0x5213,0x5317,0x5413,0x5517,
  0x5607,0x5703,0x5803,0x5907,0x5A07,0x5B03,0x5C07,0x5D03,
  0x5E03,0x5F07,0x6017,0x6113,0x6213,0x6317,0x6413,0x6517
};

#endif /* I8080_H */
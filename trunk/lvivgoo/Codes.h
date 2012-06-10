/** i8080: portable i8080 emulator ****************************/
/**                                                         **/
/**                          Codes.h                        **/
/**                                                         **/
/** This file contains implementation for the main table of **/
/** Z80 commands. It is included from Z80.c.                **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2003                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

case JP_NZ:   if(CPU.AF.B.l&Z_FLAG) CPU.PC.W+=2; else { M_JP; } break;
case JP_NC:   if(CPU.AF.B.l&C_FLAG) CPU.PC.W+=2; else { M_JP; } break;
case JP_PO:   if(CPU.AF.B.l&P_FLAG) CPU.PC.W+=2; else { M_JP; } break;
case JP_P:    if(CPU.AF.B.l&S_FLAG) CPU.PC.W+=2; else { M_JP; } break;
case JP_Z:    if(CPU.AF.B.l&Z_FLAG) { M_JP; } else CPU.PC.W+=2; break;
case JP_C:    if(CPU.AF.B.l&C_FLAG) { M_JP; } else CPU.PC.W+=2; break;
case JP_PE:   if(CPU.AF.B.l&P_FLAG) { M_JP; } else CPU.PC.W+=2; break;
case JP_M:    if(CPU.AF.B.l&S_FLAG) { M_JP; } else CPU.PC.W+=2; break;

case RET_NZ:  if(!(CPU.AF.B.l&Z_FLAG)) { CPU.ICount-=6; CPU.STACK=1; M_RET; CPU.STACK=0;} break;
case RET_NC:  if(!(CPU.AF.B.l&C_FLAG)) { CPU.ICount-=6; CPU.STACK=1; M_RET; CPU.STACK=0;} break;
case RET_PO:  if(!(CPU.AF.B.l&P_FLAG)) { CPU.ICount-=6; CPU.STACK=1; M_RET; CPU.STACK=0;} break;
case RET_P:   if(!(CPU.AF.B.l&S_FLAG)) { CPU.ICount-=6; CPU.STACK=1; M_RET; CPU.STACK=0;} break;
case RET_Z:   if(CPU.AF.B.l&Z_FLAG)    { CPU.ICount-=6; CPU.STACK=1; M_RET; CPU.STACK=0;} break;
case RET_C:   if(CPU.AF.B.l&C_FLAG)    { CPU.ICount-=6; CPU.STACK=1; M_RET; CPU.STACK=0;} break;
case RET_PE:  if(CPU.AF.B.l&P_FLAG)    { CPU.ICount-=6; CPU.STACK=1; M_RET; CPU.STACK=0;} break;
case RET_M:   if(CPU.AF.B.l&S_FLAG)    { CPU.ICount-=6; CPU.STACK=1; M_RET; CPU.STACK=0;} break;

case CALL_NZ: if(CPU.AF.B.l&Z_FLAG) CPU.PC.W+=2; else { CPU.ICount-=7; CPU.STACK=1; M_CALL; CPU.STACK=0;} break;
case CALL_NC: if(CPU.AF.B.l&C_FLAG) CPU.PC.W+=2; else { CPU.ICount-=7; CPU.STACK=1; M_CALL; CPU.STACK=0;} break;
case CALL_PO: if(CPU.AF.B.l&P_FLAG) CPU.PC.W+=2; else { CPU.ICount-=7; CPU.STACK=1; M_CALL; CPU.STACK=0;} break;
case CALL_P:  if(CPU.AF.B.l&S_FLAG) CPU.PC.W+=2; else { CPU.ICount-=7; CPU.STACK=1; M_CALL; CPU.STACK=0;} break;
case CALL_Z:  if(CPU.AF.B.l&Z_FLAG) { CPU.ICount-=7; CPU.STACK=1; M_CALL; CPU.STACK=0;} else CPU.PC.W+=2; break;
case CALL_C:  if(CPU.AF.B.l&C_FLAG) { CPU.ICount-=7; CPU.STACK=1; M_CALL; CPU.STACK=0;} else CPU.PC.W+=2; break;
case CALL_PE: if(CPU.AF.B.l&P_FLAG) { CPU.ICount-=7; CPU.STACK=1; M_CALL; CPU.STACK=0;} else CPU.PC.W+=2; break;
case CALL_M:  if(CPU.AF.B.l&S_FLAG) { CPU.ICount-=7; CPU.STACK=1; M_CALL; CPU.STACK=0;} else CPU.PC.W+=2; break;

case ADD_B:    M_ADD(CPU.BC.B.h);break;
case ADD_C:    M_ADD(CPU.BC.B.l);break;
case ADD_D:    M_ADD(CPU.DE.B.h);break;
case ADD_E:    M_ADD(CPU.DE.B.l);break;
case ADD_H:    M_ADD(CPU.HL.B.h);break;
case ADD_L:    M_ADD(CPU.HL.B.l);break;
//case ADD_A:    M_ADD(CPU.AF.B.h);break;
case ADD_A:    I=CPU.AF.B.h;M_ADD(I);break;
case ADD_xHL:  I=RdCPU(CPU.HL.W);M_ADD(I);break;
case ADD_BYTE: I=RdCPU(CPU.PC.W++);M_ADD(I);break;

case SUB_B:    M_SUB(CPU.BC.B.h);break;
case SUB_C:    M_SUB(CPU.BC.B.l);break;
case SUB_D:    M_SUB(CPU.DE.B.h);break;
case SUB_E:    M_SUB(CPU.DE.B.l);break;
case SUB_H:    M_SUB(CPU.HL.B.h);break;
case SUB_L:    M_SUB(CPU.HL.B.l);break;
case SUB_A:    I=CPU.AF.B.h;M_SUB(I);break;
case SUB_xHL:  I=RdCPU(CPU.HL.W);M_SUB(I);break;
case SUB_BYTE: I=RdCPU(CPU.PC.W++);M_SUB(I);break;

case AND_B:    M_AND(CPU.BC.B.h);break;
case AND_C:    M_AND(CPU.BC.B.l);break;
case AND_D:    M_AND(CPU.DE.B.h);break;
case AND_E:    M_AND(CPU.DE.B.l);break;
case AND_H:    M_AND(CPU.HL.B.h);break;
case AND_L:    M_AND(CPU.HL.B.l);break;
case AND_A:    I=CPU.AF.B.h;M_AND(I);break;
case AND_xHL:  I=RdCPU(CPU.HL.W);M_AND(I);break;
case AND_BYTE: I=RdCPU(CPU.PC.W++);M_AND(I);break;

case OR_B:     M_OR(CPU.BC.B.h);break;
case OR_C:     M_OR(CPU.BC.B.l);break;
case OR_D:     M_OR(CPU.DE.B.h);break;
case OR_E:     M_OR(CPU.DE.B.l);break;
case OR_H:     M_OR(CPU.HL.B.h);break;
case OR_L:     M_OR(CPU.HL.B.l);break;
case OR_A:     I=CPU.AF.B.h;M_OR(I);break;
case OR_xHL:   I=RdCPU(CPU.HL.W);M_OR(I);break;
case OR_BYTE:  I=RdCPU(CPU.PC.W++);M_OR(I);break;

case ADC_B:    M_ADC(CPU.BC.B.h);break;
case ADC_C:    M_ADC(CPU.BC.B.l);break;
case ADC_D:    M_ADC(CPU.DE.B.h);break;
case ADC_E:    M_ADC(CPU.DE.B.l);break;
case ADC_H:    M_ADC(CPU.HL.B.h);break;
case ADC_L:    M_ADC(CPU.HL.B.l);break;
case ADC_A:    I=CPU.AF.B.h;M_ADC(I);break;
case ADC_xHL:  I=RdCPU(CPU.HL.W);M_ADC(I);break;
case ADC_BYTE: I=RdCPU(CPU.PC.W++);M_ADC(I);break;

case SBC_B:    M_SBC(CPU.BC.B.h);break;
case SBC_C:    M_SBC(CPU.BC.B.l);break;
case SBC_D:    M_SBC(CPU.DE.B.h);break;
case SBC_E:    M_SBC(CPU.DE.B.l);break;
case SBC_H:    M_SBC(CPU.HL.B.h);break;
case SBC_L:    M_SBC(CPU.HL.B.l);break;
case SBC_A:    I=CPU.AF.B.h;M_SBC(I);break;
case SBC_xHL:  I=RdCPU(CPU.HL.W);M_SBC(I);break;
case SBC_BYTE: I=RdCPU(CPU.PC.W++);M_SBC(I);break;

case XOR_B:    M_XOR(CPU.BC.B.h);break;
case XOR_C:    M_XOR(CPU.BC.B.l);break;
case XOR_D:    M_XOR(CPU.DE.B.h);break;
case XOR_E:    M_XOR(CPU.DE.B.l);break;
case XOR_H:    M_XOR(CPU.HL.B.h);break;
case XOR_L:    M_XOR(CPU.HL.B.l);break;
case XOR_A:    I=CPU.AF.B.h;M_XOR(I);break;
case XOR_xHL:  I=RdCPU(CPU.HL.W);M_XOR(I);break;
case XOR_BYTE: I=RdCPU(CPU.PC.W++);M_XOR(I);break;

case CP_B:     M_CP(CPU.BC.B.h);break;
case CP_C:     M_CP(CPU.BC.B.l);break;
case CP_D:     M_CP(CPU.DE.B.h);break;
case CP_E:     M_CP(CPU.DE.B.l);break;
case CP_H:     M_CP(CPU.HL.B.h);break;
case CP_L:     M_CP(CPU.HL.B.l);break;
case CP_A:     I=CPU.AF.B.h;M_CP(I);break;
case CP_xHL:   I=RdCPU(CPU.HL.W);M_CP(I);break;
case CP_BYTE:  I=RdCPU(CPU.PC.W++);M_CP(I);break;
               
case LD_BC_WORD: M_LDWORD(BC);break;
case LD_DE_WORD: M_LDWORD(DE);break;
case LD_HL_WORD: M_LDWORD(HL);break;
case LD_SP_WORD: M_LDWORD(SP);break;

case LD_PC_HL: CPU.PC.W=CPU.HL.W;break;
case LD_SP_HL: CPU.SP.W=CPU.HL.W;break;
case LD_A_xBC: CPU.AF.B.h=RdCPU(CPU.BC.W);break;
case LD_A_xDE: CPU.AF.B.h=RdCPU(CPU.DE.W);break;

case ADD_HL_BC:  M_ADDW(HL,BC);break;
case ADD_HL_DE:  M_ADDW(HL,DE);break;
case ADD_HL_HL:  M_ADDW(HL,HL);break;
case ADD_HL_SP:  M_ADDW(HL,SP);break;

case DEC_BC:   CPU.BC.W--;break;
case DEC_DE:   CPU.DE.W--;break;
case DEC_HL:   CPU.HL.W--;break;
case DEC_SP:   CPU.SP.W--;break;

case INC_BC:   CPU.BC.W++;break;
case INC_DE:   CPU.DE.W++;break;
case INC_HL:   CPU.HL.W++;break;
case INC_SP:   CPU.SP.W++;break;

case DEC_B:    M_DEC(CPU.BC.B.h);break;
case DEC_C:    M_DEC(CPU.BC.B.l);break;
case DEC_D:    M_DEC(CPU.DE.B.h);break;
case DEC_E:    M_DEC(CPU.DE.B.l);break;
case DEC_H:    M_DEC(CPU.HL.B.h);break;
case DEC_L:    M_DEC(CPU.HL.B.l);break;
case DEC_A:    M_DEC(CPU.AF.B.h);break;
case DEC_xHL:  I=RdCPU(CPU.HL.W);M_DEC(I);WrCPU(CPU.HL.W,I);break;

case INC_B:    M_INC(CPU.BC.B.h); break;
case INC_C:    M_INC(CPU.BC.B.l); break;
case INC_D:    M_INC(CPU.DE.B.h); break;
case INC_E:    M_INC(CPU.DE.B.l); break;
case INC_H:    M_INC(CPU.HL.B.h); break;
case INC_L:    M_INC(CPU.HL.B.l); break;
case INC_A:    M_INC(CPU.AF.B.h); break;
case INC_xHL:  I=RdCPU(CPU.HL.W);M_INC(I);WrCPU(CPU.HL.W,I); break;

case RLCA:     M_RLC(CPU.AF.B.h); break;
case RLA:      M_RL(CPU.AF.B.h); break;
case RRCA:     M_RRC(CPU.AF.B.h); break;
case RRA:      M_RR(CPU.AF.B.h); break;

case RST00:     CPU.STACK=1; M_RST(0x0000); CPU.STACK=0; break;
case RST08:     CPU.STACK=1; M_RST(0x0008); CPU.STACK=0; break;
case RST10:     CPU.STACK=1; M_RST(0x0010); CPU.STACK=0; break;
case RST18:     CPU.STACK=1; M_RST(0x0018); CPU.STACK=0; break;
case RST20:     CPU.STACK=1; M_RST(0x0020); CPU.STACK=0; break;
case RST28:     CPU.STACK=1; M_RST(0x0028); CPU.STACK=0; break;
case RST30:     CPU.STACK=1; M_RST(0x0030); CPU.STACK=0; break;
case RST38:     CPU.STACK=1; M_RST(0x0038); CPU.STACK=0; break;

case PUSH_BC:   CPU.STACK=1; M_PUSH(BC); CPU.STACK=0; break;
case PUSH_DE:   CPU.STACK=1; M_PUSH(DE); CPU.STACK=0; break;
case PUSH_HL:   CPU.STACK=1; M_PUSH(HL); CPU.STACK=0; break;
case PUSH_AF:   CPU.STACK=1; M_PUSH(AF); CPU.STACK=0; break;

case POP_BC:    CPU.STACK=1; M_POP(BC); CPU.STACK=0; break;
case POP_DE:    CPU.STACK=1; M_POP(DE); CPU.STACK=0; break;
case POP_HL:    CPU.STACK=1; M_POP(HL); CPU.STACK=0; break;
case POP_AF:
	CPU.STACK=1;
	M_POP(AF);
	CPU.AF.B.l = ((CPU.AF.B.l & 0xD7) | 0x02);
	CPU.STACK=0;
	break;

case JP:   M_JP;break;
case CALL:  CPU.STACK=1; M_CALL; CPU.STACK=0; break;
case RET:  CPU.STACK=1; M_RET; CPU.STACK=0; break;

case SCF:  S(C_FLAG); break;

case CPL:	CPU.AF.B.h=~CPU.AF.B.h;	break;

case NOP:  break;

case OUTA: I=RdCPU(CPU.PC.W++);OutCPU(I,CPU.AF.B.h);break;
case INA:  I=RdCPU(CPU.PC.W++);CPU.AF.B.h=InCPU(I);break;

case HALT:
  CPU.PC.W--;
  CPU.IFF|=IFF_HALT;
//  CPU.IBackup=0;
//  CPU.ICount=0;
  break;

case DI:
//  if(CPU.IFF&IFF_EI)
//	  CPU.ICount+=CPU.IBackup-1;
  CPU.IFF&=~(IFF_1|IFF_2|IFF_EI);
  break;

case EI:
  if(!(CPU.IFF&(IFF_1|IFF_EI)))
  {
    CPU.IFF|=IFF_2|IFF_EI;
//    CPU.IBackup=CPU.ICount;
//    CPU.ICount=1;
  }
  break;

case CCF: CPU.AF.B.l^=C_FLAG; break;

case EX_DE_HL: J.W=CPU.DE.W;CPU.DE.W=CPU.HL.W;CPU.HL.W=J.W;break;

case LD_B_B:   CPU.BC.B.h=CPU.BC.B.h;break;
case LD_C_B:   CPU.BC.B.l=CPU.BC.B.h;break;
case LD_D_B:   CPU.DE.B.h=CPU.BC.B.h;break;
case LD_E_B:   CPU.DE.B.l=CPU.BC.B.h;break;
case LD_H_B:   CPU.HL.B.h=CPU.BC.B.h;break;
case LD_L_B:   CPU.HL.B.l=CPU.BC.B.h;break;
case LD_A_B:   CPU.AF.B.h=CPU.BC.B.h;break;
case LD_xHL_B: WrCPU(CPU.HL.W,CPU.BC.B.h);break;

case LD_B_C:   CPU.BC.B.h=CPU.BC.B.l;break;
case LD_C_C:   CPU.BC.B.l=CPU.BC.B.l;break;
case LD_D_C:   CPU.DE.B.h=CPU.BC.B.l;break;
case LD_E_C:   CPU.DE.B.l=CPU.BC.B.l;break;
case LD_H_C:   CPU.HL.B.h=CPU.BC.B.l;break;
case LD_L_C:   CPU.HL.B.l=CPU.BC.B.l;break;
case LD_A_C:   CPU.AF.B.h=CPU.BC.B.l;break;
case LD_xHL_C: WrCPU(CPU.HL.W,CPU.BC.B.l);break;

case LD_B_D:   CPU.BC.B.h=CPU.DE.B.h;break;
case LD_C_D:   CPU.BC.B.l=CPU.DE.B.h;break;
case LD_D_D:   CPU.DE.B.h=CPU.DE.B.h;break;
case LD_E_D:   CPU.DE.B.l=CPU.DE.B.h;break;
case LD_H_D:   CPU.HL.B.h=CPU.DE.B.h;break;
case LD_L_D:   CPU.HL.B.l=CPU.DE.B.h;break;
case LD_A_D:   CPU.AF.B.h=CPU.DE.B.h;break;
case LD_xHL_D: WrCPU(CPU.HL.W,CPU.DE.B.h);break;

case LD_B_E:   CPU.BC.B.h=CPU.DE.B.l;break;
case LD_C_E:   CPU.BC.B.l=CPU.DE.B.l;break;
case LD_D_E:   CPU.DE.B.h=CPU.DE.B.l;break;
case LD_E_E:   CPU.DE.B.l=CPU.DE.B.l;break;
case LD_H_E:   CPU.HL.B.h=CPU.DE.B.l;break;
case LD_L_E:   CPU.HL.B.l=CPU.DE.B.l;break;
case LD_A_E:   CPU.AF.B.h=CPU.DE.B.l;break;
case LD_xHL_E: WrCPU(CPU.HL.W,CPU.DE.B.l);break;

case LD_B_H:   CPU.BC.B.h=CPU.HL.B.h;break;
case LD_C_H:   CPU.BC.B.l=CPU.HL.B.h;break;
case LD_D_H:   CPU.DE.B.h=CPU.HL.B.h;break;
case LD_E_H:   CPU.DE.B.l=CPU.HL.B.h;break;
case LD_H_H:   CPU.HL.B.h=CPU.HL.B.h;break;
case LD_L_H:   CPU.HL.B.l=CPU.HL.B.h;break;
case LD_A_H:   CPU.AF.B.h=CPU.HL.B.h;break;
case LD_xHL_H: WrCPU(CPU.HL.W,CPU.HL.B.h);break;

case LD_B_L:   CPU.BC.B.h=CPU.HL.B.l;break;
case LD_C_L:   CPU.BC.B.l=CPU.HL.B.l;break;
case LD_D_L:   CPU.DE.B.h=CPU.HL.B.l;break;
case LD_E_L:   CPU.DE.B.l=CPU.HL.B.l;break;
case LD_H_L:   CPU.HL.B.h=CPU.HL.B.l;break;
case LD_L_L:   CPU.HL.B.l=CPU.HL.B.l;break;
case LD_A_L:   CPU.AF.B.h=CPU.HL.B.l;break;
case LD_xHL_L: WrCPU(CPU.HL.W,CPU.HL.B.l);break;

case LD_B_A:   CPU.BC.B.h=CPU.AF.B.h;break;
case LD_C_A:   CPU.BC.B.l=CPU.AF.B.h;break;
case LD_D_A:   CPU.DE.B.h=CPU.AF.B.h;break;
case LD_E_A:   CPU.DE.B.l=CPU.AF.B.h;break;
case LD_H_A:   CPU.HL.B.h=CPU.AF.B.h;break;
case LD_L_A:   CPU.HL.B.l=CPU.AF.B.h;break;
case LD_A_A:   CPU.AF.B.h=CPU.AF.B.h;break;
case LD_xHL_A: WrCPU(CPU.HL.W,CPU.AF.B.h);break;

case LD_xBC_A: WrCPU(CPU.BC.W,CPU.AF.B.h);break;
case LD_xDE_A: WrCPU(CPU.DE.W,CPU.AF.B.h);break;

case LD_B_xHL:    CPU.BC.B.h=RdCPU(CPU.HL.W);break;
case LD_C_xHL:    CPU.BC.B.l=RdCPU(CPU.HL.W);break;
case LD_D_xHL:    CPU.DE.B.h=RdCPU(CPU.HL.W);break;
case LD_E_xHL:    CPU.DE.B.l=RdCPU(CPU.HL.W);break;
case LD_H_xHL:    CPU.HL.B.h=RdCPU(CPU.HL.W);break;
case LD_L_xHL:    CPU.HL.B.l=RdCPU(CPU.HL.W);break;
case LD_A_xHL:    CPU.AF.B.h=RdCPU(CPU.HL.W);break;

case LD_B_BYTE:   CPU.BC.B.h=RdCPU(CPU.PC.W++);break;
case LD_C_BYTE:   CPU.BC.B.l=RdCPU(CPU.PC.W++);break;
case LD_D_BYTE:   CPU.DE.B.h=RdCPU(CPU.PC.W++);break;
case LD_E_BYTE:   CPU.DE.B.l=RdCPU(CPU.PC.W++);break;
case LD_H_BYTE:   CPU.HL.B.h=RdCPU(CPU.PC.W++);break;
case LD_L_BYTE:   CPU.HL.B.l=RdCPU(CPU.PC.W++);break;
case LD_A_BYTE:   CPU.AF.B.h=RdCPU(CPU.PC.W++);break;
case LD_xHL_BYTE: WrCPU(CPU.HL.W,RdCPU(CPU.PC.W++));break;

case LD_xWORD_HL:
  J.B.l=RdCPU(CPU.PC.W++);
  J.B.h=RdCPU(CPU.PC.W++);
  WrCPU(J.W++,CPU.HL.B.l);
  WrCPU(J.W,CPU.HL.B.h);
  break;

case LD_HL_xWORD:
  J.B.l=RdCPU(CPU.PC.W++);
  J.B.h=RdCPU(CPU.PC.W++);
  CPU.HL.B.l=RdCPU(J.W++);
  CPU.HL.B.h=RdCPU(J.W);
  break;

case LD_A_xWORD:
  J.B.l=RdCPU(CPU.PC.W++);
  J.B.h=RdCPU(CPU.PC.W++); 
  CPU.AF.B.h=RdCPU(J.W);
  break;

case LD_xWORD_A:
  J.B.l=RdCPU(CPU.PC.W++);
  J.B.h=RdCPU(CPU.PC.W++);
  WrCPU(J.W,CPU.AF.B.h);
  break;

case EX_HL_xSP:
  CPU.STACK = 1;
  J.B.l=RdCPU(CPU.SP.W);WrCPU(CPU.SP.W++,CPU.HL.B.l);
  J.B.h=RdCPU(CPU.SP.W);WrCPU(CPU.SP.W--,CPU.HL.B.h);
  CPU.STACK=0;
  CPU.HL.W=J.W;
  break;

case DAA:
  J.W=CPU.AF.B.h;
  if(CPU.AF.B.l&C_FLAG) J.W|=256;
  if(CPU.AF.B.l&H_FLAG) J.W|=512;
  CPU.AF.W=DAATable[J.W];
  break;

case JP_1:   M_JP;break;
case CALL_1:
case CALL_2:
case CALL_3:
	CPU.STACK=1; M_CALL; CPU.STACK=0;
	break;
case RET_1: CPU.STACK=1; M_RET; CPU.STACK=0; break;
case NOP1:
case NOP2:
case NOP3:
case NOP4:
case NOP5:
case NOP6:
case NOP7:
	break;

default:
  break;


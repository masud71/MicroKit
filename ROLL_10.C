#include "defina_n.h"


typedef union{
		unsigned int x;
		signed int sx;
                unsigned char y[2];
                signed char sy;
	      }abc;
abc     opcode;
int     dreg,md,sz,dr,count;
typedef unsigned long int ULI;

ULI effective_addr(int,int);

void LSL(void)
   {
	  LSHFT(1);
   }

void LSR(void)
   {
	 LSHFT(0);
   }

LSHFT(int n)
   {
      opcode.y[1] = memory[PC++];
      opcode.y[0] = memory[PC++];
      dreg = opcode.x & 0x7;
      opcode.x = opcode.x >> 3;
      md = opcode.x & 0x7;
      opcode.x = opcode.x >> 3;
      sz = opcode.x & 0x3;
      opcode.x = opcode.x >> 2;
      dr = opcode.x & 0x1;
      opcode.x = opcode.x >> 1;
      count = opcode.x & 0x7;
      if((md>3)&&(sz<3)) count = D[count].dword & 63 ;
      FLAG.v = 0;
      if ((count==0) && (sz != 3)) FLAG.c = 0;
	  else if(n) LSHL(sz,md,dreg,count);
	   else LSHR(sz,md,dreg,count);
   }

LSHL(int sz, int md, int dreg, int count)
   {
      ULI EA;
      abc opnd;

      switch(sz) {
		case 0: D[dreg].byte = D[dreg].byte << (count-1);
			if(D[dreg].byte & 0x80)
			   FLAG.c = FLAG.n = 1;
			else FLAG.c = FLAG.x = 0;
			D[dreg].byte = D[dreg].byte <<1;
			if(D[dreg].byte & 0x80) FLAG.n =1;
			 else FLAG.n = 0;
			if(D[dreg].byte) FLAG.z = 0;
			 else FLAG.z = 1;
			break;
		case 1: D[dreg].word = D[dreg].word << (count-1);
			if(D[dreg].word & 0x8000)
			   FLAG.c = FLAG.x = 1;
			else FLAG.c = FLAG.x = 0;
			D[dreg].word = D[dreg].word <<1;
			if(D[dreg].word & 0x8000) FLAG.n =1;
			 else FLAG.n = 0;
			if(D[dreg].word) FLAG.z = 0;
			 else FLAG.z = 1;
			break;
		case 2: D[dreg].dword = D[dreg].dword << (count-1);
			if(D[dreg].dword & 0x8000)
			   FLAG.c = FLAG.x = 1;
			else FLAG.c = FLAG.x = 0;
			D[dreg].dword = D[dreg].dword <<1;
			if(D[dreg].dword & 0x8000) FLAG.n = 1;
			 else FLAG.n = 0;
			if(D[dreg].dword) FLAG.z = 0;
			 else FLAG.z = 1;
			break;
		case 3: EA = effective_addr(dreg,md);
			opnd.y[1] = memory[EA];
			opnd.y[0] = memory[EA+1];
			if(opnd.x & 0x8000) FLAG.c = FLAG.x = 1;
			else FLAG.c = FLAG.x = 0;
			opnd.x = opnd.x << 1;
			if(opnd.x & 0x8000) FLAG.n = 1;
			 else FLAG.n = 0;
			if(opnd.x) FLAG.z = 0;
			 else FLAG.z = 1;
                        memory[EA] = opnd.y[1];
                        memory[EA+1] = opnd.y[0];
			break;
    }
}

LSHR()
  {
     ULI EA;
     abc opnd;

     switch(sz) {
	      case 0: D[dreg].byte = D[dreg].byte >>(count-1);
		      if(D[dreg].byte & 1) FLAG.c = FLAG.x = 1;
			else  FLAG.c = FLAG.x = 0;
		      D[dreg].byte = D[dreg].byte >> 1;
		      FLAG.n = 0;
		      if(D[dreg].byte) FLAG.z = 0;
			else FLAG.z = 1;
		      break;
		case 1: D[dreg].word = D[dreg].word >>(count-1);
		      if(D[dreg].word & 1) FLAG.c = FLAG.x = 1;
			else  FLAG.c = FLAG.x = 0;
		      D[dreg].word = D[dreg].word >> 1;
		      FLAG.n = 0;
		      if(D[dreg].word) FLAG.z = 0;
			else FLAG.z = 1;
		      break;

		case 2: D[dreg].dword = D[dreg].dword >>(count-1);
			 if(D[dreg].dword & 1) FLAG.c = FLAG.x = 1;
			   else  FLAG.c = FLAG.x = 0;
			D[dreg].dword = D[dreg].dword >> 1;
			FLAG.n = 0;
			if(D[dreg].dword) FLAG.z = 0;
			  else FLAG.z = 1;
			break;

		case 3: EA = effective_addr(dreg,md);
			opnd.y[1] = memory[EA];
			opnd.y[0] = memory[EA+1];
			 if(opnd.x & 1) FLAG.c = FLAG.x = 1;
			   else FLAG.c = FLAG.x = 0;
			opnd.x = opnd.x >> 1;
			FLAG.n = 0;
			if(opnd.x) FLAG.z = 0;
			  else FLAG.z = 1;
                        memory[EA] = opnd.y[1];
                        memory[EA+1] = opnd.y[0];
			  break;
	      }
	 }








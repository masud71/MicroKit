

	  /* INCLUDED HEADER FILES
	  -------------------------*/
#include "defina_n.h"


	  /* TYPE DEFINITION
	  -------------------*/
typedef unsigned long int ULI;
typedef union {
	unsigned int word;
	unsigned char byte[2];
} WORD;


	  /* FUNCTIONS DECLARATION
	  -------------------------*/
ULI    effective_addr(int, int);
void   shift(int);
void   ASL();
void   ASR();
void   left(int, int, int, int);
void   right(int, int, int, int);




	  /* FUNCTIONS DEFINITION
	  ------------------------
	  ------------------------*/


	  /* ARITHMETC SHIFT LEFT
	  ------------------------*/

void ASL() {
     shift(1);
}



	 /* ARITHMETC SHIFT LEFT
	  ------------------------*/

void ASR() {
     shift(0);
}



	  /* ARITHMATIC SHIFT
	  --------------------*/

void shift(int leftshift) {

     WORD  R16;                         /* 16 BITS REGISTER DEFINITION */
     int   dest, mode, ss, q_r;         /* q_r -> Quantity or register */

     R16.byte[0] = memory[PC++];        /* FETCHING OPCODE WORD */
     R16.byte[1] = memory[PC++];

     dest = R16.word & 0x7;             /* PARTING OF OPCODE WORD */
     R16.word = R16.word >> 3;          /* opcode word = 1110qqqfssmmmddd */
     mode = R16.word & 0x7;
     R16.word = R16.word >> 3;
     ss = R16.word & 0x3;
     R16.word = R16.word >> 3;
     q_r = R16.word & 0x7;
					/* SHIFTING */
     if (mode==4) q_r = D[q_r].byte & 63;
     if ((ss != 3) && !q_r) FLAG.c = 0;
     else if (leftshift) left(ss, dest, q_r, mode);
	  else right(ss, dest, q_r, mode);
}



	  /* SHIFT LEFT D[dest] reg BY q_r BIT(s)
	  ---------------------------------------*/

void left(int ss, int dest, int q_r, int mode) {
     ULI   EA;                          /* Effective Address */
     WORD  R16;                         /* 16 BITS REGISTER DEFINITION */

     switch (ss) {
	case  0: if (q_r > 8) q_r = 8;
		 FLAG.v = (D[dest].byte & 0x80) ? 1 : 0;
		 D[dest].byte <<= (q_r-1);
		 FLAG.c = FLAG.x = (D[dest].byte & 0x80) ? 1 : 0;
		 D[dest].byte <<= 1;
		 FLAG.z = D[dest].byte ? 0 : 1;
		 FLAG.n = (D[dest].byte & 0x80) ? 1 : 0;
		 if (FLAG.n) FLAG.v = !(FLAG.v);            /* v = v XOR n */
		 break;
	case  1: if (q_r > 16) q_r = 16;
		 FLAG.v = (D[dest].word & 0x8000) ? 1 : 0;
		 D[dest].word <<= (q_r-1);
		 FLAG.c = FLAG.x = (D[dest].word & 0x8000) ? 1 : 0;
		 D[dest].word <<= 1;
		 FLAG.z = D[dest].word ? 0 : 1;
		 FLAG.n = (D[dest].word & 0x8000) ? 1 : 0;
		 if (FLAG.n) FLAG.v = !(FLAG.v);            /* v = v XOR n */
		 break;
	case  2: if (q_r > 32) q_r = 32;
		 FLAG.v = (D[dest].dword & 0x80000000) ? 1 : 0;
		 D[dest].dword <<= (q_r-1);
		 FLAG.c = FLAG.x = (D[dest].dword & 0x80000000) ? 1 : 0;
		 D[dest].dword <<= 1;
		 FLAG.z = D[dest].dword ? 0 : 1;
		 FLAG.n = (D[dest].dword & 0x80000000) ? 1 : 0;
		 if (FLAG.n) FLAG.v = !(FLAG.v);            /* v = v XOR n */
		 break;
	case  3: if (q_r > 16) q_r = 16;
		 EA = effective_addr(dest, mode);
		 R16.byte[0] = memory[EA];
		 R16.byte[1] = memory[EA+1];
		 FLAG.c = FLAG.x = (R16.word & 0x8000) ? 1 : 0;
		 R16.word <<= 1;
		 memory[EA] = R16.byte[0];
		 memory[EA+1] = R16.byte[1];
		 FLAG.z = R16.word ? 0 : 1;
		 FLAG.n = (R16.word & 0x8000) ? 1 : 0;
		 FLAG.v = FLAG.n ? !FLAG.c : FLAG.c;        /* v = n XOR c */
     }
}



	  /* SHIFT LEFT D[dest] reg BY q_r BIT(s)
	  ---------------------------------------*/

void right(int ss, int dest, int q_r, int mode) {
     ULI   EA;                          /* Effective Address */
     WORD  R16;                         /* 16 BITS REGISTER DEFINITION */
     int i;

     switch (ss) {
	case  0: if (q_r > 8) q_r = 8;
		 FLAG.n = (D[dest].byte & 0x80) ? 1 : 0;
		 for (i=0; i<q_r; i++) {
		     FLAG.c = FLAG.x = D[dest].byte & 1;
		     D[dest].byte >>= 1;
		     if (FLAG.n) D[dest].byte |= 0x80;
		 }
		 FLAG.z = D[dest].byte ? 0 : 1;
		 break;
	case  1: if (q_r > 16) q_r = 16;
		 FLAG.n = (D[dest].word & 0x8000) ? 1 : 0;
		 for (i=0; i<q_r; i++) {
		     FLAG.c = FLAG.x = D[dest].word & 1;
		     D[dest].word >>= 1;
		     if (FLAG.n) D[dest].word |= 0x8000;
		 }
		 FLAG.z = D[dest].word ? 0 : 1;
		 break;
	case  2: if (q_r > 32) q_r = 32;
		 FLAG.n = (D[dest].dword & 0x80000000) ? 1 : 0;
		 for (i=0; i<q_r; i++) {
		     FLAG.c = FLAG.x = D[dest].dword & 1;
		     D[dest].word >>= 1;
		     if (FLAG.n) D[dest].word |= 0x80000000;
		 }
		 break;
	case  3: if (q_r > 16) q_r = 16;
		 EA = effective_addr(dest, mode);
		 R16.byte[0] = memory[EA];
		 R16.byte[1] = memory[EA+1];
		 FLAG.c = FLAG.x = R16.word & 1;
		 FLAG.n = (R16.word & 0x8000) ? 1 : 0;
		 R16.word >>= 1;
		 if (FLAG.n) R16.word |= 0x8000;
		 memory[EA] = R16.byte[0];
		 memory[EA+1] = R16.byte[1];
		 FLAG.z = R16.word ? 0 : 1;
     }
     FLAG.v = 0;
}



	  /* CALCULATION OF EFFECTIVE ADDRESS
	  ------------------------------------*/

ULI effective_addr(int n, int mode) {   /* n -> Register no. */
    ULI   EA;
    WORD  data;
    int   longA, Areg, regno;

    switch (mode) {
	   case 2: EA = A[n].dword;
		   break;
	   case 3: EA = A[n].dword;
		   A[n].dword += 2;
		   break;
	   case 4: A[n].dword -= 2;
		   EA = A[n].dword;
		   break;
	   case 5: data.byte[0] = memory[PC++];
		   data.byte[1] = memory[PC++];
		   EA = A[n].dword + data.word;
		   break;
	   case 6: data.byte[0] = memory[PC++];
		   data.byte[1] = memory[PC++];
		   data.byte[0] >>= 3;
		   longA = data.byte[0] & 1;
		   data.byte[0] >>= 1;
		   regno = data.byte[0] & 7;
		   data.byte[0] >>= 3;
		   Areg = data.byte[0] & 1;
		   EA = A[n].dword + data.byte[1];
		   if (Areg)
		      if (longA) EA += A[regno].dword;
		      else EA += A[regno].word;
		   else if (longA) EA += D[regno].dword;
			else EA += D[regno].word;
		   break;
	   case 7: data.byte[0] = memory[PC++];
		   data.byte[1] = memory[PC++];
		   EA = 0;
		   switch (n) {
		     case  0: EA = data.word;
			      break;
		     case  1: EA = data.word;
			      EA <<= 16;
			      data.byte[0] = memory[PC++];
			      data.byte[1] = memory[PC++];
			      EA += data.word;
		   }
    }
    return EA;
}


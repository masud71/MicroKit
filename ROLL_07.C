#include "defina_n.h"

/* BRANCH TO PC+displacement IF N=1 ELSE TO NEXT INSTRUCTION */
void BMI(void)
{
	int disp;

	if(FLAG.n == 1) {
		disp = (signed char) memory[PC+1];
		if(disp==0) disp=memory[PC+2]*0x100 + memory[PC+3];
		PC = PC+disp+2;
	}
	else if(memory[PC+1]==0) PC=PC+2;
	PC = PC + 2;
}
/* BRANCH TO PC+displacement IF Z=0 ELSE TO NEXT INSTRUCTION */
void BNE(void)
{
	int disp;

	if(FLAG.z == 0) {
		disp = (signed char) memory[PC+1];
		if(disp==0) disp=memory[PC+2]*0x100 + memory[PC+3];
		PC = PC+disp+2;
	}
	else if(memory[PC+1]==0) PC=PC+2;
	PC = PC + 2;
}
/* BRANCH TO PC+displacement IF N=0 ELSE TO NEXT INSTRUCTION */
void BPL(void)
{
	int disp;

	if(FLAG.n == 0) {
		disp = (signed char) memory[PC+1];
		if(disp==0) disp=memory[PC+2]*0x100 + memory[PC+3];
		PC = PC+disp + 2;
	}
	else if(memory[PC+1]==0) PC=PC+2;
	PC = PC + 2;
}
/* BRANCH ALWAYS TO PC+d */
void BRA(void)
{
	int disp;

	disp = (signed char) memory[PC+1];
	if(disp==0) disp=memory[PC+2]*0x100 + memory[PC+3];
	PC = PC+disp+2;
}


void BTST_Dn(void)
{
	struct addr_info {
		unsigned regno  : 3;
		unsigned mode   : 3;
		unsigned size   : 3;
		unsigned reg    : 3;
		unsigned opcode : 4;
		unsigned bit    : 1;
	} addr;
	unsigned long int EA;
	unsigned int operation; /*imm_data;*/

	operation = memory[PC]*0x100 + memory[PC+1];
	addr.regno = operation;
	addr.mode = operation >> 3;
	addr.size = operation >>6;
	addr.reg = operation >>9;
	addr.opcode = operation >>12;
	PC = PC + 2;
	switch(addr.mode) {
		case 0:
			switch(addr.size){
				case 0: /* Dn IS BYTE*/
					addr.bit = D[addr.regno].dword >> ((D[addr.reg].byte)%32);
					break;
				case 1: /* Dn IS WORD*/
					addr.bit = D[addr.regno].dword >> ((D[addr.reg].word)%32);
					break;
				case 2: /* Dn IS DOUBLE WORD*/
					addr.bit = D[addr.regno].dword >> ((D[addr.reg].dword)%32);
					break;
			}
		break;
		case 1:
			return;
		case 7:
			if(addr.regno==4) return;
		default:
			EA = find_EA(addr.mode, addr.regno, 0);
      switch(addr.size){
				case 0: /* Dn IS BYTE*/
					addr.bit = memory[EA] >> ((D[addr.reg].byte)%8);
					break;
				case 1: /* Dn IS WORD*/
					addr.bit = memory[EA] >> ((D[addr.reg].word)%8);
					break;
				case 2: /* Dn IS DOUBLE WORD*/
					addr.bit = memory[EA] >> ((D[addr.reg].dword)%8);
					break;
			 }
	}
	if(addr.bit==0)
		FLAG.z = 1;
	else FLAG.z = 0;
}

void BSET_Dn(void)
{
	struct addr_info {
		unsigned regno  : 3;
		unsigned mode   : 3;
		unsigned size   : 3;
		unsigned reg    : 3;
		unsigned opcode : 4;
    unsigned bit    : 1;
	} addr;

	unsigned long int EA,ds=1;
	unsigned int operation; /*imm_data;*/
	unsigned char es=1;

	operation = memory[PC]*0x100 + memory[PC+1];
	addr.regno = operation;
	addr.mode = operation >> 3;
	addr.size = operation >>6;
	addr.reg = operation >>9;
	addr.opcode = operation >>12;
	PC = PC + 2;
	switch(addr.mode) {
		case 0:
			switch(addr.size){
				case 0: /* Dn IS BYTE*/
					addr.bit = D[addr.regno].dword >> ((D[addr.reg].byte)%32);
					ds >>= ((D[addr.reg].byte)%32);
					break;
				case 1: /* Dn IS WORD*/
					addr.bit = D[addr.regno].dword >> ((D[addr.reg].word)%32);
					ds >>= ((D[addr.reg].word)%32);
					break;
				case 2: /* Dn IS DOUBLE WORD*/
					addr.bit = D[addr.regno].dword >> ((D[addr.reg].dword)%32);
					ds >>= ((D[addr.reg].dword)%32);
					break;
			}
		 if( addr.bit == 0 ) D[addr.regno].dword+=ds;
		break;
		case 1:
			return;
		case 7:
			if(addr.regno==4) return;
		default:
			EA = find_EA(addr.mode, addr.regno, 0);
      switch(addr.size){
				case 0: /* Dn IS BYTE*/
					addr.bit = memory[EA] >> ((D[addr.reg].byte)%8);
					es >>= ((D[addr.reg].byte)%8);
					break;
				case 1: /* Dn IS WORD*/
					addr.bit = memory[EA] >> ((D[addr.reg].word)%8);
					es >>= ((D[addr.reg].word)%8);
					break;
				case 2: /* Dn IS DOUBLE WORD*/
					addr.bit = memory[EA] >> ((D[addr.reg].dword)%8);
					es >>= ((D[addr.reg].dword)%8);
					break;
			 }
			 if( addr.bit == 0 ) memory[EA]+=es;
	}
	if(addr.bit==0)
		FLAG.z = 1;
	else FLAG.z = 0;
}

void BSET_Imm(void)
{
	struct addr_info {
		unsigned regno  : 3;
		unsigned mode   : 3;
		unsigned bit    : 1;
	} addr;
	unsigned long int EA,ds=1;
	unsigned int imm_data;
	unsigned char es=1;
	addr.regno = memory[PC+1];
	addr.mode = memory[PC+1] >> 3;
	PC = PC + 2;
	imm_data = memory[PC]*0x100 + memory[PC+1];
	PC = PC + 2;
	switch(addr.mode) {
		case 0:
			addr.bit=D[addr.regno].dword >> (imm_data%32);
			if (addr.bit == 0) {
				ds >>= (imm_data%32);
				D[addr.regno].dword += ds;
			}
			break;
		case 1:
			return;
		case 7:
			if(addr.regno==4) return;
		default:
			EA = find_EA(addr.mode, addr.regno, 0);
			addr.bit = memory[EA] >> (imm_data%8);
      if (addr.bit == 0) {
				es >>= (imm_data%8);
				memory[EA] += es;
			}
	}
	if(addr.bit==0)
		FLAG.z = 1;
	else FLAG.z = 0;
}
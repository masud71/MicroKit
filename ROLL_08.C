/***************************************************************************

		ABUL HASNAT MAMUN AL MAZID
			ROLL# 8
			CSE 3RD YEAR
			SESSION: 1991-92

 **************************************************************************/

#include "defina_n.h"

unsigned long int find_EA(short int mode,short int regno,short int size);

/**************************************************************************/
void BTST_Imm(void)
{
	struct addr_info {              /*format of the lower 6-bit*/
		unsigned regno  : 3;        /*of the opcode word*/
		unsigned mode   : 3;
		unsigned bit    : 1;
	} addr;
	unsigned long int EA;
	unsigned int imm_data;

	addr.regno = memory[PC+1];
	addr.mode = memory[PC+1] >> 3;
	PC = PC + 2;                     /*pc ponits to the immediate data*/
	imm_data = memory[PC]*0x100 + memory[PC+1];
	PC = PC + 2;                     /*pc points to the  next instruction*/
	switch(addr.mode) {
		case 0:
			addr.bit=D[addr.regno].dword >> (imm_data%32);
			break;                   /*required bit value is taken from Dn*/
		case 1:
			return;
		case 7:
			if(addr.regno==4) return;
		default:
			EA = find_EA(addr.mode, addr.regno, 0);
			addr.bit = memory[EA] >> (imm_data%8);
							/*required bit value is taken from memory*/
	}
	if(addr.bit==0)
		FLAG.z = 1;
	else FLAG.z = 0;
}

/**************************************************************************/
void BVC(void)
{
	int disp;

	if(FLAG.v == 0) {
		disp = (signed char) memory[PC+1];          	/*sign extension*/
		if(disp==0) disp=memory[PC+2]*0x100 + memory[PC+3];
		PC = PC+disp+2;                    		   /*16-bit displacement*/
	}
	else if(memory[PC+1]==0) PC=PC+2;			/*skip 16-bit displacement*/
	PC = PC + 2;
}

/**************************************************************************/
void BVS(void)
{
	int disp;

	if(FLAG.v == 1) {
		disp = (signed char) memory[PC+1];         /*sign extension*/
		if(disp==0) disp=memory[PC+2]*0x100 + memory[PC+3];
		PC = PC+disp+2;                           /*16-bit displacement*/
	}
	else if(memory[PC+1]==0) PC=PC+2;			/*skip 16-bit displacement*/
	PC = PC + 2;
}

/**************************************************************************/
void CLR(void)
{
	struct opword {
		unsigned regno  : 3;           /*format of the opcode word*/
		unsigned mode   : 3;
		unsigned size   : 2;
		unsigned        : 8;
	} ow;
	unsigned long int EA;
	unsigned int operation;

	operation = memory[PC]*0x100 + memory[PC+1];   	/*fetch opcode*/
	ow.regno = operation;
	ow.mode = operation >> 3;
	ow.size = operation >> 6;
	PC = PC + 2;
	switch(ow.mode) {
		case 0:
        	switch(ow.size) {
				case 0:
					D[ow.regno].byte=0;
					break;
				case 1:
					D[ow.regno].word=0;
					break;
				case 2:
					D[ow.regno].dword=0;
					break;
			}
			break;
		case 1:
			return;                 			/*not allowed*/
		default:
			if(ow.mode==7 && ow.regno>1) {
				PC = PC + 2;
				if((ow.regno==4) && (ow.size==2)) 	PC = PC + 2;
				return;
			}
			EA = find_EA(ow.mode, ow.regno, ow.size);
			switch(ow.size) {
				case 0:
					memory[EA]=0;
					break;
				case 1:
					memory[EA]=0;
					memory[EA+1]=0;
					break;
				case 2:
					memory[EA]=0;
					memory[EA+1]=0;
					memory[EA+2]=0;
					memory[EA+3]=0;
			}
	}
	FLAG.n = 0;
	FLAG.z = 1;
	FLAG.v = 0;
	FLAG.c = 0;
}

/**************************************************************************/
void CMP(void)
{
	struct opword {
		unsigned regno  : 3;          /*format of the opcode word*/
		unsigned mode   : 3;
		unsigned size   : 3;
		unsigned reg    : 3;
		unsigned opcode : 4;
	} ow;
	D_REGS operand,result;
	unsigned long int EA;
	unsigned int operation;

	operation = memory[PC]*0x100 + memory[PC+1];       /*fetch opcode*/
	ow.regno = operation;
	ow.mode = operation >> 3;
	ow.size = operation >> 6;
	ow.reg = operation >> 9;
	ow.opcode = operation >> 12;
	PC = PC + 2;
	switch(ow.size) {
		case 0:
        	switch(ow.mode) {
				case 0:
					operand.byte=D[ow.regno].byte;
					break;
				case 1:
					return; /*Address regs do not support byte operand*/
				default:
					EA = find_EA(ow.mode, ow.regno, ow.size);
					operand.byte = memory[EA];
			}
			result.byte=D[ow.reg].byte-operand.byte;
			if(result.byte==0) FLAG.z=1;
			else {
				FLAG.z=0;
				if(result.byte<0) {
					FLAG.n=1;
					FLAG.c=1;
				}
			}
			break;
		case 1:
        	switch(ow.mode) {
				case 0:
					operand.word=D[ow.regno].word;
					break;
				case 1:
					operand.word=A[ow.regno].word;
					break;
				default:
					EA = find_EA(ow.mode, ow.regno, ow.size);
					operand.word=memory[EA]*0x100 + memory[EA+1];
			}
			result.word=D[ow.reg].word-operand.word;
			if(result.word==0) FLAG.z=1;
			else {
				FLAG.z=0;
				if(result.dword<0) {
					FLAG.n=1;
					FLAG.c=1;
				}
			}
			break;
		case 2:
			switch(ow.mode) {
				case 0:
					operand.dword=D[ow.regno].dword;
					break;
				case 1:
					operand.dword=A[ow.regno].dword;
					break;
				default:
					EA = find_EA(ow.mode, ow.regno, ow.size);
					operand.dword=memory[EA]*0x1000000 + memory[EA+1]*0x10000
									+ memory[EA+2]*0x100 + memory[EA+3];
			}
			result.dword=D[ow.reg].dword-operand.dword;
			if(result.dword==0) FLAG.z=1;
			else {
				FLAG.z=0;
				if(result.dword<0) {
					FLAG.n=1;
					FLAG.c=1;
				}
			}
			break;
	}
}

/**************************************************************************/
unsigned long int find_EA(short int mode,short int regno,short int size)
{
	struct x_info {
		unsigned       : 3;			/*information of the index register*/
		unsigned WorL  : 1;
		unsigned reg   : 3;
		unsigned rtype : 1;
	} index;
	unsigned long int EA;

	index.WorL = memory[PC] >> 3;
	index.reg = memory[PC] >> 4;
	index.rtype = memory[PC] >> 7;
	switch(mode) {
		case 2:									/*register indirect mode*/
			EA=A[regno].dword;
			break;
		case 3:						/*register indirect with autoincrement*/
			EA=A[regno].dword;
			switch(size) {
				case 0:
					A[regno].dword=A[regno].dword+1;
					break;
				case 1:
					A[regno].dword=A[regno].dword+2;
					break;
				case 2:
					A[regno].dword=A[regno].dword+4;
					break;
			}
			break;
		case 4:						/*register indirect with autodecrement*/
            switch(size) {
				case 0:
					A[regno].dword=A[regno].dword-1;
					break;
				case 1:
					A[regno].dword=A[regno].dword-2;
					break;
				case 2:
					A[regno].dword=A[regno].dword-4;
					break;
			}
			EA=A[regno].dword;
			break;
		case 5:                						/*indexed basic mode*/
			EA=A[regno].dword+memory[PC]*0x100+memory[PC+1];
			PC=PC+2;
			break;
		case 6:										/*indexed full*/
			switch(index.rtype) {
				case 0:                        /*index register Dn*/
					if(index.WorL==0)
						EA=A[regno].dword+D[index.reg].word+(signed char)memory[PC+1];
					else EA=A[regno].dword+D[index.reg].dword+(signed char)memory[PC+1];
					break;
				case 1:                        /*index register An*/
            		if(index.WorL==0)
						EA=A[regno].dword+A[index.reg].word+(signed char)memory[PC+1];
					else EA=A[regno].dword+A[index.reg].dword+(signed char)memory[PC+1];
					break;
			}
			PC=PC+2;
			break;
		case 7:
			switch(regno) {
				case 0:									/*absolute short*/
					EA=memory[PC]*0x100+memory[PC+1];
					PC=PC+2;
					break;
				case 1:                                 /*absolute long*/
					EA=memory[PC]*0x1000000 + memory[PC+1]*0x10000
						+ memory[PC+2]*0x100 + memory[PC+3];
					PC=PC+4;
					break;
				case 2:                                 /*relative basic*/
					EA=PC + memory[PC]*0x100 + memory[PC+1];
					PC=PC+2;
					break;
				case 3:									/*relative full*/
                	switch(index.rtype) {
						case 0:                        /*index register Dn*/
							if(index.WorL==0)
								EA=PC+D[index.reg].word+(signed char)memory[PC+1];
							else EA=PC+D[index.reg].dword+(signed char)memory[PC+1];
							break;
						case 1:   					   /*index register An*/
							if(index.WorL==0)
								EA=PC+A[index.reg].word+(signed char)memory[PC+1];
							else EA=PC+A[index.reg].dword+(signed char)memory[PC+1];
							break;
					}
					PC=PC+2;
					break;
				case 4:									/*immediate mode*/
					EA=PC;
					switch(size) {                      /*8-bit operand*/
						case 0:
							EA=PC+1;
							PC=PC+2;
							break;
						case 1:   						/*16-bit operand*/
							PC=PC+2;
							break;
						case 2:                         /*32-bit operand*/
							PC=PC+4;
							break;
					}
					break;
			}
	}
	return EA;
}
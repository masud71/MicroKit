#include "defina_n.h"

#define	MAXMEM 		3072		/* Maximum memory size */

void MOVEQ(void);
void MOVEP(void);
void MOVEM(void);
void byte_to_long (long int *);
void word_to_long (long int *);
void move_memory_to_register(unsigned short int,long int *);
void move_register_to_memory(unsigned short int,long int);
void move_for_predecrement_mode(unsigned short int,long int *);
void find_mode_address(unsigned short int,unsigned short int *,long int *);


void word_to_long ( long int *disp)
{
	if (*disp & 0x8000) *disp += 0xffff0000;
}

void byte_to_long ( long int *disp)
{
	if (*disp & 0x80) *disp += 0xffffff00;
}



void MOVEQ( void )
{
	unsigned short int  low,high,regno;

	high = memory[PC];
	low = memory[PC+1];
	regno = (high & 0x0e) >> 1;   /* Data regs. no. in regno now */
	D[regno].dword = low;
	if (low  & 0x80){             /* Negative sign extended */
		D[regno].dword |= 0xffffff00;
		FLAG.n = 1;
	}
	else {
		D[regno].dword &= 0x000000ff; /* Positive sign extended */
		FLAG.n = 0;
	}
	if (low) FLAG.z = 0;
	else FLAG.z = 1;
	FLAG.v = 0;
	FLAG.c = 0;
	PC += 2;
}


void MOVEP( void )
{
	unsigned short int  low,high,dreg_no,areg_no,mode;
	int  lcv;
	long int addr,disp;

	high = memory[PC];
	low = memory[PC+1];
	dreg_no = (high & 0x0e) >> 1;
	areg_no = low & 0x07;
	disp = memory[PC+2] << 8;
	disp += memory[PC+3];       /* 16 bit displacement now in disp */
	word_to_long(&disp);
	addr = A[areg_no].dword + disp;
	mode = (low & 0xc0) >> 6;  /* mode specifies direction & size */
	switch(mode) {
		case 0:		/* memory to data regs. for word */
			D[dreg_no].word = memory[addr] << 8;
			D[dreg_no].word += memory[addr+2];
			break;
		case 1:     /* memory to data regs. for longword */
			D[dreg_no].dword = memory[addr];
			for (lcv = 0; lcv < 3; lcv++){
				addr += 2;
				D[dreg_no].dword <<= 8;
				D[dreg_no].dword += memory[addr];
			}
			break;
		case 2:		/* data regs. to memory for word */
			memory[addr+2] = D[dreg_no].byte;
			memory[addr] = (D[dreg_no].word >> 8) & 0xff;
			break;

		case 3:     /* data regs. to memory for longword */
			addr += 6;
			memory[addr] = D[dreg_no].byte;
			for (lcv = 1; lcv < 4; lcv++){
				addr -= 2;
				memory[addr] = (D[dreg_no].dword >> ( 8 * lcv)) & 0xff;
			}
			break;
		}
		PC += 4;
	}



void move_memory_to_register(unsigned short int size,long int *addr)
{
	unsigned short int regs,lcv;

	regs = memory[PC+3];  /* lower byte of register mask field */
	for(lcv = 0;lcv < 8;lcv++) {
		if (regs & 0x01) {
			if(size) {           /* for long word */
				D[lcv].dword = (memory[*addr] << 8) + memory[*addr+1];
				*addr += 2;
				D[lcv].dword = (D[lcv].dword << 8) + memory[(*addr)++];
				D[lcv].dword = (D[lcv].dword << 8) + memory[(*addr)++];
			}
			else {		/* for word */
				D[lcv].word = (memory[*addr] << 8) + memory[*addr+1];
				*addr += 2;
			}
		}
		regs >>= 1;
	}
	regs = memory[PC+2];    /* higher byte of register mask field */
    for(lcv = 0;lcv < 8;lcv++) {
		if (regs & 0x01) {
			if(size) {        /* for long word */
				A[lcv].dword = (memory[*addr] << 8) + memory[*addr+1];
				*addr += 2;
				A[lcv].dword = (A[lcv].dword << 8) + memory[(*addr)++];
				A[lcv].dword = (A[lcv].dword << 8) + memory[(*addr)++];
			}
			else {		/* for word */
				A[lcv].word = (memory[*addr] << 8) + memory[*addr+1];
				*addr +=2;
			}
		}
		regs >>= 1;
	}
}


void move_register_to_memory(unsigned short int size,long int addr)
{
	unsigned short int regs,lcv;

	regs = memory[PC+3];        /* lower byte of register mask field */
	for(lcv = 0;lcv < 8;lcv++) {
		if (regs & 0x01) {
			if(size) {    /* for long word */
				memory[addr++] = (D[lcv].dword & 0xff000000) >> 24;
				memory[addr++] = (D[lcv].dword & 0x00ff0000) >> 16;
				memory[addr++] = (D[lcv].word & 0xff00) >> 8;
				memory[addr++] = D[lcv].byte;
			}
			else {         /* for word */
				memory[addr++] = (D[lcv].word & 0xff00) >> 8;
				memory[addr++] = D[lcv].byte;
			}
		}
		regs >>= 1;
	}
	regs = memory[PC+2];          /* higher byte of register mask field */
    for(lcv = 0;lcv < 8;lcv++) {
		if (regs & 0x01) {
			if(size) {		/* for long word */
				memory[addr++] = (A[lcv].dword & 0xff000000) >> 24;
				memory[addr++] = (A[lcv].dword & 0x00ff0000) >> 16;
				memory[addr++] = (A[lcv].word & 0xff00) >> 8;
				memory[addr++] = A[lcv].word & 0x00ff;
			}
			else {         /* for word */
				memory[addr++] = (A[lcv].word & 0xff00) >> 8;
				memory[addr++] = A[lcv].word & 0x00ff;
			}
		}
	regs >>= 1;
	}
}


void move_for_predecrement_mode(unsigned short int size,long int *addr)
{
	unsigned short int regs;
	short int lcv;

	regs = memory[PC+3];        /* lower byte of register mask field */
	for(lcv = 7;lcv >= 0;lcv--) {
		if (regs & 0x01) {
			if(size) {		/* for long word */
				memory[(*addr)--] = (A[lcv].dword & 0xff000000) >> 24;
				memory[(*addr)--] = (A[lcv].dword & 0x00ff0000) >> 16;
				memory[(*addr)--] = (A[lcv].word & 0xff00) >> 8;
				memory[(*addr)--] = A[lcv].word & 0x00ff;
			}
			else {        /* for word */
				memory[(*addr)--] = (A[lcv].word & 0xff00) >> 8;
				memory[(*addr)--] = A[lcv].word & 0x00ff;
			}
		}
	regs >>= 1;
	}
	regs = memory[PC+2];      /* higher byte of register mask field */
	for(lcv = 7;lcv >= 0;lcv--) {
		if (regs & 0x01) {
			if(size) {    /* for long word */
				memory[(*addr)--] = (D[lcv].dword & 0xff000000) >> 24;
				memory[(*addr)--] = (D[lcv].dword & 0x00ff0000) >> 16;
				memory[(*addr)--] = (D[lcv].word & 0xff00) >> 8;
				memory[(*addr)--] = D[lcv].byte;
			}
			else {        /* for word */
				memory[(*addr)--] = (D[lcv].word & 0xff00) >> 8;
				memory[(*addr)--] = D[lcv].byte;
			}
		}
	regs >>= 1;
	}
}


void find_mode_address(unsigned short int low,unsigned short int *mode,long int *addr)
{
	unsigned short int reg_no,regs;
	long int  disp;


    reg_no = low & 0x07;
	switch((low & 0x38) >> 3) {
		case 2:
			*mode = 2;	  /* Address register indirect mode */
			*addr = A[reg_no].dword;
			break;

		case 3 :
			*mode = 3;   /* Address register indirect with post increment */
			*addr = A[reg_no].dword;
			break;

		case 4 :
			*mode = 4;  /* Address register indirect with predecrement */
			if((low & 0x40) >> 6) A[reg_no].dword -= 4;
			else A[reg_no].dword -= 2;
			*addr = A[reg_no].dword;
			break;

		case 5:
			*mode = 5;  /* Address register indirect with displacement */
			disp = memory[PC+4] << 8;
			disp += memory[PC+5];
			word_to_long(&disp);
			*addr = A[reg_no].dword+disp;
			break;

		case 6:
			*mode = 6; /* Address register indirecct with index & displacement */
			low = memory[PC+4];
			regs = (low & 0x70) >> 4;
			disp = memory[PC+5];
			byte_to_long(&disp);
			if (low & 0x80) {
				if (low & 0x08) {
					*addr = A[reg_no].dword + A[regs].dword + disp;
				}
				else {
					*addr = A[reg_no].dword + A[regs].word + disp;
				}
			}
			else {
				if (low & 0x08)
					*addr = A[reg_no].dword + D[regs].dword + disp;
				else
					*addr = A[reg_no].dword + D[regs].word + disp;
			}
			break;

		case 7:
			switch(reg_no) {
				case 0:      /* Absolute short */
					*mode = 7;
					*addr = (memory[PC+4] << 8) + memory[PC+5];
					break;

				case 1:   /* Absolute Long */
					*mode = 8;
					*addr = (memory[PC+4] << 8) + memory[PC+5];
					*addr = (*addr << 8) + memory[PC+6];
					break;

				case 2:
					*mode = 9;  /* Program counter relative with displacement */
					disp = memory[PC+4] << 8;
					disp += memory[PC+5];
                    word_to_long(&disp);
					*addr = PC + disp;
					break;

				case 3:
					*mode = 9;  /* Program counter relative with  index & displacement */
                    low = memory[PC+4];
					regs = (low & 0x7000) >> 12;
					disp = memory[PC+5];
					byte_to_long(&disp);
					if (low & 0x8000) {
						if (low & 0x0800)
							*addr = PC+A[regs].dword+disp;
						else
							*addr = PC+A[regs].word+disp;
					}
					else {
						if (low & 0x0800)
							*addr = PC+D[regs].dword+disp;
						else
							*addr = PC+D[regs].word+disp;
					}
				}
			}
}


void MOVEM( void )
{
	unsigned short int  low,high,size,dr,reg_no,mode;
	long int addr;

	high = memory[PC];
	low = memory[PC+1];
	size = (low & 0x40) >> 6;    /* Operand size is taken */
	dr = (high & 0x04) >> 2;	/* Direction of movement */
	find_mode_address(low,&mode,&addr);
    				/* Find operation mode & effective address */
	if (dr) {    /* memory to register transfer */
	   if(mode != 4) move_memory_to_register(size,&addr);
			  /* predecrement mode allows no memory to register transfer */
	   }
	else {    /* register to memory operation */
		if(mode == 4) move_for_predecrement_mode(size,&addr);
			/* predecrement mode has separate mask correspondence */
		else if(mode != 3) move_register_to_memory(size,addr);
		  /* for postincrement mode register to memory transfer not allowed */
	}
	switch(mode) {
		case 2:
			PC += 4;
			break;

		case 3:
			PC += 4;
			reg_no = low & 0x07;
			if (size) A[reg_no].dword = addr + 3;
			else  A[reg_no].dword = addr + 1;
			break;

		case 4:
			PC += 4;
			A[reg_no].dword = addr--;
			break;

		case 5:
		case 6:
		case 7:
		case 9:
		case 10:
			PC += 6;
			break;

		case 8:
			PC += 8;
			break;
	}
}
























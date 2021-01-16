/*					M68000	Simulator
		This file contains functions to execute six M68000 instructions.
	These  functions were written by  M. Mamunul Islam (rool #13) on the
	date 18/08/94 */

#include "defina_n.h"

void EORI_SR( void )
/*	Name	 : eori_sr.
	Function : This function executes M68000's  "EORI  #d16, SR" instruction.
	Local Varables used : temp .
	Global Variables used : memory, PC, FLAG.
	Macros used : REG8.
	Routines called : None
*/
	{
	REG8 temp;

	temp = memory[PC+3];
	if( (temp & 1) ^ FLAG.c ) FLAG.c = 1;
		else FLAG.c = 0;
	if( ((temp & 0x2) >> 1) ^ FLAG.v ) FLAG.v = 1;
		else FLAG.v = 0;
	if( ((temp & 0x4) >> 2) ^ FLAG.z ) FLAG.z = 1;
		else FLAG.z = 0;
	if( ((temp & 0x8) >> 3) ^ FLAG.n ) FLAG.n = 1;
		else FLAG.n = 0;
	if( ((temp & 0x10) >> 4) ^ FLAG.x ) FLAG.x = 1;
		else FLAG.x = 0;
	FLAG.system = FLAG.system ^ memory[PC+2];
	PC += 4;		/* increament PC */
	}


void swap( REG32 *x, REG32 *y )
/*	Name	 : swap
	Function : This function swaps the contents of two memory location.
	Local Varables used : t
	Global Variables used : None
	Macros used : None
	Routines called : None
*/
	{
	REG32 t;

	t = *x;
	*x = *y;
	*y = t;
	}


void EXG( void )
/*	Name	 : exg
	Function : This function executes M68000's exchange instruction, which is
			   used to exchange the contents of two registers.
	Local Varables used : mode, r_type, x, y
	Global Variables used : memory, PC, A, D
	Macros used : REG8
	Routines called : swap
*/
	{
	REG8 mode, r_type, x, y;

	mode =  memory[PC + 1] & 0x80; /* find if same type of register are to be used */
	/* find register numbers */
	x = (memory[PC] & 0xE) >> 1;
	y = (memory[PC + 1] & 0x7);
	if( mode ) swap( &A[y].dword, &D[x].dword );
	  else
		{
		r_type = memory[PC + 1] & 0x8; /* find data or address register to be used */
		if( r_type ) swap( &A[x].dword, &A[y].dword );
			else swap( &D[x].dword, &D[y].dword );
		}
	PC += 2;		/* increament PC */
	}


REG32 w_to_l( REG16 x )
/*	Name	 : w_to_l
	Function : this functions sign extends a word variable to long word
	Local Varables used : bit
	Global Variables used : None
	Macros used : REG8, REG16, REG32
	Routines called : None
*/
	{
	REG8 bit;

	bit = x & 0x8000;
	if( bit ) return ( 0xFFFF0000 | x );
		else return ( 0x00007FFF & x );
	}


REG16 b_to_w( REG8 x )
/*	Name	 : b_to_w
	Function : this function sign extends a byte value to word value
	Local Varables used : bit
	Global Variables used : None
	Macros used : REG8, REG16
	Routines called : None
*/
	{
	REG8 bit;

	bit = x & 0x80;
	if( bit ) return ( 0xFF00 | x );
		else return ( 0x007F & x );
	}


void EXT( void )
/*	Name	 : ext
	Function : this function excutes M68000's  "EXT   Dn" instruction
	Local Varables used : x
	Global Variables used : memory, PC, D
	Macros used : REG8
	Routines called : w_tol, b_to_w
*/
	{
	REG8 x;

	x = memory[PC + 1] & 0x7;   /* find the type of extension required */
	if( memory[PC + 1] & 0x40 ) D[x].dword = w_to_l( D[x].word );
		else D[x].word =b_to_w( D[x].byte );
	PC += 2;
	}


int get_addr( REG32 *addr, REG8 *d )
/*	Name	 : get_addr
	Function : This function is used to find the effective address of the
			   operands from the instruction.
	Local Varables used : x,y, l_or_w, a_or_d, mode, temp
	Global Variables used : memoru, PC, A, D
	Macros used : REG8, REG16, REG32
	Routines called : w_to_l, b_to_w
*/
	{
	REG8 x, y, l_or_w, a_or_d, mode;
	REG16 temp;

	*d = 2;		/* default size of instruction */
	mode = ( memory[PC+1] & 0x38 ) >> 3;	/* 3 bit of addressing */
	x = memory[PC+1] & 0x7;
	switch( mode )
		{
		case 0: case 1: case 3: case 4: return 0;	/* these modes are not req. */
		case 2: *addr = A[x].dword; return 1;
		case 5: *addr = A[x].dword + (memory[PC+2] << 8) + memory[PC+3]; *d = 4; return 1;
		case 6: y = (memory[PC+2] & 0x70) >> 4;
				l_or_w = memory[PC+2] & 0x08;
				a_or_d = memory[PC+2] & 0x80;
				if( a_or_d )
					{
					if( l_or_w ) *addr = A[y].dword;
						else *addr = w_to_l( A[y].word );
					}
				  else
					{
					if( l_or_w ) *addr = D[y].dword;
						else *addr = w_to_l( D[y].word );
					}
				temp = b_to_w( memory[PC+3] );
				*addr += w_to_l( temp ) + A[x].dword;
				*d = 4;   /* instruction size 4 byte */
				return 1; /* return valid */
		case 7: switch( x )
					{
					case 0: *addr = (memory[PC+2] << 8) + memory[PC+3]; *d = 4; return 1;
					case 1: *addr = (memory[PC+2] << 24) + (memory[PC+3] << 16)
									+ (memory[PC+4] << 8) + memory[PC+5];
							 *d = 6;
							 return 1;
					case 2: *addr = PC + (memory[PC+2] << 8) + memory[PC+3];
							*d = 4;
							return 1;
					case 3: y = (memory[PC+2] & 0x70) >> 4;
							l_or_w = memory[PC+2] & 0x08;
							a_or_d = memory[PC+2] & 0x80;
							if( a_or_d )
								{
								if( l_or_w ) *addr = A[y].dword;
									else *addr = w_to_l( A[y].word );
								}
							  else
								{
								if( l_or_w ) *addr = D[y].dword;
									else *addr = w_to_l( D[y].word );
								}
							temp = b_to_w( memory[PC+3] );
							*addr += w_to_l( temp ) + PC;
							*d = 4;
							return 1;
					case 4: *d = 4; return 0;
					}
		}
	}


void JMP( void )
/*	Name	 : jmp
	Function : this function executes the M68000's jump instruction
	Local Varables used : addr, disp
	Global Variables used : PC
	Macros used : REG32, REG8
	Routines called : get_addr
*/
	{
	REG32 addr;
	REG8 disp;

	if( get_addr( &addr, &disp ) ) PC = addr;
		else PC+= disp;
	}


void LEA( void )
/*	Name	 : lea
	Function : executes the M68000's load effective address instruction
	Local Varables used : addr, disp
	Global Variables used : PC, memory, A
	Macros used : REG32, REG8
	Routines called : get_addr
*/
	{
	REG32 addr;
	REG8  disp;

	if( get_addr( &addr, &disp ) ) A[((memory[PC] & 0xE) >> 1)].dword = addr;
	PC+= disp;
	}
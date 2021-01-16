#include<math.h>
#include "defina_n.h"

extern unsigned long int find_EA (short int mode, short int regno, short int size);

/*-------------------------------------------------------------------------*/
void Branch_On_Carry(int carry)
{
	int index = PC, displacement;

	if ( carry != FLAG.c )
	   PC += (memory[PC + 1] ) ? 2 : 4 ;
	else {
		if ( memory[PC+1])
		   displacement = (signed char)memory[PC+1];
		else
		   {
		   displacement = memory[index+2];
		   displacement <<= 8;
		   displacement |= memory[index+3];
		}
		PC += displacement + 2 ;
	}
}
/*--------------------------------------------------------------------------*/
void Clear_Or_Change_Bit (int Clear)
	{
	int mode, regs_no, bitno, index=PC, shift, immediate;
	long int EA, dst, src;

	PC += 2;
	immediate = !(memory[index] & 1);
	regs_no = memory[index+1] & 7;
	mode = (memory[index+1] >> 3 ) & 7;
	if ( mode == 1 || (mode == 7 && regs_no > 1))
	   return;
	if ( !immediate)
	   bitno = D[memory[index] >> 1].dword;
	else
	   {
	   bitno = memory[index+2];
	   bitno <<= 8;
	   bitno |= memory[index+3];
	   PC += 2;
	   }
	if ( !mode)
	  {
	  dst = D[regs_no].dword;
	  shift = 32;
	  }
	else
	  {
	  EA = find_EA (mode,  regs_no, 1);
	  dst = memory[EA];
	  shift = 8;
	  }
	bitno %= shift;
	shift--;
	src = pow(2, bitno);
	if (src & dst == 0)
	   {
	   FLAG.z = 1;
	   if (!Clear)
	      dst |= src;
	   }
	else
	   {
	   FLAG.z = 0;
	   dst &= !src;
	   }
	if ( !mode)
	   D[regs_no].dword = dst;
	else
	   memory[EA] = dst;
	}
/*-------------------------------------------------------------------------*/
void BCHG()
	{
	 Clear_Or_Change_Bit(0);
	}
/*--------------------------------------------------------------------------*/
void BCLR()
	{
	Clear_Or_Change_Bit(1);
	}
/*--------------------------------------------------------------------------*/
void BCC()
	{
	Branch_On_Carry(0);
	}
/*--------------------------------------------------------------------------*/
void BCS()
	{
	Branch_On_Carry(1);
	}
/*-------------------------------------------------------------------------*/
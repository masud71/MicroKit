/*------------------------------>>>>>>><<<<<<<------------------------------*
*                                                                           *
*                          MC68000 SIMULATOR                                *
*                                                                           *
*---------------------------------------------------------------------------*
*                PROGRAM SEGMENT : ROTATE LEFT & RIGHT                      *
*                    ROLL # 23.  3rd. Year, CSE.                            *
*                        COURSE NO : 310.                                   *
*                        DATE : 05-10-94.                                   *
*                                                                           *
*                                                                           *
*              MODULE TO BE CALLED FROM MAIN :  ROL(), ROR()                *
*              OPCODE PATTERN FOR CALL :  Bits 15-12 : 1110   and           *
*                                         if (Bit  3 is 1 when SS !=3)      *
*                                         else Bits 11-9 : 011              *
*------------------------------>>>>>>><<<<<<<-------------------------------*/

#include <math.h>
#include "defina_n.h"



/*   CONSTANT DEFINATION TO SEPERATE VARIOUS FIELDS FROM OPCODE  */

#define      SRC_MASK     0x0E00
#define      SIZE_MASK    0x00C0
#define      DEST_MASK    0x0007
#define      MODE_MASK    0x0038

extern REG16 find_EA(REG8,REG8,REG8);    /* From A. H. Mamun */

/*---------------------------------------------------------------------------
*    Module Name : save_bits()
*    Called By   : rotate()
*    Function    : Saves the bits that will be shifted out in shift operation
*       	   and reposition them for padding with the number .
*    Global Vars : None .
*    Local  Vars : saved_bits - contains the saved bits that will be returned.
*	     	   bit_wt     - contains the weight of bit positions to save
*			        bits.
----------------------------------------------------------------------------*/

D_REGS save_bits(REG8 rotate_l, REG8 size, REG8 shifts, REG8 dst_reg)
{
     D_REGS  saved_bits,bit_wt;
     int     i;

     saved_bits.dword = 0;
     switch (size)
	{ case 0 :
		  bit_wt.byte = (rotate_l) ?  pow(2,7) : 1 ;
		  for (i=0;i<shifts;i++)
		    { saved_bits.byte += bit_wt.byte;
		      bit_wt.byte = (rotate_l) ? bit_wt.byte/2 : bit_wt.byte*2 ;
		    }
		  saved_bits.byte &=  D[dst_reg].byte ;
		     /* Now saved_bits contains  the saved bits. */

		  /* Reposition  it for padding with the number */
		  saved_bits.byte = (rotate_l)  ?  saved_bits.byte >> 8 - shifts :
						   saved_bits.byte << 8 - shifts ;
		  break ;

	  case 1 :
		 bit_wt.word = (rotate_l) ?  pow(2,15) : 1 ;
		 for (i=0;i<shifts;i++)
		   { saved_bits.word += bit_wt.word;
		     bit_wt.word = (rotate_l) ? bit_wt.word/2 : bit_wt.word*2 ;
		   }
		 saved_bits.word &=  D[dst_reg].word ;
		 saved_bits.word = (rotate_l) ?  saved_bits.word >> 16 - shifts :
					      saved_bits.word << 16 - shifts ;
		 break ;

	  case 2 :
		 bit_wt.dword = (rotate_l) ?  pow(2,31) : 1 ;
		 for (i=0;i<shifts;i++)
		   { saved_bits.dword += bit_wt.dword;
		     bit_wt.dword = (rotate_l) ?  bit_wt.dword/2 : bit_wt.dword*2 ;
		   }
		 saved_bits.dword &=  D[dst_reg].dword ;
		 saved_bits.dword = (rotate_l) ?  saved_bits.dword >> 32 - shifts :
					       saved_bits.dword << 32 - shifts ;
       }
     return saved_bits ;
}


/*---------------------------------------------------------------------------
*   Module Name : rotate()                                                  *
*   Called By   : ROL() , ROR()                                             *
*   Function    : Rotates the specified REG or MEMORY according to the      *
*   	          OPCODE .                                                  *
*                                                                           *
*   Parameter   : rotate_l   - contains 1 for left rotate , otherwise 0 .   *
*   Global Vars : PC , FLAG, memory[] , A[] , D[]                           *
*   Local  Vars :                                                           *
*                 reg_direct - contains 1 if #shifts is in REG, 0 for Imm.  *
*                 shifts     - contains # of shifts                         *
*                 saved_bits - contains the shifted bits for REG or Imm mode*
*                 L_saved_bits, H_saved_bits - same but for Memory operand. *
----------------------------------------------------------------------------*/

void rotate(REG8 rotate_l)
{
      REG8   mode,size,reg_direct,shifts;
      REG8   L_saved_bits,H_saved_bits,src_reg,dst_reg;
      REG16  EA,opcode;
      D_REGS saved_bits ;

	 /* Fetch the opcode  */
      opcode   = memory[PC] + memory[PC+1] * 256 ;

	 /* Extract various fields of the opcode */
      mode     = (opcode & MODE_MASK) >>3 ;
      size     = (opcode & SIZE_MASK) >> 6;
      dst_reg  = (opcode & DEST_MASK) ;
      PC += 2;

      if (size != 3)
       {               /*  If Byte , Word or Long  */
	  src_reg    = (opcode & SRC_MASK) >> 9;
	  reg_direct = (mode == 7) ?  1 : 0 ;

	  switch (size)   /* Now for each type */
	    {  case 0 :
		       shifts = (reg_direct) ? D[src_reg].byte : src_reg ;
		       saved_bits = save_bits(rotate_l,size,shifts,dst_reg);
		       if (rotate_l)
			{  D[dst_reg].byte <<= shifts ;
			   D[dst_reg].byte |= saved_bits.byte ;
			   FLAG.c = (saved_bits.byte & 0x01) ?  1 : 0 ;
			}
		       else
			{  D[dst_reg].byte >>= shifts ;
			   D[dst_reg].byte |= saved_bits.byte ;
			   FLAG.c = (saved_bits.byte & 0x80) ?  1 : 0 ;
			}
		       FLAG.z = (D[dst_reg].byte == 0) ?  1 : 0 ;
		       FLAG.n = (D[dst_reg].byte & 0x80) ? 1 : 0 ;
		       break;

	       case 1 :
		       shifts = (reg_direct) ? D[src_reg].word : src_reg ;
		       saved_bits = save_bits(rotate_l,size,shifts,dst_reg);
		       if (rotate_l)
			{  D[dst_reg].word <<= shifts ;
			   D[dst_reg].word |= saved_bits.word ;
			   FLAG.c = (saved_bits.word & 0x0001) ?  1 : 0 ;
			}
		       else
			{  D[dst_reg].word >>= shifts ;
			   D[dst_reg].word |= saved_bits.word ;
			   FLAG.c = (saved_bits.word & 0x8000) ?  1 : 0 ;
			}
		       FLAG.z = (D[dst_reg].word == 0) ?  1 : 0 ;
		       FLAG.n = (D[dst_reg].word & 0x8000) ? 1 : 0 ;
		       break;

	       case 2 :
		       shifts = (reg_direct) ? D[src_reg].dword : src_reg ;
		       saved_bits = save_bits(rotate_l,size,shifts,dst_reg);
		       if (rotate_l)
			{  D[dst_reg].dword <<= shifts ;
			   D[dst_reg].dword |= saved_bits.dword ;
			   FLAG.c = (saved_bits.dword & 0x00000001) ?  1 : 0 ;
			}
		       else
			{  D[dst_reg].dword >>= shifts ;
			   D[dst_reg].dword |= saved_bits.dword ;
			   FLAG.c = (saved_bits.dword & 0x80000000) ?  1 : 0 ;
			}
		       FLAG.z = (D[dst_reg].dword == 0) ?  1 : 0 ;
		       FLAG.n = (D[dst_reg].dword & 0x80000000) ? 1 : 0 ;
	    }
       }
      else
       {
	   EA = find_EA(mode,dst_reg,1);           /* From  Mamun . */
	   if (rotate_l)
	    {  L_saved_bits = (memory[EA] & 0x80) >> 7 ;
	       H_saved_bits = (memory[EA+1] & 0x80) >> 7 ;
	       memory[EA] <<= 1 ;
	       memory[EA+1] <<= 1 ;
	       FLAG.c = (H_saved_bits & 0x01)  ?  1 : 0 ;
	    }
	   else
	    {  L_saved_bits = (memory[EA] & 0x01) << 7 ;
	       H_saved_bits = (memory[EA+1] & 0x01) << 7 ;
	       memory[EA] >>= 1 ;
	       memory[EA+1] >>= 1 ;
	       FLAG.c = (L_saved_bits & 0x80)  ?  1 : 0 ;
	    }
	   memory[EA]   |=  H_saved_bits ;
	   memory[EA+1] |=  L_saved_bits ;
	   FLAG.z = (memory[EA]==0 & memory[EA+1]==0)  ?  1 : 0 ;
	   FLAG.n = (memory[EA+1] & 0x80) ? 1 : 0 ;
       }
      FLAG.v = 0;
}


void  ROL()
{
      rotate(1);
}

void  ROR()
{
      rotate(0);
}

/**************************************************************************
*			 A PART OF THE	COMPLETE	SIMULATOR OF MOTOROLA MC68000				  *
***************************************************************************
*																								  *
*						NAME : Md. Mohibur Rahman										  *
*						Roll # 24                                               *
*						3rd Year	C.S.E.													  *
*						Session : '91 - '92                                     *
*							  CSE 310														  *
*								101094												        *
***************************************************************************
*		The purpose of this module is to accomplish equivalent  result		  *
*	as	six instructions of  MC68000 , which are:-                          *
*   Instructions    	Condition      		Test	(flags)   CCCC (encoding) *
*	~~~~~~~~~~~	 ~~~~~~~~~~~~~		 	~~~~~~~~~~~~~~~      ~~~~~~~~~~~~~~~  *
*		 1. SLE     ; less or equal      	Z + N^V         1111            *
*		 2. SLS     ; low or same           C + Z           0011            *
*		 3. SLT     ; less than             N ^ V           1101            *
*		 4. SMI     ; minus                   N             1011            *
*		 5. SNE     ; not equal to           !Z             0110            *
*	 &   6. SPL     ; plus                   !N             1010            *
*                                                                           *
*	  If result of the test is true then byte specified by the contents     *
*	of effective address is set to all 1's  otherwise the byte is cleared   *
*	to zero.                                                                *
*		Above six instructions can use all the addressing mode except       *
*	(*) Address register direct.                                            *
*	(*) Relative.                                                           *
*   (*) Immediate.                                                          *
***************************************************************************
**************************************************************************/


#include "defina_n.h"    /*Header file defined by the Co-ordinators*/


REG32 Calc_Effaddr24( );  /*calculates the effective address */


void  SLE(void)    /* type of the REG8 has been declared in defina_n.h */
{
		if( FLAG.z || (FLAG.n ^ FLAG.v) )
				memory[ Calc_Effaddr24(PC) ] = 0xFF; /* [EA] = 1's */
		else   memory[ Calc_Effaddr24(PC)] = 0;    /* [EA] = 0's */
}

void  SLS(void)        /*      void SLS( void )      */
{
		if( FLAG.c || FLAG.z )
				memory[ Calc_Effaddr24()] = 0xFF;
		else   memory[ Calc_Effaddr24()] = 0;
}

void  SLT(void)
{
		if( FLAG.n ^ FLAG.v)
				memory[ Calc_Effaddr24()] = 0xFF;
		else   memory[ Calc_Effaddr24()] = 0;
}

void	SMI(void)
{
		if( FLAG.n )
				memory[ Calc_Effaddr24()] = 0xFF;
		else   memory[ Calc_Effaddr24()] = 0;
}

void	SNE(void)
{
		if ( !FLAG.z )
				memory[ Calc_Effaddr24()] = 0xFF;
		else   memory[ Calc_Effaddr24()] = 0;
}

void	SPL(void)
{
		if ( !FLAG.n )
				memory[ Calc_Effaddr24()] = 0xFF;
		else   memory[ Calc_Effaddr24()] = 0;
}


/************ efffective address calculation ( E.A. )*****************/

REG32 Calc_Effaddr24( )
{

	REG8	mode, reg_no, d_a_reg, x_reg_no, wd_long,disp_8,temp ;
	REG16 disp_16;
	REG32  x_n, e_addr;

	mode = (memory[PC+1] << 2) >> 5;
	reg_no = memory[PC+1] & 0x07;
	switch(mode)
	{
		case 0 :   /*DATA REGISTER DIRECT mode*/

					e_addr = D[reg_no].dword & 0x0FFFFFFF;
								/* Consider effective address as 24 bits */
					PC += 2;
					return e_addr;

		case 2 :  /*ADDRESS REGISTER INDIRECT mode*/

					e_addr = memory[ A[reg_no].dword & 0x0FFFFFFF] ;
					PC += 2;
               return e_addr;

		case 3 :  /*ADDRESS REGISTER INDIRECT WITH POSTINCREMENT mode*/

					e_addr = memory[((A[reg_no].dword)++) & 0x0FFFFFFF];
					PC += 2;
               return e_addr;

		case 4 :  /*ADDRESS REGISTER INDIRECT WITH PREDECREMENT mode*/

					e_addr = memory[(--(A[reg_no].dword)) & 0x0FFFFFFF] ;
					PC += 2;
               return e_addr;

		case 5 :   /*ADDRESS REGISTER INDIRECT WITH DISPLACEMENT mode*/

					disp_16 = (memory[PC+2] << 8)  +  memory[PC+3];
					e_addr = (memory[ A[reg_no].dword ] + disp_16) & 0x0FFFFFFF; ;
					PC += 4;
               return e_addr;

		case 6 :   /*ADDRESS REGISTER INDIRECT WITH INDEX & DISPLACEMENT*/

					temp = memory[PC+2];    /*for calculating index*/
					d_a_reg = temp >> 7;   /*either data or address register*/
					x_reg_no = (temp << 1) >> 5; /*index reg no.*/
					wd_long = (temp << 4) >> 7; /*either word or long word*/
					switch(d_a_reg)     /*** index calculation*****/
					{
						case 0 :     /*if data reg.*/

									if(wd_long)          /*if long word*/
											x_n = D[x_reg_no].dword;
									else
										  x_n = D[x_reg_no].word;
									break;

						case 1 :      /*if address register*/

									if(wd_long)
										x_n = A[x_reg_no].dword;
									else
										  x_n = A[x_reg_no].word;
									break;

						default :
									break;
					}
					e_addr = memory[A[reg_no].dword] + memory[x_n] + memory[PC+3];
							/* E.A. = (add_reg) + (indx_reg) + DISP_8*/
					PC += 4;
               return e_addr;

		case 7 :          /* ABSOLUTE SHORT or ABSOLUTE LONG mode*/
					switch(reg_no)
					{
						case 0 :       /*absolute short*/
							e_addr = (memory[PC+2] << 8) + memory[PC+3];
									 /*contains of next word*/
							PC += 4;
							return e_addr;

						case 1:        /*absolute long*/
							e_addr = (memory[PC+2] << 24) + (memory[PC+3] << 16) +
										 (memory[PC+4]  << 8) + memory[PC+5] ;
										 /*contains of next two words*/
							PC += 6;
							return e_addr;

						default:
									break;

					}
		default : break;
	}
}
/********************  END OF THE MODULE  *******************************/
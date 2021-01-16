# include "defina_n.h"
# include <math.h>

/***********************     START OF ROLL 2  *****************************
	 This function returns the effective address when called with proper
   parameters : air == mode, reg_no == register #, octets == size of the
   instruction(1 for byte, 2 for word, 4 for long; disp_start == # of bytes
   needed to be added with PC to find the displacement, finish == # of
   bytes needed to be added with PC to get after the instruction.
	 TO THE CO-ORDINATOR : No job with this function.    (Roll - 02)
*************************************************************************/

unsigned long path_finder(char air, char reg_no, char octets,
									char disp_strt, char finish)
{
   unsigned long eff_add = 0;
   int bin = 0;
   unsigned int j = 0;
   unsigned char shifter, i;
   char ch = 0;

   if (air == 4)                  /* register indirect with predecrement */
	  A[reg_no].dword -= octets;
   if (air >= 2 && air <= 6)      /*  (An) included modes  */
	  eff_add = A[reg_no].dword;
   if ((air == 5) || (air == 7 && reg_no == 2))  /* PC related modes */
	  {
	  bin = memory[PC + disp_strt +1];
	  eff_add = bin + memory[PC + disp_strt] * 256 + eff_add;
	  if (air != 5)
		 eff_add += PC;
	  }
   if ((air == 6) || (air == 7 && reg_no == 3))  /* with index and */
	  {                                          /* displacement */
	  i = memory[PC + disp_strt + 1];
	  eff_add += i;
	  shifter = memory[PC + disp_strt];
	  j = shifter;
	  if (j > 256)
		 ch = 1;          /* adress  reg, if 0 then data reg */
	  shifter = shifter & 0x78;
	  shifter >>= 3;
	  i = shifter % 2;    /* long if i=1, word if i=0 */
	  shifter >>= 1;
	  bin = reg_no;       /* saving input reg_no to this procedure */
	  reg_no = shifter;
	  if (ch == 1)        /* Xn = address reg */
		 {
		 if (air == 6)       /* EA=(An)+(Xn)+d8 */
			{
			if(i)
			   eff_add += A[reg_no].dword;
			else
			   eff_add += A[reg_no].word;
			}
		 else
			eff_add += PC;     /* EA=(PC)+(Xn)+d8 */
		 }
	  else               /* Xn = data reg */
		 {
         if (air == 6)       /* EA=(An)+(Xn)+d8 */
			{
			if(i)
			   eff_add += D[reg_no].dword;
			else
			   eff_add += D[reg_no].word;
			}
		 else
			eff_add += PC;     /* EA=(PC)+(Xn)+d8 */
		 }
	  }
   switch(air)
	  {
	  case 3: A[bin].dword += octets;
			  break;
	  case 7: if (reg_no == 0)
				 {
				 eff_add = memory[PC + finish] * 256;
				 eff_add += memory[PC + finish + 1];
				 }
			  else if (reg_no == 1)
				 for (i = 0; i < 4; ++i)
					{
					eff_add *= 256;
					eff_add += memory[PC + finish + i];
					}
			  break;
	  }
   return eff_add;
   }
/*-----------------------------------------------------------------------
    This function adjusts the condition code bits after ADDX is invoked.
    Inputs are :
	   x == 1st operand, y == 2nd operand, sz == operand size

    TO THE CO-ORDINATOR : No job with this function     (Roll - 02)
--------------------------------------------------------------------------*/
void adjust_flag( unsigned long x, unsigned long y, char sz)
   {
   char a1, b1;
   int a2, b2;
   unsigned int p;
   long int a3, b3;

   switch(sz)
      {
      case 1: a1 = x, b1 = y;
		 if ((a1 + b1) > 0xFF)    /* if carry for byte operation */
		 FLAG.c = 1;
		 if (a1 >= 0 && b1 >= 0)    /* both operands are positive */
		 {
		 a1 += b1;
		 if (a1 < 0)                /* but the result is negative */
		    FLAG.v = 1;             /* overflow for byte operation */
		 }
		 else if (a1 < 0 && b1 < 0)  /* both operand is negative */
		 {
		 a1 += b1;
		 if (a1 > 0)                 /* but the result is positive */
		    FLAG.v = 1;
		 }
	      a1 += b1;
	      if (a1)
		 {
		 FLAG.z = 0;
		 FLAG.n = a1 >> 7;
		 }
	      break;
	 case 2: a2 = x, b2 = y;          /* for word operation */
		 p = a2 + b2;
		 if (p > 0xFFFF)     /* more than maximum word value */
		 FLAG.c = 1;
	      if (a2 > 0 && b2 > 0)
		 {
		 a2 += b2;
		 if (a2 < 0)
		    FLAG.v = 1;
		 }
	      else if (a2 < 0 && b2 < 0)
		 {
		 a2 += b2;
		 if (a2 > 0)
		    FLAG.v = 1;
		 }
	      a2 += b2;
	      if (a2)
		 {
		 FLAG.z = 0;
		 FLAG.n = a2 >> 15;
		 }
	      break;
	 case 4: a3 = x, b3 = y;             /* for double word operation */
		 if ((a3 + b3) > 0xFFFFFFFF)    /* more than maximum long value */
		 FLAG.c = 1;
		  if (a3 > 0 && b3 > 0)
		 {
		 a3 += b3;
		 if (a3 < 0)
		    FLAG.v = 1;
		 }
		  else if (a3 < 0 && b3 < 0)
		 {
		 a3 += b3;
		 if (a3 > 0)
		    FLAG.v = 1;
		 }
		  a3 += b3;
		  if (a3)
		 {
		 FLAG.z = 0;
		 FLAG.n = a3 >> 15;
		 }
	      break;
      }
   }
/*************************************************************************/
/* This function does the and immediate function to the CCR when c = 0   */
/* to the SR when c = 1                                                  */
/* TO THE CO_ORDINATOR : No job with this function                       */
/*************************************************************************/
void ANDI_to_status(char c)
   {
   unsigned char bdata = memory[PC+3];

   FLAG.c &= (bdata & 0x01);
   FLAG.v &= ((bdata >> 1) & 0x01);
   FLAG.z &= ((bdata >> 2) & 0x01);
   FLAG.n &= ((bdata >> 3) & 0x01);
   FLAG.x &= ((bdata >> 4) & 0x01);
   FLAG.unused &= (bdata >> 5) * 1;
   if ((c == 1) && (FLAG.s == 1))
	  {
	  bdata = memory[PC+2];
	  FLAG.system &= bdata & 0x1F;
	  FLAG.s &= (bdata >> 5) & 0x01;
	  FLAG.rest &= (bdata >> 6) * 1;
	  }
   PC += 4;
   }
/*-----------------------------------------------------------------------
    This function does the following job
		(immediate byte data) AND CCR --> CCR; Condition codes are affected;

    TO THE CO-ORDINATOR : Use the function name to call.      (Roll - 02)
 ------------------------------------------------------------------------*/

void ANDI_CCR()
   {
   ANDI_to_status(0);
   }
/*------------------------------------------------------------------------
    This function does the following job
		(immediate word data) AND SR --> SR; Condition codes are affected;

    TO THE CO-ORDINATOR : Use the function name to call. Please ensure that
			the supervisor bit of the SR (FLAG.s) is declared, because
			that bit is checked here.                     (Roll - 02)
 -------------------------------------------------------------------------*/

void ANDI_SR()
   {
   ANDI_to_status(1);
   }
/*------------------------------------------------------------------------
	 This function helps the AND function when effective address is not
	 in register direct form and when the mode is not '111'.
	 Inputs parameters are e_ad == effective address,bwl == byte, word
	 or double word data(0, 1, 2 when destination is D register; 4, 5,
	 6 when destination is effective address.
	 TO THE CO-ORDINATOR : No job with this function.    (Roll - 02)
 -------------------------------------------------------------------------*/
void helping_AND(unsigned long e_ad, char bwl, char rn)
   {
   unsigned int x = 0;
   unsigned long y = 0;
   unsigned char z = 0;

   x = memory[e_ad];
   x <<= 8;
   x += memory[e_ad + 1];         /* x contains word data from EA */
   y = x;
   y <<= 8;
   y += memory[e_ad + 2];
   y <<= 8;
   y += memory[e_ad + 3];
   y <<= 16;
   y += x;                    /* y contains long data from EA */
   switch(bwl)                /* byte(0,4), word(1,5) or long(2,6) */
	  {
	  case 0: D[rn].byte = D[rn].byte & memory[e_ad];      /* case 0 to */
			  if (D[rn].byte >= 128)
				 FLAG.n = 1;
			  break;
	  case 1: D[rn].word = D[rn].word & x;
			  x = D[rn].word;
			  if (x >> 15)
				 FLAG.n = 1;
			  break;
	  case 2: D[rn].dword = D[rn].dword & y;             /* 2 is for Dn */
			  y = D[rn].dword;                         /* as destination */
			  if (y >> 31)
				 FLAG.n = 1;
			  break;
	  case 4: memory[e_ad] = D[rn].byte & memory[e_ad];  /* the rest is */
			  z = z | memory[e_ad];                    /* for <EA> as */
			  if (memory[e_ad] >= 128)                 /* destination */
				 FLAG.n = 1;
			  break;
	  case 5: x = D[rn].word;
			  memory[e_ad + 1] = D[rn].byte & memory[e_ad + 1];
			  D[rn].word >>= 8;
			  z = z | memory[e_ad + 1];
			  memory[e_ad] = D[rn].byte & memory[e_ad];
			  z = z | memory[e_ad];
			  D[rn].word = x;
			  if (memory[e_ad] >= 128)
				 FLAG.n = 1;
			  break;
	  case 6: y = D[rn].dword;
			  for (x = 3; x >= 0; --x)
				 {
				 memory[e_ad + x] = D[rn].byte & memory[e_ad + x];
				 D[rn].dword >>= 8;
				 z = z | memory[e_ad + x];
				 }
			  D[rn].dword = y;
			  if (memory[e_ad] >= 128)
				 FLAG.n = 1;
			  break;
	  }
   if (bwl < 3 && D[rn].dword == 0)
	  FLAG.z = 1;        /* set zero flag for AND <EA>, Dn */
   else if (bwl > 3)      /* z remains 0 if all the bits of the result */
	  if (!z)            /* is zero  for AND Dn, <EA> */
		 FLAG.z = 1;
   }
/*------------------------------------------------------------------------
    This function does the following job:
		   <EA> AND Dn --> Dn     or
		   <EA> AND Dn --> <EA>.  It takes the help of helping_AND to
    accomplish its job.
    TO THE CO-ORDINATOR : Use the function name for calling.     (Roll - 02)
 -------------------------------------------------------------------------*/
void AND()
   {
   unsigned long z=0;
   unsigned int shifter=0;
   char areg=0, emod=0, opmod=0, dreg=0;
   int x=0;

   FLAG.n = 0;      /* Some flags are forced 0 by this instruction, so */
   FLAG.z = 0;      /* keep all of them 0 and change to 1 only if needed */
   FLAG.v = 0;
   FLAG.c = 0;
   shifter = memory[PC];
   shifter <<= 8;
   shifter += memory[PC + 1];
   areg = shifter % 8;
   emod = (shifter >> 3) % 8;    /* effective mode */
   opmod = (shifter >> 3) % 8;
   dreg = (shifter >> 3) % 8;
   x = (opmod % 4) * 2;       /* x is operand size, needed for PC increment*/
   if (x == 0)
	  x = 1;
   switch(emod)             /* select effective mode */
	  {
	  case 0: if (opmod == 0)
				 D[dreg].byte = D[dreg].byte & D[areg].byte;
			  else if (opmod == 1)
				 D[dreg].word = D[dreg].word & D[areg].word;
			  else if (opmod == 2)
				 D[dreg].dword = D[dreg].dword & D[areg].dword;
			  PC += 2;
			  break;
	  case 2:
	  case 3:
	  case 4:
	  case 5:
	  case 6: z = path_finder(emod, areg, x, 2, 2);
			  helping_AND(z, opmod, dreg);
			  PC += 2;
			  if (emod == 5 || emod == 6)
				 PC += 2;
			  break;
	  case 7: z = path_finder(emod, areg, x, 2, 2);
		  if (areg != 4)
			 helping_AND(z, opmod, dreg);
		  else
		     {
			 if (opmod == 0)
			{
			D[dreg].byte = D[dreg].byte & memory[PC+2];
			if (D[dreg].byte >= 126)
			   FLAG.c = 1;
			PC += 4;
			}
			 else if (opmod == 1 || opmod == 2)
			{
			shifter = memory[PC+2] * 256;
			shifter += memory[PC+3];
			D[dreg].word = D[dreg].word & shifter;
			x = D[dreg].word;
			PC += 4;
			if ((x < 0) && (opmod == 1))
			   FLAG.c = 1;
			}
			 if (opmod == 2)
			{
			z = shifter;
			z <<= 8;
			z += memory[PC + 4];
			z <<= 8;
			z += memory[PC + 5];
			PC += 2;
			D[dreg].dword = D[dreg].dword & z;
			z = D[dreg].dword;
			z >>= 31;
			if (z)
			   FLAG.c = 1;
			}
		  }
		  break;
	  }
   }
/*------------------------------------------------------------------------
    This function does the following job:
		   <EA> AND <immediate data> --> <EA>
    TO THE CO-ORDINATOR : Use the function name for calling.     (Roll - 02)
--------------------------------------------------------------------------*/
void ANDI()
   {
   unsigned char c = 0, areg = 0, em = 0, sz = 0, bdata = 0, finish = 4;
   unsigned int wdata = 0;
   unsigned long ldata = 0, ea = 0, saved = 0;

   FLAG.n = FLAG.z = 0;
   FLAG.v = FLAG.c = 0;
   c = memory[PC + 1];
   areg = c % 8;
   em = (c >> 3) % 8;
   sz = ((c >> 3) % 4) * 2;
   if (!sz)
      sz = 1;
   bdata = memory[PC+3];
   wdata = bdata + memory[PC+2] * 256;
   ldata = wdata;
   ldata <<= 8;
   ldata += memory[PC + 4];
   ldata <<= 8;
   ldata += memory[PC + 5];
   if (sz > 2)
      finish = 6;
   ea = path_finder(em, areg, sz, 2, finish);
   if (em == 0)
      {
      if (sz == 1)
	 {
	 D[areg].byte = D[areg].byte & bdata;
	 if (D[areg].byte >= 128)
	    FLAG.n = 1;
	 }
      else if (sz == 2)
	 {
	 D[areg].word = D[areg].word & wdata;
	 if (D[areg].word >= 32768)
	    FLAG.n = 1;
	 }
      else
         {
	 D[areg].dword = D[areg].dword & ldata;
	 if (D[areg].word >= pow(2, 31))    /* math.h required */
	    FLAG.n = 1;
	 }
      if (D[areg].dword == 0)
	 FLAG.z = 1;
      }
   else
      saved = D[1].dword;
   if (sz == 1)
      D[1].byte = bdata;
   else if (sz == 2)
      D[1].word = wdata;
   else if (sz == 1)
      {
      D[1].dword = ldata;
      --sz;
      }
   helping_AND(ea, sz + 3, 1);
   D[1].dword = saved;
   }

/*************************   END OF  ROLL 02   **************************/



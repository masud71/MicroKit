#include "defina_n.h"

/*-------------------- TYPE DEFINITION -------------------*/
typedef unsigned long int ULI;
typedef union {
	unsigned int word;
	unsigned char byte[2];
} WORD;



/*===================================================================*/
/*               <------Rotate left MEMORY var 1 bit                 */
/*=====================================68000 syntax : ROXL EA =======*/
void roxl_m(REG32 ea)
{
   REG16 word;

   PC=PC+2;					 /*              MEMORY                  */
   word =(REG16)memory[ea]; /*         _______________________      */
   word<<= 8;              /* word == |___[EA]____|___[EA+1]__|    */
	 word|=(REG16) memory[ea+1] ;


	   /* ------------Do the actual rotating--------*/

							 /*              ____________________________    */
   FLAG.c = (word>>15);     /*   FLAG.c <---|_²__________________________|  */
						   /*                         WORD                 */

   word <<=1;    /*shl one bit */
						  /*  ________________________ __            */
   word|= FLAG.x;        /*  |________________________²__|<-- x     */
   FLAG.x = FLAG.c;     /*              WORD                       */


   /*-----------Now save back in memory -------------------*/
	 memory[ea] =(REG8) word >>8;
		 memory[ea+1]=(REG8)word & 0x00FF;

    }

/*=====================================================================*/
/*===============------->Rotate right MEMORY var 1bit <-------========*/
/*=====================================68000 syntax : ROXR EA =======*/
void roxr_m(REG32 ea)
{
   REG16 word;

	PC=PC+2;						 /*              MEMORY                 */
	word =(REG16) memory[ea];      /*         _______________________      */
	word<<= 8;                      /* word == |___[EA]____|___[EA+1]__|    */
	word|=(REG16) memory[ea+1] ;


   /* -----------Do the actual rotating--------*/
								/* ___________________________         */
   FLAG.c = (word & 0x0001);   /* |________________________²__|--> c  */
							  /*                WORD                 */

   word >>=1;    /*shr one bit */


								/*        ________________________       */
   if(FLAG.x)                  /*   x--->|__²_____________________|     */
	   word|=0x8000;           /*                  WORD                */

   FLAG.x = FLAG.c;


   /*-----------Now save back in memory -------------------*/
	 memory[ea]= (REG8) word >> 8;
		 memory[ea+1]=(REG8) word & 0x00FF;

    }
 /*==========================================================*/
 /*======--------->Rotate left register  contents, count in another reg */
 /*=====================================68000 syntax : ROXL Dx,Dy =======*/

  void roxldd_byte(REG8 x_regno,REG8 y_regno)
 {
   REG8 byte,count ;
   register int i;


		byte =D[y_regno].byte;
		count = D[x_regno].dword & 0x0000003F;   /*lower 6 bits give count from 0--63*/
    count %= 9;
    /*--------------------------Do the actual rotating-----*/
    for(i=0;i<=count;i++)
       {
	 FLAG.c=byte>>7;
	 byte<<=1;
	 byte |= FLAG.x;
	 FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
       D[y_regno].byte = byte;
}
/*----------------------subroutine for word size-------------------------*/
 void roxldd_word(REG8 x_regno,REG8 y_regno)
 {
   REG8 count ;
   REG16 wd;
   register int i;


		wd =D[y_regno].word;
		count = D[x_regno].dword & 0x0000003F;   /*lower 6 bits give count from 0--63*/
    count %= 17;
    /*--------------------------Do the actual rotating-----*/
    for(i=0;i<=count;i++)
       {
	 FLAG.c=wd>>15;
	 wd<<=1;
	 wd |= FLAG.x;
	 FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
       D[y_regno].word = wd;
}
/*----------------------subroutine for long_word size-------------------------*/
 void roxldd_lwrd(REG8 x_regno,REG8 y_regno)
 {
   REG8 count ;
   REG32 lw;
   register int i;


		lw =D[y_regno].dword;
		count = D[x_regno].dword & 0x0000003F;   /*lower 6 bits give count from 0--63*/
    count %= 33;
    /*--------------------------Do the actual rotating-----*/
    for(i=0;i<=count;i++)
       {
	 FLAG.c = lw>>31;
	 lw<<=1;
	 lw |= FLAG.x;
	 FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
			 D[y_regno].dword = lw;
}
/************************************************************/
 void roxl_dd(REG8 x_regno,REG8 y_regno,REG8 ss)
 {
   PC=PC +2;
   switch(ss)
    { case  0:   roxldd_byte(x_regno,y_regno);   break;
      case  1:   roxldd_word(x_regno,y_regno);   break;
      case  2:   roxldd_lwrd(x_regno,y_regno);   break;
     }
 }
/***************************************************************/
 /*==========================================================*/
 /*======--------->Rotate right register  contents, count in another reg */
 /*=====================================68000 syntax : ROXR Dx,Dy =======*/
 /*----------------------subroutine for byte size-------------------------*/
 void roxrdd_byte(REG8 x_regno,REG8 y_regno)
 {
   REG8 byte,count ;
   register int i;


		byte =D[y_regno].byte;
		count = D[x_regno].dword & 0x0000003F;   /*lower 6 bits give count from 0--63*/
    count %= 9;
    /*--------------------------Do the actual rotating-----*/
    for(i=0;i<=count;i++)
       {
				 FLAG.c=(byte & 0x01);
				 byte >>=1;
				if (FLAG.x)
					 byte |= 0x80;
				FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
       D[y_regno].byte = byte;
}
/*----------------------subroutine for word size-------------------------*/
 void roxrdd_word(REG8 x_regno,REG8 y_regno)
 {
   REG8 count ;
   REG16 wd;
   register int i;


		wd =D[y_regno].word;
		count = D[x_regno].dword & 0x0000003F;   /*lower 6 bits give count from 0--63*/
    count %= 17;
    /*--------------------------Do the actual rotating-----*/
    for(i=0;i<=count;i++)
       {
	 FLAG.c=(wd & 0x0001);
	 wd >>=1;
	 if (FLAG.x)
	     wd |= 0x8000;
	 FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
       D[y_regno].word = wd;
}
/*----------------------subroutine for long_word size-------------------------*/
 void roxrdd_lwrd(REG8 x_regno,REG8 y_regno)
 {
   REG8 count ;
   REG32 lw;
   register int i;


		lw =D[y_regno].dword;
		count = (REG8)D[x_regno].dword & 0x0000003F;   /*lower 6 bits give count from 0--63*/
    count %= 33;
	 /*--------------------------Do the actual rotating-----*/
		for(i=0;i <= count;i++)
       {
	 FLAG.c=(lw & 0x00000001);
	 lw >>=1;
	 if (FLAG.x)
			 lw |= 0x80000000L;
	 FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
			 D[y_regno].dword = lw;
}

/***********************************************/
/***********************************************************/
 void roxr_dd(REG8 x_regno,REG8 y_regno,REG8 ss)
 {
   PC= PC+2;
   switch(ss)
    { case  0:   roxrdd_byte(x_regno,y_regno);   break;
      case  1:   roxrdd_word(x_regno,y_regno);   break;
      case  2:   roxrdd_lwrd(x_regno,y_regno);   break;
     }
 }
/***********************************************************/


 void roxlim_byte(REG8 imm,REG8 y_regno)
 {
   REG8 byte ;
   register int i;


		byte =D[y_regno].byte;
    /*--------------------------Do the actual rotating-----*/
    for(i=0;i<=imm;i++)
       {
	 FLAG.c=byte>>7;
	 byte<<=1;
	 byte |= FLAG.x;
	 FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
       D[y_regno].byte = byte;
}
/*---------------------subroutine for word size-------------------------*/
 void roxlim_word(REG8 imm,REG8 y_regno)
 {
   REG16 wd;
   register int i;


		wd =D[y_regno].word;
    /*--------------------------Do the actual rotating-----*/
    for(i=0;i<=imm;i++)
       {
	 FLAG.c=wd>>15;
	 wd<<=1;
	 wd |= FLAG.x;
	 FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
       D[y_regno].word = wd;
}
/*----------------------subroutine for long_word size-------------------------*/
 void roxlim_lwrd(REG8 imm,REG8 y_regno)
 {
   REG32 lw;
   register int i;


		lw =D[y_regno].dword;
    /*--------------------------Do the actual rotating-----*/
    for(i=0;i<=imm;i++)
       {
	 FLAG.c = lw>>31;
	 lw<<=1;
	 lw |= FLAG.x;
	 FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
			 D[y_regno].dword = lw;
}
/*-------------------------------------------------------------*/
void roxl_im(REG8 imm,REG8 y_regno,REG8 ss)
 {
   PC= PC+2;
   switch(ss)
    { case  0:   roxlim_byte(imm,y_regno);   break;
      case  1:   roxlim_word(imm,y_regno);   break;
      case  2:   roxlim_lwrd(imm,y_regno);   break;
     }
 }
 /*----------------------subroutine for byte size-------------------------*/

 /*==========================================================*/
 /*  -->Rotate right register  contents, count immediate      */
 /*=====================================68000 syntax : ROXR Dx,Dy =======*/

   void roxrim_byte(REG8 imm,REG8 y_regno)
 {
   REG8 byte ;
   register int i;


		byte =D[y_regno].byte;
    /*--------------------------Do the actual rotating-----*/
    for(i=0;i<=imm;i++)
       {
	 FLAG.c=(byte & 0x01);
	 byte <<=1;
	 if (FLAG.x)
	     byte |= 0x80;
	 FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
       D[y_regno].byte = byte;
}
/*----------------------subroutine for word size-------------------------*/
 void roxrim_word(REG8 imm,REG8 y_regno)
 {
   REG16 wd;
   register int i;


		wd =D[y_regno].word;
    /*--------------------------Do the actual rotating-----*/
    for(i=0;i<=imm;i++)
       {
	 FLAG.c=(wd & 0x0001);
	 wd <<=1;
	 if (FLAG.x)
	     wd |= 0x8000;
	 FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
       D[y_regno].word = wd;
}
/*----------------------subroutine for long_word size-------------------------*/
 void roxrim_lwrd(REG8 imm,REG8 y_regno)
 {
   REG32 lw;
   register int i;


		lw =D[y_regno].dword;
    /*--------------------------Do the actual rotating-----*/
    for(i=0;i<=imm;i++)
       {
	 FLAG.c = (lw & 0x00000001);
	 lw <<=1;
	 if (FLAG.x)
			 lw |= 0x80000000L;
	 FLAG.x = FLAG.c;
       }
    /*--------------------now save back--------*/
			 D[y_regno].dword = lw;
}
 /*****************************************************/
 void roxr_im(REG8 imm,REG8 y_regno,REG8 ss)
 {
   PC=PC+2;
   switch(ss)
    { case  0:   roxrim_byte(imm,y_regno);   break;
      case  1:   roxrim_word(imm,y_regno);   break;
      case  2:   roxrim_lwrd(imm,y_regno);   break;
     }
 }
 /*----------------------subroutine for byte size-------------------------*/


/*------------------------------------------------------------*/
ULI memaddr(REG8 mode,REG8 n)
{
	REG32  ea;
    WORD  data;
    int   longA, Areg, regno;

    switch (mode) {
	   case 2: ea = A[n].dword;
		   break;
	   case 3: ea = A[n].dword;
		   A[n].dword += 2;
		   break;
	   case 4: A[n].dword -= 2;
		   ea = A[n].dword;
		   break;
	   case 5: data.byte[0] = memory[PC++];
		   data.byte[1] = memory[PC++];
		   ea = A[n].dword + data.word;
		   break;
	   case 6: data.byte[0] = memory[PC++];
		   data.byte[1] = memory[PC++];
		   data.byte[0] >>= 3;
		   longA = data.byte[0] & 1;
		   data.byte[0] >>= 1;
		   regno = data.byte[0] & 7;
		   data.byte[0] >>= 3;
		   Areg = data.byte[0] & 1;
		   ea = A[n].dword + data.byte[1];
		   if (Areg)
		      if (longA) ea += A[regno].dword;
		      else ea += A[regno].word;
		   else if (longA) ea += D[regno].dword;
			else ea += D[regno].word;
		   break;
	   case 7: data.byte[0] = memory[PC++];
		   data.byte[1] = memory[PC++];
		   ea = 0;
		   switch (n) {
		     case  0: ea = data.word;
			      break;
		     case  1: ea = data.word;
			      ea <<= 16;
			      data.byte[0] = memory[PC++];
			      data.byte[1] = memory[PC++];
			      ea += data.word;
		   }
    }
    return ea;
}
/*---------------------------------------------------------------------*/
void ROX(void)
{ REG8 op,opn,imm,xno,yno,ss,dir,ee,mod,reg;
  REG32 ea;
  op=memory[PC] ;
  opn=memory[PC+1];

  if( (op>>4)==0x0E)       /* 1110----*/
  {
					/* ---- 010-| 11-- ---- */
	if( (((op & 0x0E)>>1)==2) && ((( opn & 0xC0)>>6)==3)  )
	{
					ee= opn & 0x3F;
					PC+=2;
					 mod = (ee & 0x38)>>3;
					 reg=ee & 0x07;
					ea= memaddr(mod,reg);
					dir=op & 0x01;
					(dir==1) ? roxl_m(ea): roxr_m(ea);
	}
	else
	{
		   switch( (opn & 0x38)>>3)       /* ---- ---- | --?? ?-- */
		   {
				  case 2:               /*    010 => imm  */
						  imm= (op & 0x0E)>>1;
						  dir=op & 0x01;
						  yno= opn & 0x07;
						  ss = (opn & 0xC0)>>6;
						  (dir==1)?	 roxl_im(imm,yno,ss) : roxr_im(imm,yno,ss);
						  break;

				 case 6:               /*    110 => Dx,Dy */
						  xno= (op & 0x0E)>>1;
						  dir=op & 0x01;
						  yno= opn & 0x07;
						  ss = (opn & 0xC0)>>6;
						  (dir==1) ? roxl_dd(xno,yno,ss):roxr_dd(xno,yno,ss);
						  break;
		   } /*switch*/
	} /*else*/
  }/*if*/
}/*****************************************************************/
/**********************************************/
void ROXL(void)
{
	 ROX();
}
/**********************************************/
void ROXR(void)
{
	 ROX();
}
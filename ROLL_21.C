#include "defina_n.h"

void OR(void);
void ORI(void);
void ORI_IMM(void);
void ORI_CCR(void);
void ROL(void);
void ROL_REG(void);
void ROL_IMM(void);
void rst(int,int,REG32);
REG32 Calc_Effaddr21(REG32,int,int);

/*       global  variables   */

REG32 i;
D_REGS result,sd;

/*************************************************************************/
void rst(int md,int type,REG32 ad)
{
   D_REGS d_reg1,d_reg2;
   int k;

   switch(type)
     {
	case 0: if(md==0) result.byte=(sd.byte|(D[(int)(ad)]).byte);
		else result.byte=(sd.byte|(memory[(int)(ad)]));
		break;
	case 64:if(md==0)
		 d_reg1.word=D[(int)(ad)].word;
		else
		 {
		   d_reg1.word=memory[(int)(ad)];
		   d_reg2.word=memory[(int)(ad)+1];
		   d_reg1.word=((d_reg2.word)|(d_reg1.word<<8));
                   if(ad==i) i=i+2;
		 }
		result.word=(d_reg1.word|sd.word);
		break;
	case 128:if(md==0)
		   d_reg1.dword=D[(int)(ad)].dword;
		 else
		   {
		      d_reg1.dword=memory[(int)(ad)];
		      for(k=1;k<4;++k)
		       {
			  d_reg2.dword=memory[(int)(ad)+k];
			  d_reg1.dword=((d_reg2.dword)|(d_reg1.dword<<8));
			}
                      if(ad==i) i=i+4;
		   }
		 result.dword=(d_reg1.dword|sd.dword);
      }
}
/**************************************************************************/
REG32 Calc_Effaddr21(REG32 reg_no,int n,int md)
{
   REG32 add,add1;
   REG8  ind,ind_no,ind_size;
   int j;

   switch(md)
     {
	case 0:return reg_no;
	case 1:return reg_no;
	case 2:return A[(int)(reg_no)].dword;
	case 3:add=A[(int)(reg_no)].dword;
	       A[(int)(reg_no)].dword=A[(int)(reg_no)].dword+n;
	       return add;
	case 4:A[(int)(reg_no)].dword=A[(int)(reg_no)].dword-n;
	       return A[(int)(reg_no)].dword;
	case 5:add=(REG32)(memory[(int)(i)]);
	       add=(add<<8)|memory[(int)(i)+1];
	       i=i+2;
	       return A[(int)(reg_no)].dword+add;
	case 6:ind=memory[(int)(i)];
	       ind=ind>>7;
	       ind_no=(memory[(int)(i)]>>4)&7;
	       ind_size=(memory[(int)(i)]>>3)&1;
	       if(ind==0)
		 {
		    if(ind_size==0)
		       D[ind_no].dword=(D[ind_no].dword<<16)>>16;
		    add=D[ind_no].dword+A[(int)(reg_no)].dword+memory[(int)(i)+1];
		 }
	       else if(ind==1)
		 {
		    if(ind_size==0)
		       A[ind_no].dword=(A[ind_no].dword<<16)>>16;
		    add=A[ind_no].dword+A[(int)(reg_no)].dword+memory[(int)(i)+1];
		 }
	      i=i+2;
	      return add;
	case 7:switch((int)(reg_no))
		 {
		    case 0:add= memory[(int)(i)];
			   add=(add<<8)|memory[(int)(i)+1];
			   i=i+2;
			   return add;
		    case 1: add=memory[(int)(i)];
			    for(j=1;j<4;++j)
			    {
			      add1=memory[(int)(i)+j];
			      add=(add<<8)|add1;
			    }
			   i=i+4;
			   return add;
		    case 2:add=memory[(int)(i)];
			   add=(add<<8)|memory[(int)(i)+1];
			   i=i+2;
			   return PC+add;
		    case 3:ind=memory[(int)(i)];
			   ind=ind>>7;
			   ind_no=(memory[(int)(i)]>>4)&7;
			   ind_size=(memory[(int)(i)]>>3)&1;
			   if(ind==0)
				 {
				   if(ind_size==0)
				       D[ind_no].dword=(D[ind_no].dword<<16)>>16;
				       add=D[ind_no].dword+PC+memory[(int)(i)+1];
				 }
			   else if(ind==1)
			     {
			       if(ind_size==0)
				  A[ind_no].dword=(A[ind_no].dword<<16)>>16;
				  add=A[ind_no].dword+PC+memory[(int)(i)+1];
			      }
			   i=i+2;
			   return add;
		    case 4:add=memory[(int)(i)];
			   i=i+n;
			   return add;
		 }
     }
   return -1;
}
/**************************************************************************/
void OR (void)
  {
     int var,d_no,op_type,mode,j,k;
     REG32 EA,reg_no,rst2;
     REG16 rst1;
     D_REGS d_reg1;
     int n;

     i=PC;
     reg_no=(REG32)(memory[(int)(i+1)]&7);
     op_type=(int)(memory[(int)(i+1)]&192);
     mode=(int)(memory[(int)(i+1)]&56);
     mode=mode>>3;
     var=memory[(int)(i)]&1;
     d_no=memory[(int)(i)]&14;
     d_no=d_no>>1;
     i=i+2;
     if(op_type==0) n=1;
     else if(op_type==64) n=2;
     else n=4;
     if(var==1)
       {
	  if((mode==2)||(mode==3)||(mode==4)||(mode==5)||(mode==6)||((mode==7)&&((reg_no==0)||(reg_no==1))))
	   {
	     EA=Calc_Effaddr21(reg_no,n,mode);
	     switch(op_type)
	     {
	      case 0:sd.byte=D[d_no].byte;
		     rst(mode,0,EA);
		     memory[(int)(EA)]=result.byte;
		     break;
	      case 64:sd.word=D[d_no].word;
		      rst(mode,64,EA);
		      k=1;
		      for(j=0;j<2;++j)
		       {
			  rst1=result.word;
			  d_reg1.byte=(rst1>>j*8);
			  memory[(int)(EA)+k]=d_reg1.byte;
			  --k;
		       }
		      break;
	     case 128:sd.dword=D[d_no].dword;
		      rst(mode,128,EA);
		      k=3;
		      for(j=0;j<4;++j)
		       {
			  rst2=result.dword;
			  d_reg1.byte=(rst2>>j*8);
			  memory[(int)(EA)+k]=d_reg1.byte;
			  --k;
		       }
		      break;
	 }
       }
     }
    else
      {
	 if((mode==0)||(mode==2)||(mode==3)||(mode==4)||(mode==5)||(mode==6)||((mode==7)&&((reg_no==0)||(reg_no==1)||(reg_no==2)||(reg_no==3)||(reg_no==4))))
	   {
	     EA=Calc_Effaddr21(reg_no,n,mode);
	     switch(op_type)
	     {
	      case 0:sd.byte=D[d_no].byte;
		     rst(mode,0,EA);
		     D[d_no].byte=result.byte;
		     break;
	      case 64:sd.word=D[d_no].word;
		      rst(mode,64,EA);
		      D[d_no].word=result.word;
		      break;
	     case 128:sd.dword=D[d_no].dword;
		      rst(mode,128,EA);
		      D[d_no].dword=result.dword;
		      break;
	   }
       }
     }
    PC=i;
 }
 /*************************************************************************/
 void ORI(void)
  {
     if(memory[(int)(i)]==60)
	 ORI_CCR();
     else ORI_IMM();
  }

 /*************************************************************************/
void ORI_IMM(void)
 {
    int op_type,mode,n,j,k;
    REG32 reg_no,EA,rst2;
    REG16 rst1;
    D_REGS imm1,imm2,d_reg1;

    i=PC;
    reg_no=memory[(int)(i)+1]&7;
    op_type=memory[(int)(i)+1]&192;
    mode=memory[(int)(i)+1]&56;
    mode=mode>>3;
    i=i+2;
    if(op_type==0) n=1;
    else if(op_type==64) n=2;
    else n=4;
    if((mode==0)||(mode==2)||(mode==3)||(mode==4)||(mode==5)||(mode==6)||((mode==7)&&((reg_no==0)||(reg_no==1))))
      {
	 EA=Calc_Effaddr21(reg_no,n,mode);
	 switch(op_type)
	    {
	      case 0:sd.byte=memory[(int)(i)];
		     i=i+1;
		     rst(mode,0,EA);
		     if (mode==0)
			 D[(int)(EA)].byte=result.byte;
		     else
			 memory[(int)(EA)]=result.byte;
		     break;
	      case 64:imm1.word=memory[(int)(i)];
		      imm1.word=imm1.word<<8;
		      imm1.word=imm1.word|memory[(int)(i)+1];
		      sd.word=imm1.word;
		      i=i+2;
		      rst(mode,64,EA);
		      if(mode==0)
			D[(int)(EA)].word=result.word;
		      else
			{
			  k=1;
			  for(j=0;j<2;++j)
			   {
			     rst1=result.word;
			     d_reg1.byte=(char)(rst1>>j*8);
			     memory[(int)(EA)+k]=d_reg1.byte;
			     --k;
			   }
			}
		      break;
	     case 128:imm1.dword=memory[(int)(i)];
		       for(j=1;j<4;++j)
			{
			 imm2.dword=memory[(int)(i)+j];
			 imm1.dword=imm1.dword<<8;
			 imm1.dword=imm2.dword|imm1.dword;
		       }
		      sd.dword=imm1.dword;
		      i=i+4;
		      rst(mode,128,EA);
		      if(mode==0)
			D[(int)(EA)].dword=result.dword;
		      else
			{
			      k=3;
			      for(j=0;j<4;++j)
			       {
				  rst2=result.dword;
				  d_reg1.byte=(char)(rst2>>j*8);
				  memory[(int)(EA)+k]=d_reg1.byte;
				  --k;
			       }
			  }
		      break;
	 }
     }
     PC=i;
 }
 /*************************************************************************/
void  ORI_CCR(void)
 {
    REG8 d8;

    i=PC;
    d8=memory[(int)(i)+2];
    FLAG.c=(d8&1)|FLAG.c;
    FLAG.v=((d8&2)>>1)|FLAG.v;
    FLAG.z=((d8&4)>>2)|FLAG.z;
    FLAG.n=((d8&8)>>3)|FLAG.n;
    FLAG.x=((d8&16)>>4)|FLAG.x;
    i=i+3;
    PC=i;
 }
 /*************************************************************************/
/* void ROL(void)
 {
    int var;

    var=memory[(int)(i)];
    var=(var>>3)&7;
    if(var==7)
	ROL_REG();
    else ROL_IMM();

 }  */
 /************************************************************************/
void ROL_REG(void)
 {
    int op_type;
    int d_reg1,src_reg1,var1;
    D_REGS no;

    i=PC;
    op_type=(int)(memory[(int)(i)+1])&192;
    var1=(int)(memory[(int)(i)]);
    src_reg1=(var1>>1)&7;
    d_reg1=(int)(memory[(int)(i)+1])&7;
    i=i+2;
    switch(op_type)
      {
	 case 0: no.byte=(D[d_reg1].byte&128);
		 D[d_reg1].byte=D[d_reg1].byte<<D[src_reg1].byte;
		 if (FLAG.c!=0)
		    D[d_reg1].byte=D[d_reg1].byte|1;
		 if (no.byte!=0)
		    FLAG.c=1;
		 else  FLAG.c=0;
		 break;
	 case 64:no.word=((D[d_reg1].word>>15)&1);
		 D[d_reg1].word=D[d_reg1].word<<D[src_reg1].word;
		 if (FLAG.c!=0)
		    D[d_reg1].word=D[d_reg1].word|1;
		 if (no.word!=0)
		     FLAG.c=1;
		 else  FLAG.c=0;
		 break;
	 case 128:no.dword=((D[d_reg1].dword>>31)&1);
		 D[d_reg1].dword=D[d_reg1].dword<<D[src_reg1].dword;
		 if (FLAG.c!=0)
		    D[d_reg1].dword=D[d_reg1].dword|1;
		 if (no.dword!=0)
		    FLAG.c=1;
		 else  FLAG.c=0;
		  break;
      }
     PC=i;
 }
 /************************************************************************/
 void ROL_IMM(void)
 {
    int var1,q_imm,op_type;
    int dreg1;
    D_REGS no;

    i=PC;
    op_type=(int)(memory[(int)(i)+1])&192;
    var1=(int)(memory[(int)(i)]);
    q_imm=(var1>>1)&7;
    dreg1=(int)(memory[(int)(i)+1])&7;
    i=i+2;
    switch(op_type)
      {
	 case 0:no.byte=(D[dreg1].byte&128);
		 D[dreg1].byte=D[dreg1].byte<<q_imm;
		 if (FLAG.c!=0)
		    D[dreg1].byte=D[dreg1].byte|1;
		 if (no.byte!=0)
		    FLAG.c=1;
		 else  FLAG.c=0;
		 break;
	 case 64:no.word=((D[dreg1].word>>15)&1);
		 D[dreg1].word=D[dreg1].word<<q_imm;
		 if (FLAG.c!=0)
		    D[dreg1].word=D[dreg1].word|1;
		 if (no.word!=0)
		     FLAG.c=1;
		 else  FLAG.c=0;
		 break;
	 case 128:no.dword=((D[dreg1].dword>>31)&1);
		 D[dreg1].dword=D[dreg1].dword<<q_imm;
		 if (FLAG.c!=0)
		    D[dreg1].dword=D[dreg1].dword|1;
		 if (no.dword!=0)
		    FLAG.c=1;
		 else  FLAG.c=0;
		  break;
      }

   PC=i;
 }
 /************************************************************************/

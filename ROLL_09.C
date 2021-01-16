# include "defina_n.h"

/*------------------GLOBAL VARIABLES FOR THIS FILE-----------------------*/
/*  These variables must not be  used as external variables   */

char  op_bit_str[32],data_bit_str[32];
D_REGS a;
D_REGS b;
int EA;

/*-----------------------------FUNCTIONS--------------------------------*/

void CMPI(void);
void CMPM(void);
void CMPA(void);
/*void DBCC(void);
void DBCS(void);
void DBEQ(void);*/

void DB_func(void);
void opcode_conv(void);
int op_mem_conv(int,int,int,int);
void dec_to_bin(int,int,int,int);
D_REGS bin_to_dec(char*,int,int);
int disp_count(int,int,int);
D_REGS  data_cal (int,int );
int EA_cal(int,int,int,int,int,int,int);
void set_flag(int);

/*---------------------------------***----------------------------------*/
void CMPI(void)
 {
  int o,n,i,mod_val,reg_no,dis_no,now_pos,index_no,byte_no;
  int pos,add,index_mod_no,index_size,data_flag;

   opcode_conv();
   n=disp_count(8,2,0);
   if(n==0)
     byte_no=1;
   else
    if(n==1)
     byte_no=2;
    else
      byte_no=4;
    pos=10;
    mod_val=disp_count(10,3,0);
    pos=13;
    reg_no=disp_count(13,3,0);
    add=op_mem_conv(mod_val,reg_no,byte_no,(int)PC+2);
   if(mod_val==6||(mod_val==7 && reg_no==3))
    {
     index_mod_no=op_bit_str[16]-'0';
     if(index_mod_no==0)
       data_flag=1;
      else
	data_flag=0;
      pos=17;
     index_no=disp_count(pos,3,0);
     index_size=op_bit_str[20]-'0';
     dis_no=disp_count(24,8,1);
   }
 if(mod_val==5||(mod_val==7 && reg_no==2))
   dis_no=disp_count(16,16,1);
  now_pos=EA_cal(mod_val,reg_no,byte_no,dis_no,index_no,index_size,data_flag);
  pos=now_pos;
  for(i=0;i<byte_no;++i)
   {
     o=memory[add];
     dec_to_bin(o,pos,0,8);
     ++add;
     pos+=8;
   }
  switch(byte_no)
     {
       case 1:if(mod_val==0)
		b.byte=D[reg_no].byte;
	       else
		if(mod_val==1)
		  b.byte=A[reg_no].word;
	       else
		 if(mod_val==7 && reg_no==4)
		  b.byte=(int)disp_count(16,8,0);
	       else
		  b=data_cal(1,EA);
		a.byte=(int)disp_count(now_pos,8,0);
		break;

       case 2: if(mod_val==0)
		b.word=D[reg_no].word;
	       else
		if(mod_val==1)
		  b.word=A[reg_no].word;
               else
		 if(mod_val==7 && reg_no==4)
		  b.word=disp_count(16,16,0);
	       else
		  b=data_cal(2,EA);
		a.word=disp_count(now_pos,16,0);
		break;
       case 4: if(mod_val==0)
		  b.dword=D[reg_no].dword;
	       else
		 if(mod_val==1)
		   b.dword=A[reg_no].dword;
                else
		 if(mod_val==7 && reg_no==4)
		  b.dword=disp_count(16,32,0);
	       else
		  b=data_cal(4,EA);
		a.dword=disp_count(now_pos,32,0);
		break;
     }

   set_flag(byte_no);
}
/*----------------------------***------------------------------------*/
void CMPM(void)
 {
   int byte_no,first_reg_no,second_reg_no,n;
   unsigned int  A_add,B_add;

   opcode_conv();
   first_reg_no=disp_count(4,3,0);
   second_reg_no=disp_count(13,3,0);
   n=disp_count(8,2,0);
   if(n==2)
    {
     A_add=(int)A[first_reg_no].dword;
     B_add=(int)A[second_reg_no].dword;
   }
   else
     {
      A_add=A[first_reg_no].word;
      B_add=A[second_reg_no].word;
    }
   switch(n)
     {
       case 0: a.byte=memory[A_add];
	       b.byte=memory[B_add];
	       A_add +=1;
	       B_add +=1;
	       byte_no=1;
	       break;
       case 1: a=data_cal(2,A_add);
	       b=data_cal(2,B_add);
	       A_add +=2;
	       B_add +=2;
	       byte_no=2;
	       break;
       case 2: a=data_cal(4,A_add);
	       b=data_cal(4,B_add);
	       A_add +=4;
	       B_add +=4;
	       byte_no=4;
	       break;
	default:break;
     }
     set_flag(byte_no);
  }
/*--------------------------------***-------------------------------------*/
void CMPA(void)
 {
  int n,dis_no,a_reg_no,pos,reg_no,byte_no;
  int index_size,mod_val,data_flag,index_no,index_mod_no;

  opcode_conv();
  n=disp_count(7,3,0);
  if(n==3)
    byte_no=2;
  else
    byte_no=4;
  pos=4;
  a_reg_no=disp_count(pos,3,0);

  pos=10;
  mod_val=disp_count(pos,3,0);

  pos=13;
  reg_no=disp_count(pos,3,0);
  op_mem_conv(mod_val,reg_no,byte_no,(int)PC+2);
  if(mod_val==6||(mod_val==7 && reg_no==3))
    {
     index_mod_no=op_bit_str[16]-'0';
     if(index_mod_no==0)
       data_flag=1;
      else
	data_flag=0;
      pos=17;
     index_no=disp_count(pos,3,0);
     index_size=op_bit_str[20]-'0';
     dis_no=disp_count(24,8,1);
   }
 if(mod_val==5||(mod_val==7 && reg_no==2))
   dis_no=disp_count(16,16,1);
  EA_cal(mod_val,reg_no,byte_no,dis_no,index_no,index_size,data_flag);
  switch(byte_no)
     {
       case 2: if(mod_val==0)
		b.word=D[reg_no].word;
	       else
		if(mod_val==1)
		  b.word=A[reg_no].word;
               else
		 if(mod_val==7 && reg_no==4)
		  b.word=disp_count(16,16,0);
	       else
		  b=data_cal(2,EA);
		a.word=A[a_reg_no].word;
		break;
       case 4: if(mod_val==0)
		  b.dword=D[reg_no].dword;
	       else
		 if(mod_val==1)
		   b.dword=A[reg_no].dword;
               else
		 if(mod_val==7 && reg_no==4)
		  b.dword=disp_count(16,32,0);
	       else
		  b=data_cal(4,EA);
		a.dword=A[a_reg_no].dword;
		break;
     }
  set_flag(byte_no);
}
/*----------------------------------***----------------------------------*/
			/******/

/*void DBCC(void)
 {
	 DB_func();
	}
/*----------------------------------***----------------------------------*/
void DBCS(void)
 {
	 DB_func();
	}
/*---------------------------------***------------------------------------*/
void DBEQ(void)
 {
	 DB_func();
	}
/*---------------------------------***------------------------------------*/
void DB_func(void)
 {
   int ch,val,cond_ok=0,db_reg_no,dis_no;
   int n;

   opcode_conv();
   n=(int)PC+2;
   val=memory[n];
   dec_to_bin(val,16,0,8);
   val=memory[++n];
   dec_to_bin(val,24,0,8);
   ch=bin_to_dec(op_bit_str,4,4).word;
   switch(ch)
    {
     case 4:if (FLAG.c==0)
	       cond_ok=1;
	       break;
     case 5:if (FLAG.c==1)
	       cond_ok=1;
	       break;
     case 7:if (FLAG.z==1)
	       cond_ok=1;
	       break;
     }
   db_reg_no=bin_to_dec(op_bit_str,13,3).word;
   dis_no=disp_count(16,16,1);
   if(cond_ok)
    PC=PC+2;
   else
     {
       D[db_reg_no].word -=1;
       if(D[db_reg_no].word >=0)
	 PC=PC+dis_no;
       else
	 PC=PC+2;
     }
}       */
						/*************************/
/*--------------------------------***-------------------------------------*/
void opcode_conv(void)
{
  int i,val,pos;
  int n;

  n=(int)PC;
  for(i=0,pos=0;i<2;++i)
    {
      val=memory[n];
      dec_to_bin(val,pos,0,8);
      ++n;
      pos+=8;
   }
  op_bit_str[16]='\0';
}
/*--------------------------------***-------------------------------------*/
int op_mem_conv(int mod_val,int reg_no,int size,int add)
{
  int i,val,pos,byte_no;

  if(mod_val==5 || mod_val==6 || mod_val==7 )
     {
       if( mod_val==7 )
	{
	if(reg_no==4)
	 byte_no=size;
	else
	 if(reg_no==1)
	  byte_no=4;
	else
	  byte_no=2;
	}
       else
	 byte_no=2;
      }
  else
     byte_no=0;
  pos=16;
  for(i=0;i<byte_no;++i)
    {
      val=memory[add+i];
      dec_to_bin(val,pos,0,8);
      pos+=8;
   }

  return (add+byte_no);
}
/*--------------------------------***-------------------------------------*/
void dec_to_bin(int val,int pos,int data_flag,int length)
 {
    int i,len,max;
    char temp_str[16];

    i=0;
    while(val/2)
     {
       temp_str[i]=(val%2)+'0';
       val/=2;
       ++i;
     }
    temp_str[i]=val+'0';
    len=i;
    max=length-1;
    for(i=0;i<length;++i)
    {
      if(!data_flag)
       {
	if(len<max)
	 op_bit_str[pos]='0';
      else
	op_bit_str[pos]=temp_str[max];
       }
      else
	{
          if(len<max)
	    data_bit_str[pos]='0';
	 else
	   data_bit_str[pos]=temp_str[max];
	 }
       max--;
       pos++;

     }
 }
/*--------------------------------***-------------------------------------*/
D_REGS bin_to_dec(char *temp_bit_str,int pos,int len)
  {
    int i,temp_pos,factor=1;
    D_REGS val;

    val.byte=0;
    val.word=0;
    val.dword=0;
    temp_pos=pos+len;
    for(i=temp_pos-1;i>=pos;i--)
     {
      if(len<=16)
	val.word+=(temp_bit_str[i]-'0')*factor;
       else
	 if(len>16)
	val.dword+=(temp_bit_str[i]-'0')*factor;
      factor*=2;
     }
   return val;
 }
/*--------------------------------***-------------------------------------*/
int disp_count(int pos,int len,int flag)
 {
  int dis_no,i;
  long int factor=2;

  dis_no=bin_to_dec(op_bit_str,pos,len).word;
  if(flag)
    {
      if(op_bit_str[pos]=='1')
	{
	   for(i=0;i<len;++i)
	     factor *=2;
	   dis_no=(int)(factor-dis_no)*(-1);
	}
     }
  return dis_no;
}
/*---------------------------------***------------------------------------*/
D_REGS  data_cal (int byte_no, int add)
{
  int i,val,pos;
  D_REGS data;

  pos=0;
  for(i=add;i<(add+byte_no);++i)
    {
      val=memory[i];
      dec_to_bin(val,pos,1,8);
      pos +=8;
     }
  switch(byte_no)
   {
    case 1:
	   data.byte=bin_to_dec(data_bit_str,0,8).byte;
	   break;
    case 2:
	   data.word=bin_to_dec(data_bit_str,0,16).word;
	   break;
    case 4:
	    data.dword=bin_to_dec(data_bit_str,0,32).dword;
	    break;
    default:break;
   }
return data;
}
/*-----------------------------***----------------------------------------*/
void set_flag(int byte_no)
 {
  D_REGS temp;

  switch(byte_no)
   {
     case 1:temp.word=a.byte-b.byte;
	    if(a.byte==b.byte)
	     FLAG.z=1;
	    if(a.byte<b.byte)
	      {
	       FLAG.n=1;
	       FLAG.c=1;
	      }
	    if(temp.word>255)
	      FLAG.v=1;
	    break;
      case 2:temp.dword=a.word-b.word;
	    if(a.word==b.word)
	     FLAG.z=1;
	    if(a.word<b.word)
	      {
	       FLAG.n=1;
	       FLAG.c=1;
	      }
	    if(temp.dword>32767)  /* Here condition (temp.dword >32768)    */
	      FLAG.v=1;           /* shows warning.So,it is checked against*/
	    break;                /* this value.                           */
     case 4:temp.dword=a.dword-b.dword;
	    if(a.dword==b.dword)
	     FLAG.z=1;
	    if(a.dword<b.dword)
	      {
	       FLAG.n=1;
	       FLAG.c=1;
	      }
	 /*   if(temp.dword>2147483648)   Here as 32 bitdata is used,*/
	 /*	FLAG.v=1;		  overflow  can't be checked */

	    break;
	}
  }
/*-----------------------------***----------------------------------------*/
int EA_cal(int mod_val,int reg_no,int byte_no,int dis_no,int index_no,int index_size,int data_flag)
 {
  int bit_pos;

  bit_pos=16;
  switch(mod_val)
   {
    case 0:
    case 1:EA=reg_no;
	    break;

    case 2:EA=(int)A[reg_no].dword;
	   break;
    case 3:EA=(int)A[reg_no].dword;
	   A[reg_no].dword +=byte_no;
	   break;
    case 4:A[reg_no].dword -=byte_no;
	   EA=(int)A[reg_no].dword;
	   break;
    case 5:EA=(int)A[reg_no].dword+dis_no;
	   break;
    case 6:
	   switch(index_size)
	    {
	     case 0:
		    if(data_flag)
		      EA=(int)A[reg_no].dword+D[index_no].word+dis_no;
		    else
		      EA=(int)A[reg_no].dword+A[index_no].word+dis_no;
		    break;
	     case 1:
                    if(data_flag)
		     EA=(int)(A[reg_no].dword+D[index_no].dword+dis_no);
		    else
		      EA=(int)(A[reg_no].dword+A[index_no].dword+dis_no);
		    break;
	   }
	    bit_pos=32;
	    break;
       case 7:
	      switch(reg_no)
		{
		 case 0:EA=disp_count(16,16,0);
			 bit_pos=32;
			 break;
		 case 1: EA=disp_count(16,32,0);
			 bit_pos=48;
			 break;
		 case 2: EA=(int)(PC+dis_no);
			 bit_pos=32;
			 break;
		 case 3: if(data_flag)
			  {
			   if(index_size==0)
			     EA=(int)(PC+D[index_no].word+dis_no);
			    else
			     EA=(int)(PC+D[index_no].dword+dis_no);
			   }
			 else
			   {
			   if(index_size==0)
			     EA=(int)(PC+A[index_no].word+dis_no);
			   else
			     EA=(int)(PC+A[index_no].dword+dis_no);
			    }
			 bit_pos=32;
			 break;
		 case 4: break;
	       }
	      break;

     }
     return bit_pos;
  }
/*--------------------------------***-------------------------------------*/

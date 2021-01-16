
/*-------------------------------------------------------------------------*/
/*                             MC68000 SIMULATOR                           */
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*			     MD. MASUD HASAN KHAN                          */
/*                          3RD YEAR CSE,ROLL - 27                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           :COMMANDS:                                    */
/*                                                                         */
/*                   ST,STOP,SUB,SUBA,SUBI,SUBQ                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/


	 #include "defina_n.h"
/*-----------------------------FUNCTIONS----------------------------------*/

     long int eff_add(short int Ea,short int size);
     void carry_set(short int size,long int no1,long int no2);
     void flag_set(short int size,long int no1,long int no2,long int result);
     void put_result(short int size,short int Ea,long int eff_ad,long int result);
     void ST();
     void STOP();
     void SUB();
     void SUBA();
     void SUBQ();
     void SUBI();

 /*------------------------GLOBAL VARIABLES--------------------------------*/

     short int data=0,source8=0,dest8=0;
     int source16=0,dest16=0;
     long int source32=0,dest32=0;


/**************************************************************************/

/*--------------------CALCULATE EFFECTIVE ADDRESS-------------------------*/


    long int eff_add(short int EFF_A,short int size1)
    {
      short int i;
      long int disp =0;
      switch(EFF_A>>3)
      {
      case 0: data=1;
	      if (size1 ==0) return(D[(EFF_A<<5)>>5].byte);
	      else if (size1==1) return(D[(EFF_A<<5)>>5].word);
	      else  return(D[(EFF_A<<5)>>5].dword);
      case 1: data=1;
	      if (size1 ==0) return 0;
	      else if (size1==1) return(A[(EFF_A<<5)>>5].word);
	      else  return(A[(EFF_A<<5)>>5].dword);
      case 2:
	      return(A[(EFF_A<<5)>>5].dword);
      case 3:
		  if (size1 == 0) i=1;
	      else if (size1==1) i=2;
	      else i=4;
	      A[(EFF_A<<5)>>5].dword = A[(EFF_A<<5)>>5].dword +i;
	      return(A[(EFF_A<<5)>>5].dword-i);
       case 4:
	      if (size1 == 0) i=1;
	      else if (size1==1) i=2;
	      else i=4;
	      A[(EFF_A<<5)>>5].dword = A[(EFF_A<<5)>>5].dword -i;
	      return(A[(EFF_A<<5)>>5].dword);
       case 5:  disp=memory[PC+3];
		disp=(disp<<8)+memory[PC+2];
		return(A[(EFF_A<<5)>>5].dword+disp);
       case 6: break;
       case 7:switch((EFF_A<<5)>>5)
	      {
	      case 0: disp=memory[PC+3];
		      disp=(disp<<8)+memory[PC+2];
		      return(disp);
	       case 1: for(i=5;i>=2;--i)
		      {
			disp=disp<<8;
			disp=disp+memory[PC+i];
		      }
		      return(disp);
	       case 2: disp=memory[PC+3];
		       disp=(disp<<8)+memory[PC+2];
		       return(PC+disp);
	       case 3: break;
	       case 4: disp=memory[PC+3];
		       disp=(disp<<8)+memory[PC+2];
		       data=1;
		       return(disp);
	      }
	      break;
	}
      }

  /***********************************************************************/

 /*-------------------------DETERMINE CARRY-------------------------------*/

      void set_carry(short int size,long int num1,long int num2)
      {
	short int i,j,check=0,op1,op2,cary=0,tot=0;
	if (size==0) j=7;
	else if (size==1) j=15;
	else j=31;
	for(i=0;i<=j;++i)
	 {
	   op1=(num1 <<(31-i))>>31;
	   op2=(num2 <<(31-i))>>31;
	   if (check==0) {
			  if (op1==1) ++tot;
			  if (op2==1) ++tot;
			  if (op1 && op2) check=1;
			  }
		  else {
                        if (op1==1) ++tot;
			if (op2==1) ++tot;
			if (cary==1) ++tot;
			}
	    if (tot>1) cary=1;
	    else cary=0;
	    tot=0;
	   }
	   if (cary) FLAG.c=1;
	}

  /***********************************************************************/

 /*-----------------------AFFECTED FLAG SETTINGS-------------------------*/

       void  flag_set(short int siz,long int no1,long int no2,long int res)
       { short int j;

	if (res==0) FLAG.z=1;
	else if (res<0) FLAG.n=1;
	set_carry(siz,no1,no2);
	FLAG.x=FLAG.c;
	if (siz==0) j=7;
	else if (siz==1) j=15;
	else j=31;
	FLAG.v=FLAG.c ^ (( res <<(31-j))>>31);
       }

  /************************************************************************/

 /*-----------------PUT RESULT IN THE EFFECTIVE ADDRESS-------------------*/

      void put_result(short int size,short int eff_ad,long int ef_add,long int res)
      {
       short int i;
       if(data)
	{
	  switch(eff_ad>>3)
	  {
	  case 0: if (size==0) D[(eff_ad<<5)>>5].byte=(res<<24)>>24;
		  else if (size==1) D[(eff_ad<<5)>>5].word=(res<<16)>>16;
		  else D[(eff_ad<<5)>>5].dword=res;
		  break;
	  case 1: if (size==0) ;
		  else if (size==1) A[(eff_ad<<5)>>5].word=(res<<16)>>16;
		  else A[(eff_ad<<5)>>5].dword=res;
		  break;
	  case 7: if (((eff_ad<<5)>>5)==4)
		   {
		     switch(size)
		     {
		     case 0:memory[PC+2]=(res<<24)>>24;
			    break;
		     case 1:memory[PC+2]=(res<<24)>>24;
			    memory[PC+3]=(res<<16)>>24;
			    break;
		     case 2:for(i=0;i<=3;++i)
			     memory[PC+2+i]=(res<<((3-i)*8))>>24;
			    break;
		      }
		    }
		   break;
	   }
	 }
	 else{
		  switch(size)
		     {
		     case 0:memory[ef_add]=(res<<24)>>24;
			    break;
		     case 1:memory[ef_add]=(res<<24)>>24;
			    memory[ef_add+1]=(res<<16)>>24;
			    break;
		     case 2:for(i=0;i<=3;++i)
			     memory[ef_add+i]=(res<<((3-i)*8))>>24;
			    break;
		      }
	     }
       }


  /************************************************************************/

       void SUB()
       {
	 short int op_size,EA,code1,code2,i;
	 long int ef_ad,result,Ts_com;
	 code1=memory[PC];
	 code2=memory[PC+1];
	 if (((code1>>4)==9)&&((code2>>6)!=3))
	  {
	    op_size=code2>>6;
	    EA=(code2<<2)>>2;
	    ef_ad=eff_add(EA,op_size);
	    if (((code1<<7)>>7)==1)
	     {
	       switch(op_size)
	       {
	       case 0:if (data)  dest8=(ef_ad<<24)>>24;
		      else dest8=memory[ef_ad];
		      source8=D[(code1<<4)>>5].byte;
		      result=source8-dest8;
		      put_result(op_size,EA,ef_ad,result);
		      Ts_com=~dest8+1;
		      flag_set(op_size,source8,Ts_com,result);
		      break;
	       case 1:if (data)  dest16=(ef_ad<<16)>>16;
		      else {
			   dest16=memory[ef_ad+1];
			   dest16=(dest16<<8)+memory[ef_ad];
			   }
		      source16=D[(code1<<4)>>5].word;
		      result=source16-dest16;
		      put_result(op_size,EA,ef_ad,result);
		      Ts_com=~dest16+1;
		      flag_set(op_size,source16,Ts_com,result);
		      break;
	       case 2:if (data)  dest32=ef_ad;
		      else {
			    for(i=3;i>=0;--i)
			    {
			     dest32=dest32<<8;
			     dest32=dest32+memory[ef_ad+i];
			    }
			   }
		      source32=D[(code1<<4)>>5].dword;
		      result=source32-dest32;
		      put_result(op_size,EA,ef_ad,result);
                      Ts_com=~dest32+1;
		      flag_set(op_size,source32,Ts_com,result);
		      break;
	      }

	    }
	    else {
		switch(op_size)
	       {
	       case 0:if (data)  source8=(ef_ad<<24)>>24;
		      else source8=memory[ef_ad];
		      dest8=D[(code1<<4)>>5].byte;
		      result=source8-dest8;
		      D[(code1<<4)>>5].byte=(result<<24)>>24;
		      Ts_com=~dest8+1;
		      flag_set(op_size,source8,Ts_com,result);
		      break;
	       case 1:if (data)  source16=(ef_ad<<16)>>16;
		      else {
			   source16=memory[ef_ad+1];
			   source16=(source16<<8)+memory[ef_ad];
			   }
		      dest16=D[(code1<<4)>>5].word;
		      result=source16-dest16;
		      D[(code1<<4)>>5].word=(result<<16)>>16;
		      Ts_com=~dest16+1;
		      flag_set(op_size,source16,Ts_com,result);
		      break;
	       case 2:if (data)  source32=ef_ad;
		      else {
			    for(i=3;i>=0;--i)
			    {
			     source32=source32<<8;
			     source32=source32+memory[ef_ad+i];
			    }
			   }
		      dest32=D[(code1<<4)>>5].dword;
		      result=source32-dest32;
		      D[(code1<<4)>>5].dword=result;
		      Ts_com=~dest32+1;
		      flag_set(op_size,source32,Ts_com,result);
		      break;
	      }
	    }
	   }
	   PC+=2;
	  }

  /**********************************************************************/

	  void SUBA()
	  {
	   short int op_size,EA,code1,code2,i;
	   long int result,ef_ad;
           code1=memory[PC];
	   code2=memory[PC+1];
	   if (((code1>>4)==9)&&((code2>>6)==3))
	   {
	    op_size=(code1 <<7)>>7;
	    EA=(code2<<2)>>2;
	    ++op_size;
	    ef_ad=eff_add(EA,op_size);
	    --op_size;
	    switch(op_size)
	    {
	    case 0: dest16=A[(code1<<4)>>5].word;
                    if (data)  source16=(ef_ad<<16)>>16;
		      else {
			   source16=memory[ef_ad+1];
			   source16=(source16<<8)+memory[ef_ad];
			   }
		     result=dest16-source16;
		     A[(code1<<4)>>5].word=(result<<16)>>16;
		     break;
	     case 1: dest32=A[(code1<<4)>>5].dword;
                     if (data)  source32=ef_ad;
		      else {
			    for(i=3;i>=0;--i)
			    {
			     source32=source32<<8;
			     source32=source32+memory[ef_ad+i];
			    }
			   }
		     result=dest32-source32;
		     A[(code1<<4)>>5].dword=result;
		     break;
	      }
	    }
		PC+=2;
	  }

  /**********************************************************************/

	 void SUBQ()
	 {
          short int op_size,EA,code1,code2,i;
	  long int result,ef_ad,Ts_com;
           code1=memory[PC];
	   code2=memory[PC+1];
	   if (((code1>>4)==5)&&(((code1 <<7)>>7)==1))
	   {
	    op_size=code2 >>6;
	    EA=(code2<<2)>>2;
	    ef_ad=eff_add(EA,op_size);
	    source8=(code1<<4)>>5;
	    switch(op_size)
	    {
	    case 0: if (data)  dest8=(ef_ad<<24)>>24;
		    else dest8=memory[ef_ad];
		    result= dest8-source8;
		    put_result(op_size,EA,ef_ad,result);
		    Ts_com=~source8+1;
		    flag_set(op_size,dest8,Ts_com,result);
		    break;
	       case 1:if (data)  dest16=(ef_ad<<16)>>16;
		      else {
			   dest16=memory[ef_ad+1];
			   dest16=(dest16<<8)+memory[ef_ad];
			   }
		      source16=source8;
		      result=dest16-source16;
		      put_result(op_size,EA,ef_ad,result);
		      Ts_com=~source16+1;
		      flag_set(op_size,dest16,Ts_com,result);
		      break;
	       case 2:if (data)  dest32=ef_ad;
		      else {
			    for(i=3;i>=0;--i)
			    {
			     dest32=dest32<<8;
			     dest32=dest32+memory[ef_ad+i];
			    }
			   }
		      source32=source8;
		      result=dest32-source32;
		      put_result(op_size,EA,ef_ad,result);
		      Ts_com=~source32+1;
		      flag_set(op_size,dest32,Ts_com,result);
		      break;
	      }
	   }
	   PC+=2;
	 }

   /***********************************************************************/

       void SUBI()
       {
        short int op_size,EA,code1,code2,i;
	 long int ef_ad,result,Ts_com;
	 code1=memory[PC];
	 code2=memory[PC+1];
	 if (((code1>>4)==0)&&(((code1<<4)>>4)==4))
	  {
	    op_size=code2>>6;
	    EA=(code2<<2)>>2;
	    ef_ad=eff_add(EA,op_size);
            switch(op_size)
	       {
	       case 0:if (data)  dest8=(ef_ad<<24)>>24;
		      else dest8=memory[ef_ad];
		      source8=memory[PC+2];
		      result=dest8-source8;
		      put_result(op_size,EA,ef_ad,result);
		      Ts_com=~source8+1;
		      flag_set(op_size,dest8,Ts_com,result);
		      break;
	       case 1:if (data)  dest16=(ef_ad<<16)>>16;
		      else {
			   dest16=memory[ef_ad+1];
			   dest16=(dest16<<8)+memory[ef_ad];
			   }
		      source16=memory[PC+3];
		      source16=(source16<<8)+memory[PC+2];
		      result=dest16-source16;
		      put_result(op_size,EA,ef_ad,result);
		      Ts_com=~source16+1;
		      flag_set(op_size,dest16,Ts_com,result);
		      break;
	       case 2:if (data)  dest32=ef_ad;
		      else {
			    for(i=3;i>=0;--i)
			    {
			     dest32=dest32<<8;
			     dest32=dest32+memory[ef_ad+i];
			    }
			   }
		      for(i=5;i>=2;--i)
			    {
			     source32=source32<<8;
			     source32=source32+memory[ef_ad+i];
			    }
		      result=dest32-source32;
		      put_result(op_size,EA,ef_ad,result);
		      Ts_com=~source32+1;
		      flag_set(op_size,dest32,Ts_com,result);
		      break;
	      }
	    }
		PC+=4;
	  }

  /************************************************************************/


       void STOP()
       {
	short int code1,code2;
	code1=memory[PC];
	code2=memory[PC+1];
	if ((code1==0x4E)&&(code2==0x72)&&(((FLAG.system & 0x20)>>5)==1))
	  {
	   FLAG.system=memory[PC+3];
	   FLAG.c=(memory[PC+2]<<7)>>7;
	   FLAG.v=(memory[PC+2]<<6)>>7;
	   FLAG.z=(memory[PC+2]<<5)>>7;
	   FLAG.n=(memory[PC+2]<<4)>>7;
	   FLAG.x=(memory[PC+2]<<3)>>7;
	   FLAG.unused=memory[PC+2]>>5;
	   for(;;)
	    {
	     if (kbhit()) break;
	    }
	 }
	 else
	  {
	   /*TRAP*/
	  }
	  PC+=2;
	  }

 /*************************************************************************/

      void ST()
      {
       short int code1,code2,EA;
       long int ef_ad;
       code1=memory[PC];
       code2=memory[PC+2];
       if (((code1>>4)==5)&&((code2>>6)==3))
       {
	EA=(code2<<2)>>2;
	ef_ad=eff_add(EA,0);
	if (data!=1) memory[ef_ad]=0xFF;
	else put_result(0,EA,ef_ad,0xFF);
       }
	   PC+=2;
	  }
 /*************************************************************************/
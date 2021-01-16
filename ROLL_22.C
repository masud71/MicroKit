#include "defina_n.h"


 unsigned long int eff_addr() ;

/*************  Function  Declaration  **********************************/
 void  SBCD();
 void  SCC();
 void  SCS();
 void  SEQ();
 void  SF();
 void  SGE();
 void  SGT();
 void  SHI();

/***************End of defination ***********************/
 unsigned  long int eff_addr(void)
{
   unsigned int  pattern , temp, reg_no ,mode ,pc_rla,disp16,temp3,temp4 ;
   unsigned int   in_r ,by_te , r_no , disp8 , abs16 ;
   unsigned long int  E_addr , abs32  ;

   PC += 1;
   pattern = memory[PC] ;
   temp = pattern;
   mode = temp & 0x38;
   mode >>= 3;
   temp = pattern;
   if ( mode == 7)
	   pc_rla = temp & 0x07;
   else
	  reg_no = temp & 0x07;

   switch(mode){

		case 0: E_addr = reg_no ;
				PC += 1;
				break;
		case 1: E_addr = reg_no ;
				PC += 1;
				break;
		case 2: E_addr =  A[reg_no].dword ;
				PC += 1;
				break;
		case 3: E_addr =  A[reg_no].dword;
				A[reg_no].dword += 1;
				PC += 1 ;
				break;
		case 4: A[reg_no].dword -= 1;
				E_addr = A[reg_no].dword ;
				PC += 1;
				break ;
		case 5: PC += 1;
				disp16 = memory[PC] ;
				disp16 <<= 8;
				PC += 1;
				disp16 += memory[PC];
				E_addr = A[reg_no].dword + disp16;
				PC += 1;
				break ;
		case 6: PC += 1 ;
				temp3 = memory[PC] ;
				temp4 = temp3;
				in_r  = temp4 & 0x80 ;
				in_r >>= 7;
				temp4 = temp3;
				r_no = temp4 & 0x70 ;
				r_no >>= 4 ;
				temp4 = temp3 ;
				by_te = temp4 & 0x08 ;
				by_te >>= 3 ;
				switch(in_r){
				   case 0:switch(by_te){
							   case 0: temp4 = D[r_no].byte ;
									   break ;
							   case 1: temp4 = D[r_no].word;
									   break ;
						   }
						   break;
				   case 1:switch(by_te){
							   case 1: temp4 = A[r_no].word;
									   break ;
						   }
						   break;

				 }
				PC += 1;
				disp8 = memory[PC] ;
				E_addr = A[reg_no].dword + temp4 + disp8;
				PC += 1;
				break;

		 case 7: switch(pc_rla){
					  case 0: PC += 1;
							  abs16 = memory[PC] ;
							  abs16 <<= 8;
							  PC += 1;
							  abs16 += memory[PC];
							  E_addr = abs16;
							  PC += 1;
							  break;
					  case 1: PC += 1;
							  abs32 = memory[PC] ;
							  abs32 <<= 8;
							  PC += 1;
							  abs32 += memory[PC];
							  abs32 <<= 16;
							  PC += 1;
							  temp4 =  memory[PC] ;
							  temp4 <<= 8;
							  PC += 1;
							  temp4 += memory[PC] ;
							  abs32 += temp4 ;
							  E_addr = abs32 ;
							  PC += 1;
							  break;
				  }
				  break;
		   }
	  return E_addr;
   }
 /*************************************************************************/
  void SBCD(void)
   {
		 unsigned  int  reg1,reg2,mode,pattern,temp1,temp2 ,cr, shift,lcv,i  ;
		 long int  mask,source,dest, store,result ;
     pattern = memory[PC+1] ;
     reg1 = pattern & 0x07 ;  /* source reg no.  */
     mode = pattern & 0x38 ;
     mode >>= 3  ;
     pattern = memory[PC] ;
     reg2 = pattern & 0x0E ;  /*  dest. reg no. */
     cr = FLAG.x ;
     shift = 0;
     store = 0 ;
     if ( mode ==0) {
		     source= D[reg1].byte ;
		     dest = D[reg2].byte ;    /* For Byte  operation */
		     lcv =2 ;
		     mask = 0x0f ;
		 }
	  else {
		    source = D[reg1].dword ;
		    dest = D[reg2].dword ;
		    lcv = 8 ;                  /* For  Long Word  operation */
		    mask = 0x0000000f ;
		}
	 for(i=1;i<=lcv; i++) {
			 temp1 = ( source & mask) >> shift ;
			 temp2 = ( dest & mask ) >> shift ; /* one nibble  extract */
			 if (temp2 < temp1 )
						temp2 = temp2 + 16 ;
			 result = (temp2 -temp1) - cr   ;
			 store = store + ( result << shift ) ;
			 shift = shift + 4 ;
			 mask = mask << 4 ;
			 cr = FLAG.x ;
		      }
   if ( mode == 0 )
		D[reg2].byte =  store ;
	else
		D[reg2].dword =  store ;

   }
 /***********************************************************************/

 void  SCC(void)
{
   unsigned  int condition ,test ,pattern , mode , temp;
   unsigned  long int EA ;
   pattern =  memory[PC+1] ;
   mode = pattern & 0x38 ;
   mode >>= 3;
   pattern = memory[PC] ;
   condition = pattern & 0x0f ;
   if(condition == 4){
		 EA = eff_addr();
		 if(FLAG.c == 0) {
			   switch(mode){
						 case 0: D[EA].byte |= 0xff ;
								 break;
						 default: memory[EA] |= 0xff ;
								  break;
				}
		   }
		   else{
                  switch(mode){
						 case 0: D[EA].byte &= 0x00 ;
								 break;
						 default: memory[EA] &= 0x00 ;
								  break;

				 }
			}
		}
  else{
	  printf("\nThe call is invalid\n");
	  printf("\nTry to check instruction\n");
   }
 }
 /************************************************************************/
 void  SCS(void)
{
   unsigned  int condition ,test ,pattern , mode , temp;
   unsigned  long int EA ;
   pattern =  memory[PC+1] ;
   mode = pattern & 0x38 ;
   mode >>= 3;
   pattern = memory[PC] ;
   condition = pattern & 0x0f ;
   if(condition == 5){
		 EA = eff_addr();
		 if(FLAG.c == 1) {
			   switch(mode){
						 case 0: D[EA].byte |= 0xff ;
								 break;
						 default: memory[EA] |= 0xff ;
								  break;
				}
		   }
		   else{
                  switch(mode){
						 case 0: D[EA].byte &= 0x00 ;
								 break;
						 default: memory[EA] &= 0x00 ;
								  break;

				 }
			}
		}
  else{
	  printf("\nThe call is invalid\n");
	  printf("\nTry to check instruction\n");
   }
 }
 /************************************************************************/
 void  SEQ(void)
{
   unsigned  int condition ,test ,pattern , mode , temp;
   unsigned  long int EA ;
   pattern =  memory[PC+1] ;
   mode = pattern & 0x38 ;
   mode >>= 3;
   pattern = memory[PC] ;
   condition = pattern & 0x0f ;
   if(condition == 7){
		 EA = eff_addr();
		 if(FLAG.z ==1) {
			   switch(mode){
						 case 0: D[EA].byte |= 0xff ;
								 break;
						 default: memory[EA] |= 0xff ;
								  break;
				}
		   }
		   else{
                  switch(mode){
						 case 0: D[EA].byte &= 0x00 ;
								 break;
						 default: memory[EA] &= 0x00 ;
								  break;

				 }
			}
		}
  else{
	  printf("\nThe call is invalid\n");
	  printf("\nTry to check instruction\n");
   }
 }
 /************************************************************************/
 void  SF(void)
{
   unsigned  int condition ,test ,pattern , mode , temp;
   unsigned  long int EA ;
   pattern =  memory[PC+1] ;
   mode = pattern & 0x38 ;
   mode >>= 3;
   pattern = memory[PC] ;
   condition = pattern & 0x0f ;
   if(condition == 1){
		 EA = eff_addr();
		 if(FLAG.c == 0) {
			   switch(mode){
						 case 0: D[EA].byte |= 0xff ;
								 break;
						 default: memory[EA] |= 0xff ;
								  break;
				}
		   }
		   else{
                  switch(mode){
						 case 0: D[EA].byte &= 0x00 ;
								 break;
						 default: memory[EA] &= 0x00 ;
								  break;

				 }
			}
		}
  else{
	  printf("\nThe call is invalid\n");
	  printf("\nTry to check instruction\n");
   }
 }
 /************************************************************************/
 void  SGE(void)
{
   unsigned  int condition ,test ,pattern , mode , temp;
   unsigned  long int EA ;
   pattern =  memory[PC+1] ;
   mode = pattern & 0x38 ;
   mode >>= 3;
   pattern = memory[PC] ;
   condition = pattern & 0x0f ;
   if(condition == 12){
		 EA = eff_addr();
		 if(( FLAG.n & FLAG.v )| (!FLAG.n & !FLAG.v )) {
			   switch(mode){
						 case 0: D[EA].byte |= 0xff ;
								 break;
						 default: memory[EA] |= 0xff ;
								  break;
				}
		   }
		   else{
                  switch(mode){
						 case 0: D[EA].byte &= 0x00 ;
								 break;
						 default: memory[EA] &= 0x00 ;
								  break;

				 }
			}
		}
  else{
	  printf("\nThe call is invalid\n");
	  printf("\nTry to check instruction\n");
   }
 }
 /************************************************************************/
 void  SGT(void)
{
   unsigned  int condition ,test ,pattern , mode , temp;
   unsigned  long int EA ;
   pattern =  memory[PC+1] ;
   mode = pattern & 0x38 ;
   mode >>= 3;
   pattern = memory[PC] ;
   condition = pattern & 0x0f ;
   if(condition == 14){
	 EA = eff_addr();
	 if( (FLAG.n & FLAG.v & FLAG.z ) | (!FLAG.n & !FLAG.v & !FLAG.z ) ) {
			   switch(mode){
						 case 0: D[EA].byte |= 0xff ;
								 break;
						 default: memory[EA] |= 0xff ;
								  break;
				}
			    }
		   else{
                  switch(mode){
						 case 0: D[EA].byte &= 0x00 ;
								 break;
						 default: memory[EA] &= 0x00 ;
								  break;

				 }
			}
		}
  else{
	  printf("\nThe call is invalid\n");
	  printf("\nTry to check instruction\n");
   }
 }
 /************************************************************************/
 void  SHI(void)
{
   unsigned  int condition ,test ,pattern , mode , temp;
   unsigned  long int EA ;
   pattern =  memory[PC+1] ;
   mode = pattern & 0x38 ;
   mode >>= 3;
   pattern = memory[PC] ;
   condition = pattern & 0x0f ;
   if(condition == 2){
		 EA = eff_addr();
		 if( !FLAG.c &  !FLAG.z ) {
			   switch(mode){
						 case 0: D[EA].byte |= 0xff ;
								 break;
						 default: memory[EA] |= 0xff ;
								  break;
				}
		   }
		   else{
                  switch(mode){
						 case 0: D[EA].byte &= 0x00 ;
								 break;
						 default: memory[EA] &= 0x00 ;
								  break;

				 }
			}
		}
  else{
	  printf("\nThe call is invalid\n");
	  printf("\nTry to check instruction\n");
   }
 }

 /************************************************************************/



 /************************************************************************/


			/* NAME :       Shibir saha  */
			/* ROLL :       26  */
			/*INSTRUCTION : SVS , SWAP , TAS */


/***********************************************************************/
#include "defina_n.h"

unsigned long int Calc_Effaddr26(void)    /* This function calculates EA */
{
   unsigned int  pattern , temp, reg_no ,mode ,pc_rla,disp16,temp3,temp4 ;
   unsigned int   in_r ,wo_rd , r_no , disp8 , abs16 ;
   unsigned long int  E_addr , abs32  ;

   PC += 1;
   pattern = memory[PC] ;  /* Load lower 8 bit of the op code bit pattern */
   temp = pattern;
   mode = temp & 0x38 ;   /*Extract 3 bit for addressing  mode */
   mode >>= 3;

   temp = pattern;
   if ( mode == 7)
	  pc_rla = temp & 0x07;  /* Extract 3 bit for PC relative addressing */
   else                      /*  mode */
	  reg_no = temp & 0x07;   /* Extract 3 bit for other than PC relative */
			   /* addressiing mode (This 3 bit indicates REG. no */

   switch(mode){

	case 0: E_addr = reg_no ;  /* Data register no as  EA */
			PC += 1;
			break;

	case 2: E_addr =  A[reg_no].dword ; /* Content of Address reg. as */                            PC += 1;
			break;                      /*   EA */

	case 3: E_addr =  A[reg_no].dword;  /* Content of Address reg. as */                            PC += 1;
			A[reg_no].dword += 1;       /*   EA  & add 1(byte) to the */
			PC += 1 ;                   /* Content of that reg . */
			break;

	case 4: A[reg_no].dword -= 1;   /* Subtract 1(byte) from the content*/
			E_addr = A[reg_no].dword ; /* of the Address reg. & content */
			PC += 1;                  /* of that reg. as EA */
			break ;

	case 5: PC += 1;
			disp16 = memory[PC] ;   /*Upper byte of the DISP16 */
			disp16 <<= 8;
			PC += 1;
			disp16 += memory[PC];    /* 16 bit displacement  */
			E_addr = A[reg_no].dword + disp16;
			PC += 1;
			break ;

	case 6: PC += 1 ;
			temp3 = memory[PC] ; /* 8 bit for finding  Xn */
			temp4 = temp3;
			in_r  = temp4 & 0x80 ;  /* Indicating bit for address reg */
			in_r >>= 7;             /* or data reg. */
			temp4 = temp3;
			r_no = temp4 & 0x70 ;   /* Extract reg. no */
			r_no >>= 4 ;
			temp4 = temp3 ;
			wo_rd = temp4 & 0x08 ; /* Indicating bit for word(0) or long */
			wo_rd >>= 3 ;	/* word (1) */

			switch(in_r){
				case 0:switch(wo_rd){   /* Data reg. as Xn */
						   case 0: temp4 = D[r_no].word;
								   break ;
						   case 1: temp4 = D[r_no].dword;
								   break ;
			     }
			    break;
		      case 1:switch(wo_rd){    /* Address reg. as Xn */
						 case 0: temp4 = A[r_no].word ;
								 break;
						 case 1: temp4 = A[r_no].dword;
								 break ;
			      }
			      break;

		  }
		  PC += 1;
		  disp8 = memory[PC] ;   /* Retrieve 8 bit displacement */
		  E_addr = A[reg_no].dword + temp4 + disp8;/* EA calculation */
		  PC += 1;
		  break;

	 case 7: switch(pc_rla){
				case 0:  PC += 1 ;  /* Retrieve 16 bit after op code bit */
						 abs16 = memory[PC] ; /* pattern which is used as */
						 abs16 <<= 8 ;       /*     EA */
						 PC += 1 ;
						 abs16 += memory[PC] ;
						 E_addr = abs16 ;
						 PC += 1 ;
						 break;

				case 1: PC += 1; /* Retrieve 32 bit after op bit pattern */
						abs32 = memory[PC] ;  /* which is used as EA */
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
 void SWAP(void)         /* This function performs the task of SWAP inst. */
 {

	extern A_REGS		A[8] ;		/* A0...A[7] Address registers */
	extern D_REGS		D[8] ;		/* D0...D[7]    Data registers */
	extern FLAG_REGS   FLAG ;		/* flag register */
	extern REG32		PC	 ;      /* program counter */

	unsigned int  pattern , Dreg_no, temp1 ,temp2 ;

	pattern = memory[PC+1]  ;
	Dreg_no = pattern & 0x07 ;
	temp1 = D[Dreg_no].word;  /*Lower 16 bit */
	D[Dreg_no].dword >>= 16;
	temp2 = D[Dreg_no].word ; /* Upper 16 bit */
	D[Dreg_no].word = temp1 ; /* Swaping lower 16 bit */
	D[Dreg_no].dword <<= 16 ;
	D[Dreg_no].word = temp2 ;  /* Swaping upper 16 bit */
	FLAG.c = 0 ;
	FLAG.v = 0 ;
	PC += 2 ;
 }
 /*************************************************************************/
 void  TAS(void)             /* This funcction performs the task of */
 {                          /*   TAS instruction */

   extern     A_REGS		A[8] ;		/* A0...A[7] Address registers */
   extern D_REGS		D[8] ;		/* D0...D[7]    Data registers */
   extern FLAG_REGS   FLAG ;		/* flag register */
   extern REG32		PC	 ;      /* program counter */

   unsigned long int  EA  ;
   unsigned int test , pattern , mode ;

   pattern = memory[PC+1] ;
   mode = pattern & 0x38 ;
   mode >>= 3 ;
   EA = Calc_Effaddr26() ;

   switch(mode){
	 case 0: test = D[EA].byte ; /* Retrieve lower 8 bit of Data reg. */
			 switch(test){
				       /* for testing , EA provides reg no */
		       case 0: FLAG.z = 1 ;  /* Set zero(0)  flag */
					   break ;

		       default: FLAG.z = 0 ;  /* Reset zero flag */
						FLAG.n = 1 ;  /* Reset negative flag */
						D[EA].byte |= 0x80 ; /* Set 7th bit of test byte */
						break;               /* as one(1)  */
		   }
		   break ;

	  default : test = memory[EA] ;  /* Retrieve 8 bit  from memory */
				switch(test){       /* for testing */
					  case 0: FLAG.z = 1 ;
							  break ;

					  default: FLAG.z = 0 ;
							   FLAG.n = 1 ;
							   memory[EA] |= 0x80 ;  /* Set 7th bit of memory */
							   break;               /* test byte to one(1) */
		       }
		       break ;
	}
	FLAG.c = 0;
	FLAG.v = 0;
 }
/**************************************************************************/
void  SVS(void)            /* This function performs the  task of SVS */
{                                  /* instruction */

   extern  A_REGS		A[8] ;		/* A0...A[7] Address registers */
   extern  D_REGS		D[8] ;		/* D0...D[7]    Data registers */
   extern  FLAG_REGS   FLAG ;		/* flag register */
   extern  REG32		PC	;      /* program counter */

   unsigned  int test ,pattern , mode , temp;
   unsigned  long int EA  ;

   pattern =  memory[PC+1] ;
   mode = pattern & 0x38 ;
   mode >>= 3;
   pattern = memory[PC] ;

    EA = Calc_Effaddr26();
    if(FLAG.v == 1) {
		   switch(mode){
				  case 0: D[EA].byte |= 0xff ; /* Set lower 8 bit of */
						  break;   /* Data reg. as 1's */

				  default: memory[EA] |= 0xff ; /* Set 8 bit of */
						   break;              /* memory as 1's */
		 }
     }
    else{
      switch(mode){
			case 0: D[EA].byte &= 0x00 ;  /* Reset lower bit of */
					break;                /* Data reg. as 0's */

			default: memory[EA] &= 0x00 ; /* Reset 8 bit of */
					 break;               /* memory as  0's */
       }
    }
 }
 /************************************************************************/

/*--------- Written jointly by Rolls : 4, 15 & 19 ----------*/

#include <stdio.h>
#include "defina_n.h"
#include "nemonics.h"

void group0();
void group1();
void group2_3();
void group4();
void group5();
void group6();
void group7();
void group8();
void group9();
void group11();
void group12();
void group13();
void group14();

void ABCD();
void ADD();
void ADDA();
void ADDI();
void ADDQ();
void ADDX();
void AND();
void ANDI();
void ANDI_CCR();
void ANDI_SR();
void ASL();
void ASR();
void BCHG();
void BCLR();
void BSET();
void BTST();
void CLR();
void CMP();
void CMPA();
void CMPI();
void CMPM();
void DBcc( );
void DIVS();
void DIVU();
void EOR();
void EORI();
void EORI_CCR();
void EORI_SR();
void EXG();
void EXT();
void JMP();
void LEA();
void LSL();
void LSR();
void MOVE();
void MOVE_CCR();
void MOVE_SR();
void MOVE_USP();
void MOVEA();
void MOVEM();
void MOVEP();
void MOVEQ();
void MULS();
void MULU();
void NBCD();
void NBCD();
void NEG();
void NOP();
void NOT();
void OR();
void ORI();
void ROL();
void ROR();
void ROXL();
void ROXR();
void SBCD();
void STOP();
void SUB();
void SUBA();
void SUBI();
void SUBQ();
void SUBX();
void SWAP();
void TAS();
void TST();
void UNLINK();

static unsigned char fb  , 		/* first byte of opcode */
					 fbln , 	/* first byte lower nibble of opcode */
					 sb ,	/* second byte of opcode */
					 sbu2 ,	/*   "     "  upper 2 bits */
					 halt ; /*    Whether STOP is executed   */
void (*opcode4[16])() = {group0,group1,group2_3,group2_3,group4,group5,group6,
						group7,group8,group9,NULL,group11,group12,group13,
						group14,NULL};
/*void (*DBcc[16])() = {NULL,NULL,DBHI,DBLS,DBCC,DBCS,DBNE,DBEQ,DBVC,DBVS,DBPL,
					  DBMI,DBGE,DBLT,DBGT,DBLE};*/
void (*Bcc[16])() ={BRA,NULL,BHI,BLS,BCC,BCS,BNE,BEQ,BVC,BVS,BPL,BMI,BGE,BLT,
								/* BLT is written in this module */
					 BGT,BLE};
void (*Scc[16])() = {ST,SF,SHI,SLS,SCC,SCS,SNE,SEQ,SVC,SVS,SPL,SMI,SGE,SLT,
					 SGT,SLE};

void single_step()
{
	halt = 0;
	fb = memory[PC];
	fbln = fb & 0x0F;
	sb = memory[PC+1];
	sbu2 = sb >> 6 ;


	if ((memory[PC] || memory[PC + 1]) && !(PC % 2))
		( *opcode4[ fb >> 4 ] )();
	else halt = 1;
}

void run()
{
	for(halt = 0 ; !halt ; )
		single_step();
}

void BLT(void)
{
	int displacement ;

    displacement = (signed char) memory[PC + 1];
	if (FLAG.n ^ FLAG.v) {		/* condition met */
		if (!displacement) 		/* 16 bit displacement */
			displacement = (memory[PC + 2] << 8) | memory[PC + 3];
		PC += displacement + 2;
	}
	PC += (displacement) ? 2 : 4 ;
}

void BTST()
{
	if (fbln % 2)
		BTST_Dn();
	else {
		if ((fbln & 0x0E) == 0x08)
			BTST_Imm();
	}
}

void BSET()
{
	if (fbln % 2)
		BSET_Dn();
	else {
		if ((fbln & 0x0E) == 0x08)
			BSET_Imm();
	}
}


void group0()
{
	if (fbln % 2)
		switch (sbu2) {
			case 0 : BTST() ; break ;
			case 1 : BCHG() ; break ;
			case 2 : if ((sb & 0x38) == 0x08)
						MOVEP();
					 else
						BCLR();
					 break ;
			case 3 : BSET() ;
		}
	else
		switch (fbln) {
			case 0 : if (sb == 0x3C)
						ORI();      		/* ORI_CCR*/
					 else if (sb == 0x7C)
						; 					/* ORI_SR(); N.W. */
					 else
						ORI();
					 break;
			case 2 : if (sb == 0x3C)
						ANDI_CCR();
					 else if (sb == 0x7C)
						ANDI_SR();
					 else
						ANDI();
					 break;
			case 4 : SUBI() ; break ;
			case 6 : ADDI() ; break ;
			case 10: if (sb == 0x3C)
						 ; 			/* EORI_CCR();          Not WRITTEN */
					 else if (sb == 0x7C)
						EORI_SR();
					 else
						EORI();
					 break;
			case 12: CMPI() ;
		}
}

void group1()
{
	MOVE();
}

void group2_3()
{
	if (!(fbln % 2) && sb >> 6 == 0x01 )
		MOVEA();
	else
		MOVE();
}

void group4(void)
{
	if(fbln%2){
		if((sb>>6)==3){
			LEA();
			return;
		}
	 }
	 else{
		switch(fbln){

			case 0 : if((sb>>6)==3)
						MOVE_SR();
					  else
						NEG();  	/*   NEGX	*/
					  return ;

			case 2 : CLR();
					 return;

			case 4 : if((sb>>6)==3)
						MOVE_CCR();
					  else
						NEG();
					  return;

			case 6 : if((sb>>6)==3)
						MOVE_SR();
					  else
						NOT();
					  return;

			case 8 : switch(sb>>6){

						case 0 : NBCD();
								 return;

						case 1 : if((sb>>3)==0x08)
									SWAP();
								 else
									; /* PEA();         NOT WRITTEN  */
								 return;

						case 2 : if((sb>>3)==0x10)
									EXT();
								 else
									MOVEM();
								 return;

						case 3 : MOVEM();
								 return;

						}

			case 10 : if((sb>>6)==3)
						 TAS();
					  else
						 TST();
					   return;

			case 12 : if((sb>>6)>1)
						  MOVEM();
					  return;

			case 14 : switch(sb>>6){

						case 1 : switch((sb & 0x38)>>3){

										case 2 :  /* LINK();   NOT WRITTEN*/
												 return;

										case 3 : UNLINK();
												 return;

										case 5 : MOVE_USP();
												 return;

										case 6 : switch( sb & 0x03){

													case 0 : /* RESET();
																 N.W.*/
															 return;

													case 1 : NOP();
															 return;

													case 2 : halt = 1;
															 PC += 4;
															 /*STOP()*/
															 return;
													}
										}

						case 3 : JMP();
								 return;
						}
			   }
		 }
  }

void group5()
{
	if ( sb >> 6 == 0x03 ){
		if (( sb & 0x38 ) >> 3 == 1 )
			DBcc();
		else
			(*Scc[fbln])();
	}
	else {
		if ( fbln % 2 )
			SUBQ();
		else
			ADDQ();
	}
}

void group6()
{
	(*Bcc[fbln])();
}

void group7()
{
	if (!(fbln % 2))
		MOVEQ();
}

void group8()
{
	if (fbln % 2)
		switch (sbu2) {
			case 0 : if (!(sb & 0x38))
						SBCD();
					 else
						OR();
					 break ;
			case 1 :
			case 2 : OR() ; break ;
			case 3 : DIVS() ;
		}
	else {
		if (sb == 0x03)
			DIVU();
		else
			OR();
	}
}

void group9()
{
	if (fbln % 2){
		if ( !(sb & 0x30) )
			SUBX();
		else if ( (sb >> 6) == 0x03 )
			SUBA();
		else
			SUB();
	}
	else{
		if ( (sb >> 6) == 0x03 )
			SUBA();
		else
			SUB();
	}
}

void group11()
{
	if (fbln % 2){
		if ( (sb & 0x38) == 0x08 )
			CMPM();
		else if ( (sb >> 6) == 0x03 )
			CMPA();
		else
			EOR();
	}
	else{
		if ( (sb >> 6) == 0x03 )
			CMPA();
		else
			CMP();
	}
}

void group12()
{
	if (fbln % 2){
		if ( !(sb & 0xF0) )
			ABCD();
		else if ( (sb & 0xF0) == 0x40 )
			EXG();
		else if ( (sb >> 3) == 0x11 )
			EXG();
		else if ( (sb >> 6) == 0x03 )
			MULS();
		else
			AND();
	}
	else{
		if ( (sb >> 6) == 0x03 )
			MULU();
		else
			AND();
	}
}

void group13()
{
	if (fbln % 2){
		if ( !(sb & 0x30) )
			ADDX();
		else if ( (sb >> 6) == 0x03 )
			ADDA();
		else
			ADD();
	}
	else{
		if ( (sb >> 6) == 0x03 )
			ADDA();
		else
			ADD();
	}
}

void group14()
{
	if (fb % 2){
		if ( (sb >> 6) == 0x03 )
			switch ( fbln >> 1 ){
				case 0 : ASL(); return;
				case 1 : LSL(); return;
				case 2 : ROXL(); return;
				case 3 : ROL(); return;
			}
		else
			switch ( (sb & 0x18) >> 3 ){
            	case 0 : ASL(); return;
				case 1 : LSL(); return;
				case 2 : ROXL(); return;
				case 3 : ROL(); return;
			}
	}
	else {
    	if ( (sb >> 6) == 0x03 )
			switch ( fbln >> 1 ){
				case 0 : ASR(); return;
				case 1 : LSR(); return;
				case 2 : ROXR(); return;
				case 3 : ROR(); return;
			}
		else
			switch ( (sb & 0x18) >> 3 ){
				case 0 : ASR(); return;
				case 1 : LSR(); return;
				case 2 : ROXR(); return;
				case 3 : ROR(); return;
			}
	}
}

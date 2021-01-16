#include "defina_n.h"

void choice_func(void);      /*This function is to update the PC
			     by adding the displacement*/

void BEQ(void);
void BGE(void);
void BGT(void);
void BHI(void);
void BLE(void);
void BLS(void);


/*************************************************************************/
void choice_func(void)
{
	   int patt ;

	   patt = (signed char) memory[PC+1];          /*lower byte*/
       if(patt)                /*Displacement in byte*/
		 PC += patt + 2 ;            /* Add diplace to byte*/
	else                            /* Displacement in word*/
	   {
		  patt = (memory[PC+2] << 8) | memory[PC + 3];    /*Displacement*/
		  PC += patt + 2;
	   }
}
/***********************************************************************/
void BEQ(void)
 {
	if (FLAG.z)
		choice_func();
	else
		PC += (memory[PC + 1]) ? 2 : 4 ;


 }
 /**********************************************************************/
 void BGE(void)
 {
	if((FLAG.n & FLAG.v) | (!FLAG.n & !FLAG.v))
		choice_func();
	else
		PC += (memory[PC + 1]) ? 2 : 4 ;
 }

 /***********************************************************************/
 void BGT(void)
 {
   if((FLAG.n & FLAG.v & !FLAG.z) | (!FLAG.n & !FLAG.v & !FLAG.z))
	  choice_func();
   else
		PC += (memory[PC + 1]) ? 2 : 4 ;

 }
 /********************************************************************/
 void BHI(void)
 {
   if(!FLAG.c & !FLAG.z)
	  choice_func();
   else
		PC += (memory[PC + 1]) ? 2 : 4 ;

 }
 /**********************************************************************/
 void BLE(void)
 {
   if(FLAG.z | (FLAG.n & !FLAG.v) | (!FLAG.n & FLAG.v))
	  choice_func();
   else
		PC += (memory[PC + 1]) ? 2 : 4 ;

 }
 /***********************************************************************/
 void BLS(void)
 {
   if(FLAG.c | FLAG.z)
	  choice_func();
   else
		PC += (memory[PC + 1]) ? 2 : 4 ;

 }
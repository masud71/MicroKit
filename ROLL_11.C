/*****


													NAIMUL BASET
													ROLL - 11
													ZAODAT HASAN RAHMAN
													ROLL-12
													CSE 3RD YEAR
													COURSE : CSE 310
													1994

*****/

/************************************************************************

				The general purpose of the DBCC instructions are to test both
				the condition codes and the value in a data register.

				Sequence of operations are given below :-

						STEP 1. If the condition is true then PC is incremented by
										2 i.e. executes next instructions.

						STEP 2. If the condition is not true  then

														i. Decrement the value of the counter
															 register by 1.

													 ii. If the value is -1 after decrement
															 then PC is incremented by 2 and return.

													iii. If the value is not -1 after decreasing
															 the counter register content then branch
															 to the instruction specified by the displacement.

**************************************************************************/
#define BYTE unsigned char
#include "defina_n.h"

void DBcc( void );
void set_val_PC(void);     /* CREATED BY ROLL : 12 */
int find_displacement(void);   /*JOINTLY CREATED BY ROLL 11 AND 12*/
void dbcc_false(int reg_number,int displacement);	/***CREATED BY ROLL : 11 ***/

#define BYTE unsigned char
void DBcc( void ) /*JOINTLY CREATED BY ROLL : 11 AND 12 */
{
	BYTE	cc;
	int reg_no,disp;

  cc = memory[ PC ];
  cc &= 15;

	reg_no = memory[ PC + 1 ];
	reg_no &= 7;

	disp = find_displacement();

	switch(cc)
  {
	case 0 : set_val_PC();
			 break;
	case 1 :
					dbcc_false(reg_no,disp);
					break;

	case 2 :
					if (( !FLAG.c ) && ( !FLAG.z ))
									set_val_PC();
					else dbcc_false(reg_no,disp);
					break;

	case 3 :
					if (( !FLAG.c ) || ( !FLAG.z ))
									set_val_PC();
       		else dbcc_false(reg_no,disp);
					break;

	case 4 :
					if ( !FLAG.c )
									set_val_PC();
          else dbcc_false(reg_no,disp);
					break;

	case 5 :
					if ( FLAG.c )
								 set_val_PC();
          else dbcc_false(reg_no,disp);
					break;

	case 6 :
					if ( !FLAG.z )
								 set_val_PC();
          else dbcc_false(reg_no,disp);
					break;

	case 7 :
					if ( FLAG.z )
								 set_val_PC();
          else dbcc_false(reg_no,disp);
					break;

	case 8 :
					if ( !FLAG.v )
									set_val_PC();
					else dbcc_false(reg_no,disp);
					break;

	case 9 :
					if ( FLAG.v )
									set_val_PC();
          else dbcc_false(reg_no,disp);
					break;

	case 10 :
					if ( !FLAG.n )
									set_val_PC();
          else dbcc_false(reg_no,disp);
					break;

	case 11 :
					if ( FLAG.n )
									set_val_PC();
          else dbcc_false(reg_no,disp);
					break;

	case 12 :
					if ((( FLAG.n ) && ( FLAG.v )) || (( !FLAG.n ) && ( !FLAG.v )))
									set_val_PC();
          else dbcc_false(reg_no,disp);
					break;

	case 13 :
					if ((( FLAG.n ) && ( !FLAG.v ))	|| (( !FLAG.n ) && ( FLAG.v )))
									set_val_PC();
          else dbcc_false(reg_no,disp);
					break;

	case 14 :
					if ((( FLAG.n ) && ( FLAG.v ) && ( !FLAG.z ))
					|| (( !FLAG.n ) && ( !FLAG.v ) && ( !FLAG.z)))
								set_val_PC();
          else dbcc_false(reg_no,disp);
					break;

	case 15 :
					if ((( FLAG.z ) || (( FLAG.n ) && ( !FLAG.v ))) ||
					(( !FLAG.n ) && ( FLAG.v )))
								set_val_PC();
          else dbcc_false(reg_no,disp);
					break;

	default :break;
	}
}

/**************************************************************************/

int find_displacement(void)   /*JOINTLY CREATED BY ROLL 11 AND 12*/
{
 int content = 0;
 content = (int) memory[PC + 2];
 content <<= 8;
 content |= (int) memory[PC + 3];
 return content;
}

/**************************************************************************/

void set_val_PC(void)     /* CREATED BY ROLL : 12 */
{
   if ( PC > MAXMEM -4 )
	   /****?****/
	   return;
   else
		PC += 4;

}

/**************************************************************************/

void dbcc_false(int reg_number,int displacement)	/***CREATED BY ROLL : 11 ***/
{
					if (D[reg_number].dword == 0)				/* Checking whether the value
																							 of counter is -1 or not */
										PC += 4;								/* Jumps to next instruction */
					else {
							--D[reg_number].dword;					/* Decrements counter value */
							PC += displacement + 2 ;			/* Jumps to the address defined
																							 by displacement from PC */
					}
}

/*************************************************************************/










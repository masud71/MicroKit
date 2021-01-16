
/*					MC68000												  */
/*  Zaodat Hasan Rahman      #12										  */
/*  3RD YEAR CSE	 		       										  */
/* This is a portion of MC68000 simulator program.Instructions concerning */
/*to DIVISIONS and XOR are written here.								  */

#include "defina_n.h"

#define BYTE unsigned char

	  /* FUNCTION LISTING*/

unsigned long int calculate_EA( BYTE,char );
long int calc_reg_val( BYTE cond );
int calc_disp();
void DIVS( void );
void DIVU( void );
void EOR();
void EORI();
void EOR_EORI();
void reg_update( unsigned long int , BYTE , BYTE );
long int content_of_Addr( unsigned long int, BYTE );
void update_memory( unsigned long int,unsigned long int, BYTE );
BYTE __flag1;

/*NAME : content_of_Addr()
  FUNCTION :This function returns the content of a memory address.
  ARGUMENTS:It takes the address and size of content.Upon the value
			of size it reads from memory BYTE,WORD or LONG WORD.

   VARIABLES USED : int counter,long int content(content of memory)

*/
long int content_of_Addr( unsigned long int Addr, BYTE size )
{
   BYTE counter;
   long int content = 0;

   if ( size == 2 ) size = 3;            /*Adjust size for long word*/
   for ( counter = 0; counter <= size; ++counter )
   {
	 content <<= 8;
	 content |= memory[ Addr + counter ];/*Read from memory*/
	}
  return content;
}

/*NAME : calculate_EA()
  FUNCTION :This function returns the value of effective address.
  ARGUMENTS:It takes size of content( 16 0r 24 bits) and instruction type.
  VARIABLES USED : long int EA,unsigned char data_reg_no,Addr_reg_no,
				   mode.

*/
unsigned long int calculate_EA( BYTE N, char inst)
{
  unsigned long int EA;
  BYTE data_reg_no, Addr_reg_no, mode;

  N = ( N != 2 ) ? (N + 1) : 4;
  Addr_reg_no = mode = memory[ PC + 1 ];
  Addr_reg_no &= 7;   /*Determine address register number*/
  mode &= 56;
  mode >>= 3;         /*Determine addressing mode*/
  data_reg_no = memory[ PC ];
  data_reg_no &= 14;
  data_reg_no >>= 1;  /*Determine data register number*/

  switch( mode )
  {
	int d16;
	BYTE ex_byte = 0;

	case 0:	EA = D[ data_reg_no].dword;
			__flag1 = 1; /*Set flag for register operation*/
			break;
	case 1:	break;       /*Not a used mode*/
	case 2:	EA = A[ Addr_reg_no ].dword;
			break;
	case 3: EA = A[ Addr_reg_no ].dword;
			A[ Addr_reg_no ].dword += N;
			break;
	case 4:	A[ Addr_reg_no ].dword -= N;
			EA = A[ Addr_reg_no ].dword;
			break;
	case 5: d16 = content_of_Addr( PC + 2,1);/*calculate displacement*/
			EA = A[ Addr_reg_no].dword + d16;/*calculate effec.address*/
			break;
	case 6:	ex_byte = memory[ PC + 2 ];      /*extended byte*/
			if  ( ex_byte & 0x80 )
				ex_byte = 1;                 /*determine data or addr. reg*/
			EA =( long int )(A[ Addr_reg_no].dword + calc_reg_val( ex_byte ) + calc_disp());
			break;
	case 7:switch( Addr_reg_no )
		   {
			 case 0 : EA = content_of_Addr( PC + 2,1); /*16 bit displacement*/
					  break;
			 case 1 : EA = content_of_Addr( PC + 2,2);/*32 bit displacement*/
					  break;
			 case 2:  if ( inst == 'd')
					  {d16 = content_of_Addr( PC + 2,1 );
					   EA = PC + d16;
					  }
					  break;
			 case 3:  if ( inst == 'd')
					  {
						ex_byte = memory[ PC + 2 ];
						if  ( ex_byte & 0x80 )
							ex_byte = 1;
						EA =( long int )( PC  + calc_reg_val( ex_byte ) + calc_disp());
					   }
					   break;
			 case 4:  if ( inst == 'd')
						break;

			}
			break;
  }
 return EA;
}

/*NAME : calc_rag_val()
  FUNCTION :This function returns the value of data or address register.
  ARGUMENTS:It takes condition, whether the operation involves address or
			data register	.
  VARIABLES USED : long int rag_val,unsigned char reg_no,

*/
long int calc_reg_val( BYTE cond )
{
  BYTE reg_no;
  long int reg_val;

  reg_no = memory[ PC + 2 ];
  reg_no &= 0x70;
  reg_no >>= 4;/*Determine register number*/
  switch( cond )
  {
	case 0 : reg_val = D[ reg_no ].dword;
			 break;
	case 1 : reg_val = A[ reg_no ].dword;
			 break;
   }
   if (( memory[ PC + 2 ] & 0x08 ) == 0 )
   {
	reg_val &= 0x0000ffff;       /*Sign extension*/
	if ( reg_val & 0x00008000 )
	  reg_val |= 0xffff0000;     /*Sign extension*/
   }
   return reg_val;
}

int calc_disp()
{
 int d8 = 0;

  d8 = memory[ PC + 3 ];
  if ( d8 & 0x0080 )     /*Signed displacement calculation*/
	d8 |= 0xff00 ;
  return d8;
}

/*NAME : DIVU()
  FUNCTION :This function does unsigned division.
  VARIABLES USED : quotient,remainder,EA(effective address),cont_EA(content
  of effective address),data_reg(data register)

*/
void DIVU()
{
   BYTE data_reg;
   unsigned long int quotient,EA;
   unsigned int remainder,cont_EA;

   data_reg = memory[ PC ];
   data_reg &= 14;             /*Determine data register number*/
   EA = calculate_EA( 1,'d' );
   if ( !__flag1 )             /*Not register operation*/
	cont_EA = content_of_Addr( EA,1 );
   else cont_EA = EA;
   if ( cont_EA )
   {
	   data_reg >>= 1;

	   quotient = abs(D[ data_reg ].dword) / abs(cont_EA);
	   remainder = abs(D[ data_reg ].dword) % abs(cont_EA);
												  /*Set flag register*/
	   FLAG.v = ( quotient / 65536) ? 1 : 0;
	   FLAG.c = 0;
	   if ( !FLAG.v ) FLAG.n = (  quotient & 0x00008000 ) ? 1 : 0;
	   if ( !FLAG.v ) FLAG.z = ( ! quotient ) ? 1 : 0;

	   D[ data_reg ].dword = remainder;/*Store quotient*/
	   D[ data_reg ].dword <<= 16;
       quotient &= 0x0000ffff;
	   D[ data_reg ].dword |= quotient;/*Store remainder*/
   }
 __flag1 = 0;                                         /*Reset flag */
}

/*NAME : DIVS()
  FUNCTION :This function does signed division.
  VARIABLES USED : quotient,remainder,EA(effective address),cont_EA(content
  of effective address),data_reg(data register)

*/
void DIVS()
{
   BYTE data_reg;
   unsigned long int EA;
   int cont_EA;
   long int quotient;
   int remainder;

   __flag1 = 0;
   data_reg = memory[ PC ];
   data_reg &= 14;             /*Determine data register number*/
   EA = calculate_EA( 1,'d');
   if ( !__flag1 )             /*Not register operation*/
	cont_EA = content_of_Addr( EA,1 );
   else cont_EA = EA;
   if ( cont_EA )
   {
	  data_reg >>= 1;
	  quotient = abs(D[ data_reg ].dword) / abs(cont_EA);
	  remainder =abs(D[ data_reg ].dword) % abs(cont_EA);
	   /*For negative number adjust quotient,remainder*/
	  if (( cont_EA < 0 ) && ( (long int)D[ data_reg ].dword > 0 ))
		quotient *= -1;
	  if (( cont_EA > 0 ) && ( (long int)D[ data_reg ].dword < 0 ))
	  {
		 quotient *= -1;
		 remainder *= -1;
	  }                                       /*Set flag register*/
      FLAG.v = (( quotient > 32767 ) || ( quotient < -32768 )) ? 1 : 0;
	  FLAG.c = 0;
	  if ( !FLAG.v ) FLAG.n = (  quotient < 0 ) ? 1 : 0;
	  if ( !FLAG.v ) FLAG.z = ( ! quotient )  ? 1 : 0;

	  D[ data_reg ].dword = remainder; /*Store remainder*/
	  D[ data_reg ].dword <<= 16;
	  quotient &= 0x0000ffff;
	  D[ data_reg ].dword |= quotient; /*Store remainder*/
   }
 __flag1 = 0;                          /*Reset flag */
}
void EOR()
{
	EOR_EORI();
}
void EORI()
{
	EOR_EORI();
}

/*NAME : EOR_EORI()
  FUNCTION :This function XOR or immediate XOR operation.
  VARIABLES USED :src_reg(source register),addr_reg(address register)
  EA(effective address),cont_EA(content of effective address)
*/

void EOR_EORI()
{
  BYTE src_reg,size,addr_reg;
  unsigned long int EA,cont_EA;

  __flag1 = 0;
  if ( memory[ PC ] & 0x10 )
  {
	src_reg = memory[ PC ];
	src_reg &= 14;
	src_reg >>= 1;        /*Determine source register number*/
  }
  size = memory[ PC + 1];  /*Determine size*/
  size >>= 6;
  EA = calculate_EA( size,'e' );
  addr_reg = memory[ PC + 1 ];
  addr_reg &= 0x07;       /*Determine address register number*/
  if( !__flag1 )
	cont_EA = content_of_Addr( EA,size );
  else cont_EA = EA;
  if ( memory[ PC ] & 0x10 )
   {
	switch( size )
	{
	 case 0: cont_EA ^= D[ src_reg ].byte ;
			 break;
	 case 1: cont_EA ^= D[ src_reg ].word ;
			 break;
	 case 2: cont_EA ^= D[ src_reg ].dword ;
			 break;
	}
	if ( !__flag1 )
	 update_memory( EA,cont_EA,size );
	else
	  reg_update( cont_EA, size,addr_reg );
	PC += 2;
   }
  else                                  /*EORI*/
  {
    switch( size )
	{
	 unsigned int data16;
	 unsigned long int data32;
	 case 0: cont_EA ^= memory[ EA ] ;
			 PC += 4;
			 break;
	 case 1: data16 = content_of_Addr( PC+2,size );
			 cont_EA ^= data16 ;
			 PC += 4;
			 break;
	 case 2: data32 = content_of_Addr( PC+2,size );
			 cont_EA ^= data32 ;
			 PC += 6;
			 break;
	}
	if ( !__flag1 )                   /*Register operation*/
		update_memory( EA,cont_EA,size );
	else
    	reg_update( cont_EA, size,addr_reg );
   }
   FLAG.v = FLAG.c = 0;
   FLAG.n = ( (long int) cont_EA < 0 ) ? 1 : 0;
   FLAG.z = ( ! cont_EA ) ? 1 : 0;
   __flag1 = 0;
}

/*NAME : update_memory()
  FUNCTION :This function keeps the result of a operation in memory.
  ARGUMENTS:Addr(address),cont_EA(content of memory),size(for determining
			whether a BYTE,WORD or LONG WORD will be kept in memory.
  VARIABLES USED :unsigned char counter.

*/
void update_memory( unsigned long int Addr,
					unsigned long int cont_EA, BYTE size )
{
   BYTE counter;

   switch(size)
   {
	 case 0: cont_EA <<= 24;
			 break;
	 case 1: cont_EA <<= 16;
			 break;
	 case 2: size = 3;
			 break;
   }
   for ( counter = 0; counter <= size; ++counter )
   {
	 memory[ Addr + counter ] = cont_EA / 0x1000000;
	 cont_EA <<= 8;
	}
}

void reg_update( unsigned long int data, BYTE size, BYTE number)
{
   switch( size )
   {
	 case 0 : D[number].byte = data;
			  break;
	 case 1 : D[number].word = data;
			  break;
	 case 2 : D[number].dword = data;
			  break;
	}
}
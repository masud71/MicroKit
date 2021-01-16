/*                         A. K. M. HASAN                       */
/*                            ROLL # 25                         */
/*                            3RD YEAR                          */
/*                            CSE 310                           */

#include "defina_n.h"

void SUBX() ;
void SVC() ;
int Calc_Effaddr25( unsigned , unsigned ) ;
void data_reg_mode( unsigned  , unsigned  , unsigned ) ;
void addr_reg_mode( unsigned  , unsigned  , unsigned  , unsigned ) ;
void set_flags( unsigned , unsigned , unsigned ,  unsigned ) ;
/*--------------------------------------------------------*/


 void SUBX()
	 {

	 unsigned op_code , dest_no , src_no , size , mode ,msb = 0 ;

	 op_code =  memory[ PC ] ;          /* taking opcode bit pattern */
	 dest_no = ( op_code & 0x0F ) >> 1 ;/* and breaking it in desired*/
	 op_code =  memory[ ++PC ] ;        /* field */
	 size = op_code >> 6 ;
	 mode = ( op_code & 0x38 ) >> 3 ;
	 src_no = op_code & 0x7 ;
	 if( mode == 0 ) data_reg_mode( dest_no , src_no , size ) ;
	 else  addr_reg_mode( dest_no , src_no , size , msb) ;
	 set_flags( size , mode , dest_no , msb ) ;
	 PC++ ;
	 }
/*-----------------------------------------------------------*/
							/* Dx - Dy - X -> Dx */

 void data_reg_mode( unsigned dest_no , unsigned src_no , unsigned size )
	 {

	 unsigned temp_x ;

	 switch( size ){
			 case 0 : if( D[ src_no ].byte < ( D[ dest_no ].byte + FLAG.x ))
												  temp_x = 1 ;  /* save carry */
						 else               temp_x = 0 ;
						 D[ dest_no ].byte = D[ src_no ].byte - D[ dest_no ].byte - FLAG.x ;
						 break ;
			 case 1 : if( D[ src_no ].word < ( D[ dest_no ].word + FLAG.x ))
												  temp_x = 1 ;   /* save carry */
						 else               temp_x = 0 ;
						 D[ dest_no ].word = D[ src_no ].word - D[ dest_no ].word - FLAG.x ;
						 break ;
			 case 2 : if( D[ src_no ].dword < ( D[ dest_no ].dword + FLAG.x ))
												  temp_x = 1 ;
						 else               temp_x = 0 ;
						 D[ dest_no ].dword = D[ src_no ].dword - D[ dest_no ].dword - FLAG.x ;
						 break ;
			}
	 FLAG.x = temp_x ;
	 }
/*----------------------------------------------------------------*/
		 /*    -( Ax ) - -( Ay ) - X -> ( Ax ) */

void addr_reg_mode( unsigned dest_no , unsigned src_no , unsigned size , unsigned msb)
	{

	unsigned    temp_x , test = 0 ;
	long int src_adr , dest_adr ;
	register int i ,loop ;

	if( size == 2 ) loop = 3 ;
	else loop = size ;
	for( i = loop ; i >= 0 ; i-- )
		{
		src_adr =  ( A[ src_no ].dword + i ) & 0x0FFF ;  /* use 24 bits as */
		dest_adr =  ( A[ dest_no ].dword + i ) & 0x0FFF ;/* address        */
		if(  memory[ src_adr ] < (  memory[ dest_adr ] + FLAG.x ))
						temp_x = 1 ;   /* save carry */
		else        temp_x = 0 ;
		memory[ dest_adr] = memory[ src_adr ] - memory[  dest_adr ] -FLAG.x ;
		msb = memory[ dest_adr ] >> 7 ;  /* find msb */

		/* find Z FLAG have to be set or reset */
		if(  memory[ dest_adr  ] != 0 )
													{
													FLAG.z = 0 ;
													test = 1 ;
													}
		else
			 if( memory[ dest_adr ] == 0 && !test ) FLAG.z = 1 ;
		FLAG.x = temp_x ;
		}
	}
/*------------------------------------------------------------*/

void set_flags( unsigned size , unsigned mode , unsigned dest_no ,  unsigned msb)
  {

	/* find msb for DATA ADDRESS MODE  */
	if( mode == 0 )
		switch( size ){
			case 0:  msb = D[ dest_no ].byte >> 7 ;
						break ;
			case 1:  msb = D[ dest_no ].word >> 15 ;
						break ;
			case 2:  msb = D[ dest_no ].dword >> 31 ;
						break ;
			}
  if( msb == 1 ) FLAG.n = 1 ;
  else           FLAG.n = 0 ;
  FLAG.v = msb ^ FLAG.x  ;
  FLAG.c = FLAG.x ;
  if( mode == 0 )
	  if( D[ dest_no ].dword == 0 )  FLAG.z = 1 ;
	  else                           FLAG.z = 0 ;
	}
/*--------------------------------------------------------*/
		  /*   if V = 0 , then 1s -> [ EA ]  else 0s -> [ EA ]  */
void SVC()
	{

	unsigned op_code ,_op_code , d8 , mode ,_mode , reg_no , _reg_no , _size ;
	register int loop , i;
	int d16  ;
	long int EA = 0 , Xn ;

	op_code =  memory[ ++PC ] ;          /* taking opcode bit pattern */
	mode = ( op_code  & 0x38 ) >> 3 ;    /* and breaking it in desired*/
	reg_no = op_code & 0x07 ;            /* field */
	EA = Calc_Effaddr25( mode , reg_no ) ;
	if( FLAG.v == 0 ) memory[ EA ] = 0xFF ; /* 1s->EA */
	else              memory[ EA ] = 0 ;    /* 0s->EA */
	PC++ ;
	}
/*-------------------------------------------------------------------*/

 int Calc_Effaddr25( unsigned mode , unsigned reg_no)
	{

	unsigned op_code ,_op_code , d8  , _mode ,  _reg_no , _size ;
	register int loop , i;
	int d16  ;
	long int EA = 0 , Xn ;

	switch( mode ){
		  case 0: EA = D[ reg_no ].dword ; /* DATA REGISTER DIRECT MODE */
					 break ;
		  case 2: EA = memory[ A[ reg_no ].dword ] ; /* ADDRESS REGISTER */
					 break ;                            /* INDIRECT MODE */
		  case 3: EA = memory[ A[ reg_no ].dword++ ] ; /* postincrement */
					 break ;
		  case 4: EA = memory[ --A[ reg_no ].dword ] ; /* predecriment  */
					 break ;
		  case 5: d16 = memory[ ++PC ] ;                 /* with displacment*/
					 d16 = ( d16 << 8 ) + memory[ ++PC ] ;
					 EA  = memory[ A[ reg_no ].dword ] + d16 ;
					 break ;
		  case 6: _op_code = memory[ ++PC ] ;          /* with index and */
					 _mode = _op_code >> 7 ;              /* displacment */
					 _reg_no = ( _op_code & 0x70 ) >> 4 ;
					 _size = ( _op_code & 0x08 ) >> 3 ;
					 if( _mode == 0 )
						 if( _size == 0 ) Xn = D[ _reg_no ].word ;
						 else             Xn = D[ _reg_no ].dword ;
					 else
							if( _size == 0 ) Xn = A[ _reg_no ].word ;
							else             Xn = A[ _reg_no ].dword ;
					 d8 = memory[ ++PC ] ;
					 EA = memory[ A[ reg_no ].dword ] + memory[ Xn ] + d8 ;
					 break ;
		  case 7: if ( reg_no == 0 ) loop = 1 ;   /* ABSOLUTE SHORT AND */
					 else                            /* LONG */
						  if( reg_no == 1 ) loop = 3 ;
					 for( i = loop ; i >= 0 ; i-- )
						 EA = ( EA << 8 ) + memory[ ++PC ] ;
					 break ;
		  default:
					 printf(" Error " ) ;
					 break ;
		  }
	 return ( EA & 0x0FFF ) ;
	 }

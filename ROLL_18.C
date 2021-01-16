#include<math.h>
#include "defina_n.h"
int pc_inc;
unsigned int SR;
unsigned char permited = 1;
unsigned char executed = 0;
unsigned char next = 0;
unsigned int instruction = 0;
/***************************************************************************/
void execute()
{
 executed = 1;
}
/***************************************************************************/
int mask_16(unsigned int x , char *ptr)
{
 int flag = 1;
 int i = 15;
 do{
    if(ptr[i] == 'x' || ptr[i] == 'X'){
       i--;
       x = x >> 1;
       continue;
    }
    if( ( ptr[i] == '1' ) || ( x & 1 ) )flag = 0;
    if( ( ptr[i] == '1' ) && ( x & 1 ) )flag = 1;
    x = x >> 1;
    i--;
 }while( i >= 0);
 return(  flag );
}
/***************************************************************************/
int binary_to_decimal(unsigned int x , int start , int digit)
{
 int value = 0 , i;

 x = x >> ( start - digit + 1 );
 for( i = 0 ; i < digit ; i++){
    if( x & 1 ) value = value + ( int ) pow( 2 , (float) i );
    x = x >> 1;
 }
 return ( value );
}
/***************************************************************************/
unsigned long int content_from_memory(unsigned long int start,int num_of_byte )
{ unsigned long int x = 0;
  int i;

 for( i = 0 ; i < num_of_byte ; i++ ){
    x = x << 8;
    x = x | (unsigned char )memory[ start + i ];
 }
return x;
}
/***************************************************************************/
void load_to_memory(unsigned long int address , unsigned long int content ,\
		    int num)
{int i;

 for( i = num - 1 ; i <= 0 ; i--){
    memory[address + i] = (unsigned char) content & 0x000000ff;
    content = content >> 8;
 }
}
/***************************************************************************/
unsigned long int Calc_Effaddr18(int mode , int reg)
{ unsigned long int EA = 0;
  int inc_mode;
  unsigned long int indx_reg_value;
  unsigned int indx_info;
  int indx;

  inc_mode = binary_to_decimal( instruction , 13 , 2 );
  switch( mode ){
   case 2 : EA = A[reg].dword;
	    if(permited) pc_inc = 2;
	    break;
   case 3 : EA = A[reg].dword;
	    switch( inc_mode ){
		case 1: A[reg].dword = A[reg].dword + 1;
			break;
		case 2: A[reg].dword = A[reg].dword + 4;
			break;
		default:A[reg].dword = A[reg].dword + 2;
			break;
	    }
	    if( permited ) pc_inc = 2;
	    break;
   case 4 : switch( inc_mode ){
		case 1: A[reg].dword = A[reg].dword - 1;
			break;
		case 2: A[reg].dword = A[reg].dword - 4;
			break;
		default:A[reg].dword = A[reg].dword - 2;
			break;
	    }
	    EA = A[reg].dword;
	    if( permited ) pc_inc = 2;
	    break;
   case 5 : EA = A[reg].dword + content_from_memory( PC + 2 + next , 2 );
	    if ( permited ) pc_inc  = 4;
	    break;
   case 6 : if( permited ) pc_inc = 4;
	    indx_info = content_from_memory( PC + 2 + next , 2 );
	    indx = binary_to_decimal( indx_info , 14 , 3 );
	    if ( indx_info & 0x8000 ){
		  indx_reg_value = A[ indx ].word ;
		  if( indx_info & 0x0800 )
		     indx_reg_value = A[ indx ]. dword;
	    }
	    else{
		  indx_reg_value = D[ indx ].word ;
		  if( indx_info & 0x0800 )
		     indx_reg_value = D[ indx ]. dword;
	    }
	    EA = indx_reg_value + A[ reg ].dword + (unsigned char) indx_info;
	    break;
   case 7 : switch( reg ){
	       case 0 : EA = content_from_memory( PC + 2 + next , 2 );
			if( permited ) pc_inc = 4;
			break;
	       case 1 : EA = content_from_memory( PC + 2 + next , 4 );
			if( permited ) pc_inc = 6;
			break;
	       case 2 : EA = PC + content_from_memory( PC + 2 + next , 2 );
			if( permited ) pc_inc  = 4;
			break;
	       case 3 : if ( permited ) pc_inc = 4;
			indx_info = content_from_memory( PC + 2 + next , 2 );
			indx = binary_to_decimal( indx_info , 14 , 3 );
			if ( indx_info & 0x8000 ){
			   indx_reg_value = A[ indx ].word ;
			   if( indx_info & 0x0800 )
			       indx_reg_value = A[ indx ]. dword;
			}
			else {
			   indx_reg_value = D[ indx ].word ;
			   if( indx_info & 0x0800 )
			       indx_reg_value = D[ indx ]. dword;
			}
			EA = indx_reg_value + PC + (unsigned char) indx_info;
			break;
	       case 4 : if( binary_to_decimal( instruction , 13 , 2 ) == 2 ){
				EA = content_from_memory( PC + 2 + next , 4 );
				if( permited ) pc_inc = 6;
			}
			else {
				EA = content_from_memory( PC + 2 + next , 2 );
				if( permited ) pc_inc = 4;
			}
			break;
	    }
	    break;
   }
return EA;
}
/***************************************************************************/
void move_to_flag( unsigned int value , int sys )
{
 if( value & 0x0001 )
   FLAG.c = 1;
 else
   FLAG.c = 0 ;
 if( value & 0x0002 )
   FLAG.v = 1;
 else
   FLAG.v = 0 ;
 if( value & 0x0004 )
   FLAG.z = 1;
 else
   FLAG.z = 0 ;
 if( value & 0x0008 )
   FLAG.n = 1;
 else
   FLAG.n = 0 ;
 if( value & 0x0010 )
   FLAG.x = 1;
 else
   FLAG.x = 0 ;
 if( sys ){
   value = value >> 8;
   FLAG.system = (unsigned char ) value ;
 }
}
/***************************************************************************/
unsigned int value_from_flag()
{
 unsigned int value = 0;

 value = FLAG.system;
 value = value << 8;
 if( FLAG.c == 1 )
    value = value | 0x0001;
 if( FLAG.v == 1 )
    value = value | 0x0002;
 if( FLAG.z == 1 )
    value = value | 0x0004;
 if( FLAG.n == 1 )
    value = value | 0x0008;
 if( FLAG.x == 1 )
    value = value | 0x0010;
return value;
}
/***************************************************************************/
void  move_ccr( unsigned instruction )
{
 int mode , reg;

 mode = binary_to_decimal( instruction , 5 , 3 );
 reg  = binary_to_decimal( instruction , 2 , 3 );
 switch( mode ){
    case 0 : move_to_flag( ( unsigned int ) D[reg].word , 0);
	     pc_inc = 2;
	     execute();
	     break;
    case 1 : break;
    default: if(( mode == 7 ) && ( reg == 4 )){
		 move_to_flag( ( unsigned int ) content_from_memory( PC + 2 ,\
		  2),0 );
		 pc_inc = 4;
	     }
	     else {
		 move_to_flag( (unsigned int) content_from_memory( Calc_Effaddr18 ( mode \
		 , reg ) , 2 ) ,0 );
	     }
	     execute();
	     break;
 }
PC = PC + pc_inc;
}
/***************************************************************************/
void  move_w_source_SR(unsigned instruction )
{
 int mode , reg;

 mode = binary_to_decimal( instruction , 5 , 3 );
 reg  = binary_to_decimal( instruction , 2 , 3 );
 switch( mode ){
    case 0 : move_to_flag( ( unsigned int ) D[reg].word , 1);
	     pc_inc = 2;
	     execute();
	     break;
    case 1 : break;
    default: if(( mode == 7 ) && ( reg == 4 )){
		 move_to_flag( ( unsigned int ) content_from_memory( PC + 2 ,\
		 2),1 );
		 pc_inc = 4;
	     }
	     else {
		 move_to_flag( (unsigned int) content_from_memory( Calc_Effaddr18 ( mode \
		 , reg ) , 2 ) , 1);
	     }
	     execute();
	     break;
 }
PC = PC + pc_inc;
}
/**************************************************************************/
void  move_w_SR_destin(unsigned instruction )
{
 int mode , reg;

 mode = binary_to_decimal( instruction , 5 , 3 );
 reg  = binary_to_decimal( instruction , 2 , 3 );
 switch( mode ){
    case 0 : D[reg].word = value_from_flag();
	     pc_inc = 2;
	     execute();
	     break;
    case 1 : break;
    default:  if(( mode == 7 ) && ( reg > 1 )) break;
	      load_to_memory( Calc_Effaddr18 ( mode , reg ) , value_from_flag()\
	      , 2 );
	      execute();
	      break;
 }
PC = PC + pc_inc;
}
/*******************************************************************************/
void  move_l_USP_destin(unsigned int instruction )
{
 int reg;
 if( binary_to_decimal( instruction , 5 , 3 ) == 1 ){
	 reg = binary_to_decimal( instruction , 2 , 3 );
	 A[reg].dword = D[7].dword;
	 PC = PC + 2;
	 execute();
 }
}
/*******************************************************************************/
void  move_l_source_USP( unsigned instruction )
{
 int reg;

 if( binary_to_decimal( instruction , 5 , 3 ) == 1 ){
	 reg = binary_to_decimal( instruction , 2 , 3 );
	 D[7].dword = A[reg].dword;
	 PC = PC + 2;
	 execute();
 }
}
/******************************************************************************/
void  movea_w_source_An( unsigned instruction )
{
 int mode , reg;
 mode = binary_to_decimal( instruction , 5 , 3 );
 reg  = binary_to_decimal( instruction , 2 , 3 );
 switch( mode ){
    case 0 : A[binary_to_decimal( instruction , 11 , 3 )].word  = D[reg].word;
	     pc_inc = 2;
	     break;
    case 1 : A[binary_to_decimal( instruction , 11 , 3 )].word  = A[reg].word;
	     pc_inc = 2;
	     break;
    default : if (( mode == 7 ) && ( reg == 4 )){
		     A[binary_to_decimal( instruction , 11 , 3 )].word  = \
		     ( unsigned int )content_from_memory( PC + 2 , 2 );
		     pc_inc = 4;
	      }
	      else {
		      A[binary_to_decimal( instruction , 11 , 3 )].word  = \
		      (unsigned int) content_from_memory( Calc_Effaddr18 ( mode \
		      , reg ) , 2 );
	      }
	      break;
 }
PC = PC + pc_inc;
execute();
}
/******************************************************************************/
void  movea_l_source_An( unsigned instruction )
{
 int mode , reg;
 mode = binary_to_decimal( instruction , 5 , 3 );
 reg  = binary_to_decimal( instruction , 2 , 3 );
 switch( mode ){
    case 0 : A[binary_to_decimal( instruction , 11 , 3 )].dword = D[reg].dword;
	     pc_inc = 2;
	     break;
    case 1 : A[binary_to_decimal( instruction , 11 , 3 )].dword = A[reg].dword;
	     pc_inc = 2;
	     break;
    default : if( (mode == 7) && (reg == 4 )){
		 A[binary_to_decimal( instruction , 11 , 3 )].dword  = \
		 ( unsigned long int )content_from_memory( PC + 2 , 4 );
		 pc_inc = 6;
	      }
	      else {
		 A[binary_to_decimal( instruction , 11 , 3 )].dword  = \
		 (unsigned long int) content_from_memory( Calc_Effaddr18 \
		 ( mode , reg ) , 4 );
	      }
	      break;
 }
PC = PC + pc_inc;
execute();
}
/***************************************************************************/
void moveb_EA_EA ( unsigned int instruction )
{
 unsigned long int  destin_addr = 0 ;
 int d_reg , s_reg , s_mode , d_mode;

 s_reg  = binary_to_decimal( instruction , 2 , 3 );
 s_mode = binary_to_decimal( instruction , 5 , 3 );
 d_reg  = binary_to_decimal( instruction , 11 , 3 );
 d_mode = binary_to_decimal( instruction , 8 , 3 );

 switch( d_mode ){
	case 0: switch( s_mode ){
			case 0:D[d_reg].byte = D[s_reg].byte;
			       pc_inc = 2;
			       break;
			case 1:D[d_reg].byte = (unsigned char)A[s_reg].word;
			       pc_inc = 2;
			       break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:if(( s_mode == 7 ) && ( s_reg == 4 ))
				      D[d_reg].byte = (unsigned char)\
				      Calc_Effaddr18( s_mode , s_reg );
				else {
				      D[d_reg].byte = ( unsigned char )\
				      content_from_memory( Calc_Effaddr18\
				      ( s_mode , s_reg ) , 1 );
				}
				break;
		 };
		 break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:if( s_mode == 0 ){
			load_to_memory( Calc_Effaddr18( d_mode , d_reg ) ,\
			D[s_reg].byte , 1 );
	       }
	       if ( s_mode == 1 ){
			load_to_memory( Calc_Effaddr18( d_mode , d_reg ) , \
			A[s_reg].word , 1 );
	       }
	       if (( s_mode < 5 ) && ( s_mode > 1 )){
			destin_addr = Calc_Effaddr18( d_mode , d_reg );
			permited = 0;
			load_to_memory( destin_addr , content_from_memory\
			( Calc_Effaddr18 ( s_mode , s_reg ) ,1 ) , 1 ) ;
			permited = 1;
	       }
	       if ((( s_mode > 4 ) && ( s_mode < 8 )) && !(( s_mode == 7 ) \
	       && ( s_reg == 4 ))){
			if ( d_mode > 4 ) {
				if((s_mode == 7) && ( s_reg == 1))
					next = 4;
				else
					next = 2;
				destin_addr = Calc_Effaddr18 ( d_mode , d_reg );
				next = 0;
			}
			else
				destin_addr = Calc_Effaddr18 ( d_mode , d_reg );

			load_to_memory ( destin_addr , content_from_memory(\
			Calc_Effaddr18( s_mode , s_reg ),1),1);
	       }
	       if (( s_mode == 7 ) && ( s_reg == 4 )){
			if( binary_to_decimal( instruction , 13 , 2 ) == 2 )
				next = 4;
			else
				next = 2;
			destin_addr = Calc_Effaddr18 ( d_mode , d_reg );
			next = 0;
			load_to_memory ( destin_addr , Calc_Effaddr18( s_mode \
			, s_reg ),1);
	       }
	       if ( ( s_mode > 4 ) && ( d_mode > 4 ) ) {
			pc_inc = pc_inc + 2;
			if ( ( d_mode == 7 ) && ( d_reg == 1 ) )
			       pc_inc = pc_inc + 2;
	       }
	       break;
 }
}
/***************************************************************************/
void movew_EA_EA ( unsigned  int instruction )
{
 int d_reg , s_reg , s_mode , d_mode;
 unsigned long int destin_addr = 0;

 s_reg  = binary_to_decimal( instruction , 2 , 3 );
 s_mode = binary_to_decimal( instruction , 5 , 3 );
 d_mode = binary_to_decimal( instruction , 8 , 3 );
 d_reg  = binary_to_decimal( instruction , 11 , 3 );

 switch( d_mode ){
	case 0: switch( s_mode ){
			case 0:D[d_reg].word = D[s_reg].word;
			       pc_inc = 2;
			       break;
			case 1:D[d_reg].word = A[s_reg].word;
			       pc_inc = 2;
			       break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:if(( s_mode == 7 ) && ( s_reg == 4 ))
				      D[d_reg].word = (unsigned int)\
				      Calc_Effaddr18( s_mode , s_reg );
				else {
				      D[d_reg].word = ( unsigned int )\
				      content_from_memory( Calc_Effaddr18\
				      ( s_mode , s_reg ) , 2 );
				}
				break;
		 };
		 break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:if( s_mode == 0 ){
			load_to_memory( Calc_Effaddr18( d_mode , d_reg ) ,\
			D[s_reg].word , 2 );
	       }
	       if ( s_mode == 1 ){
			load_to_memory( Calc_Effaddr18( d_mode , d_reg ) , \
			A[s_reg].word , 2 );
	       }
	       if (( s_mode < 5 ) && ( s_mode > 1 )){
			destin_addr = Calc_Effaddr18( d_mode , d_reg );
			permited = 0;
			load_to_memory( destin_addr , content_from_memory\
			( Calc_Effaddr18 ( s_mode , s_reg ) ,2 ) , 2 ) ;
			permited = 1;
	       }
	       if ((( s_mode > 4 ) && ( s_mode < 8 )) && !(( s_mode == 7 ) \
	       && ( s_reg == 4 ))){
			if ( d_mode > 4 ) {
				if((s_mode == 7) && ( s_reg == 1))
					next = 4;
				else
					next = 2;
				destin_addr = Calc_Effaddr18 ( d_mode , d_reg );
				next = 0;
			}
			else
				destin_addr = Calc_Effaddr18 ( d_mode , d_reg );
			load_to_memory ( destin_addr , content_from_memory(\
			Calc_Effaddr18( s_mode , s_reg ),2),2);
	       }
	       if (( s_mode == 7 ) && ( s_reg == 4 )){
			if( binary_to_decimal( instruction , 13 , 2 ) == 2 )
				next = 4;
			else
				next = 2;
			destin_addr = Calc_Effaddr18 ( d_mode , d_reg );
			next = 0;
			load_to_memory ( destin_addr , Calc_Effaddr18( s_mode \
			, s_reg ),2);
	       }
	       if ( ( s_mode > 4 ) && ( d_mode > 4 ) ) {
			pc_inc = pc_inc + 2;
			if ( ( d_mode == 7 ) && ( d_reg == 1 ) )
			       pc_inc = pc_inc + 2;
	       }
	       break;
 }
}
/***************************************************************************/
void movel_EA_EA ( unsigned int instruction )
{
 int d_reg , s_reg , s_mode , d_mode;
 unsigned long int destin_addr = 0;

 s_reg  = binary_to_decimal( instruction , 2 , 3 );
 s_mode = binary_to_decimal( instruction , 5 , 3 );
 d_mode = binary_to_decimal( instruction , 8 , 3 );
 d_reg  = binary_to_decimal( instruction , 11 , 3 );

 switch( d_mode ){
	case 0: switch( s_mode ){
			case 0:D[d_reg].dword = D[s_reg].dword;
			       pc_inc = 2;
			       break;
			case 1:D[d_reg].dword = (unsigned long int)A[s_reg].dword;
			       pc_inc = 2;
			       break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:if(( s_mode == 7 ) && ( s_reg == 4 ))
				      D[d_reg].dword = (unsigned long int)\
				      Calc_Effaddr18( s_mode , s_reg );
				else {
				      D[d_reg].dword = ( unsigned long int)\
				      content_from_memory( Calc_Effaddr18\
				      ( s_mode , s_reg ) , 4 );
				}
				break;
		 };
		 break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:if( s_mode == 0 ){
			load_to_memory( Calc_Effaddr18( d_mode , d_reg ) ,\
			D[s_reg].dword , 4 );
	       }
	       if ( s_mode == 1 ){
			load_to_memory( Calc_Effaddr18( d_mode , d_reg ) , \
			A[s_reg].dword , 4 );
	       }
	       if (( s_mode < 5 ) && ( s_mode > 1 )){
			destin_addr = Calc_Effaddr18( d_mode , d_reg );
			permited = 0;
			load_to_memory( destin_addr , content_from_memory\
			( Calc_Effaddr18 ( s_mode , s_reg ) ,4 ) , 4 ) ;
			permited = 1;
	       }
	       if ((( s_mode > 4 ) && ( s_mode < 8 )) && !(( s_mode == 7 ) \
	       && ( s_reg == 4 ))){
			if ( d_mode > 4 ) {
				if((s_mode == 7) && ( s_reg == 1))
					next = 4;
				else
					next = 2;
				destin_addr = Calc_Effaddr18 ( d_mode , d_reg );
				next = 0;
			}
			else
				destin_addr = Calc_Effaddr18 ( d_mode , d_reg );
			load_to_memory ( destin_addr , content_from_memory(\
			Calc_Effaddr18( s_mode , s_reg ),4),4);
	       }
	       if (( s_mode == 7 ) && ( s_reg == 4 )){
			if( binary_to_decimal( instruction , 13 , 2 ) == 2 )
				next = 4;
			else
				next = 2;
			destin_addr = Calc_Effaddr18 ( d_mode , d_reg );
			next = 0;
			load_to_memory ( destin_addr , Calc_Effaddr18( s_mode \
			, s_reg ),4);
	       }
	       if ( ( s_mode > 4 ) && ( d_mode > 4 ) ) {
			pc_inc = pc_inc + 2;
			if ( ( d_mode == 7 ) && ( d_reg == 1 ) )
			       pc_inc = pc_inc + 2;
	       }
	       break;
 }
}
/***************************************************************************/
/***************************************************************************/
void MOVE_CCR( void )
{
  permited = 1;
  executed = 0;
  next = 0;

 instruction = ( unsigned int ) content_from_memory( PC , 2 );
 if( mask_16( instruction , "0100010011xxxxxx" ) )
   move_ccr( instruction );
}
/***************************************************************************/
void MOVE_SR ( void )
{
  permited = 1;
  executed = 0;
  next = 0;

 instruction = ( unsigned int ) content_from_memory( PC , 2 );
 if( mask_16( instruction , "0100011011xxxxxx" ) )
   move_w_source_SR( instruction );
 if( mask_16( instruction , "0100000011xxxxxx" ) )
   move_w_SR_destin( instruction );
}
/***************************************************************************/
void MOVE_USP ( void )
{
  permited = 1;
  executed = 0;
  next = 0;

 instruction = ( unsigned int ) content_from_memory( PC , 2 );
 if( mask_16( instruction , "0100111001101xxx" ) )
   move_l_USP_destin( instruction );
 if( mask_16( instruction , "0100111001100xxx" ) )
   move_l_source_USP( instruction );
}
/***************************************************************************/
void MOVE ( void )
{
  permited = 1;
  executed = 0;
  next = 0;

 instruction = ( unsigned int ) content_from_memory( PC , 2 );
 if( mask_16( instruction , "0001xxxxxxxxxxxx" ) )
   moveb_EA_EA( instruction );
 if( mask_16( instruction , "0011xxxxxxxxxxxx" ) )
   movew_EA_EA( instruction );
 if( mask_16( instruction , "0001xxxxxxxxxxxx" ) )
   movel_EA_EA( instruction );
 }
/***************************************************************************/
void MOVEA ( void )
{
  permited = 1;
  executed = 0;
  next = 0;

 instruction = ( unsigned int ) content_from_memory( PC , 2 );
 if( mask_16( instruction , "0011xxx001xxxxxx" ) )
   movea_w_source_An( instruction );
 if( mask_16( instruction , "0010xxx001xxxxxx" ) )
   movea_l_source_An( instruction );
 if( ! executed )
   MOVE ();
}
/***************************************************************************/

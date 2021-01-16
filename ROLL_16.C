#include "defina_n.h"

/*char * error_msg[10]={
					  "succsessfull execution",
					  "address out of existing mmemory range",
					  "boundary is NOT even"
					 };*/
unsigned long int m_EA;
unsigned char m_inst_size,m_dest_reg,m_source_reg;
unsigned char find_mode(void);
void flag_for_NEG(unsigned char size,unsigned long int result,unsigned long int source);
void flag_for_NOT(unsigned char size,unsigned long int result);
unsigned char find_mode(void);
void m_effective_address(void);
void MULS(void);
void MULU(void);
void NBCD(void);
void NEG(void);/*NEG & negx */
void NOP(void);
void NOT(void);

void m_effective_address(void)

{
   unsigned long int ultemp;
   unsigned char mem,mode_no,reg_no;
   int temp;
   unsigned int utemp;
   m_dest_reg=memory[PC];
   m_dest_reg>>=1;
   m_dest_reg&=7;
   m_source_reg=memory[PC+1];
   m_source_reg&=7;
   mode_no=find_mode();
   switch(mode_no){
	 case 0:
	 case 1:
			 m_inst_size=2;
		   break;
	 case 2:
			m_EA=A[m_source_reg].dword;
			m_inst_size=2;
			break;
	 case 3:
			m_EA=A[m_source_reg].dword;
            m_inst_size=2;
			A[m_source_reg].dword+=2;
			break;
	 case 4:
			A[m_source_reg].dword-=2;
			m_EA=A[m_source_reg].dword;
            m_inst_size=2;
			break;
	 case 5:
	 case 9:
			m_inst_size=4;
			temp=memory[PC+2];
			temp<<=8;
			temp+=memory[PC+3];
			if (mode_no==5) m_EA=A[m_source_reg].dword+temp;
			else if (mode_no==9) m_EA=PC+temp;
			break;
	 case 6:
	 case 10:
			m_inst_size=4;
			temp=memory[PC+3];
			if (mode_no==6) m_EA=A[m_source_reg].dword+temp;
			else if (mode_no==10) m_EA=PC+temp;
			mem=memory[PC+2];
			reg_no=(mem<<1)>>5;
			if(mem & 0xF0 !=0){
			  if (mem & 0x08!=0) m_EA+=A[reg_no].dword;
			  else {
				temp=A[reg_no].dword;
				m_EA+=temp;
			  }
			}
			else {
			  if (mem & 0x08!=0) m_EA+=D[reg_no].dword;
			  else {
				temp=D[reg_no].dword;
				m_EA+=temp;
			  }
			}
			break;
	 case 7:
			m_inst_size=4;
			utemp=memory[PC+2];
			utemp<<=8;
			utemp+=memory[PC+3];
			temp=utemp;
			m_EA=temp;
			break;
	 case 8:
			m_inst_size=6;
			ultemp=memory[PC+2];
			m_EA=ultemp<<24;
			ultemp=memory[PC+3];
			m_EA+=ultemp<<16;
			ultemp=memory[PC+4];
			m_EA+=ultemp<<8;
			ultemp=memory[PC+5];
			m_EA+=ultemp;
			break;
	  default: break;
   }
}
unsigned char find_mode(void)
{
  unsigned char mode,reg;

  mode=memory[PC+1];
  reg=mode;
  reg&=7;
  mode&=0x38;
  mode>>=3;
  if(mode>=0 && mode<=6) return mode;
  if(mode==7) return (mode+reg);
}

void MULS(void)
{
  int mode_no;
  long int dest,source;
  unsigned int utemp;

  m_effective_address();
  utemp=memory[PC]<<8 + memory[PC+1];
  if(utemp & 0xC1C0 !=0xC1C0 ) return;
  dest=D[m_dest_reg].word;
  switch(find_mode()){
	case 0:
		   source=D[m_source_reg].word;
		   D[m_dest_reg].dword=source*dest;
		   break;
	case 1:
		   break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		  source=memory[m_EA]<<8+memory[m_EA+1];
		  D[m_dest_reg].dword=source*dest;
		  break;
	case 11:
		  m_inst_size=4;
		  source=memory[PC+2]<<8+memory[PC+3];
		  D[m_dest_reg].dword=source*dest;
		  break;
  }
  PC+=m_inst_size;
  if (D[m_dest_reg].dword==0) FLAG.z=1; else FLAG.z=0;
  FLAG.c=0;
  FLAG.v=0;
  if(D[m_dest_reg].dword & 0x80000000 !=0) FLAG.n=1; else FLAG.n=0;
}
void MULU(void)
{
  int mode_no;
  unsigned long int dest,source;
  unsigned int utemp;

  m_effective_address();
  utemp=memory[PC]<<8 + memory[PC+1];
  if(utemp & 0xC0C0 !=0xC0C0 ) return;
  dest=D[m_dest_reg].word;
  switch(find_mode()){
	case 0:
		   source=D[m_source_reg].word;
		   D[m_dest_reg].dword=source*dest;
		   break;
	case 1:
		   break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		  source=memory[m_EA]<<8+memory[m_EA+1];
		  D[m_dest_reg].dword=source*dest;
		  break;
	case 11:
		  m_inst_size=4;
		  source=memory[PC+2]<<8+memory[PC+3];
		  D[m_dest_reg].dword=source*dest;
		  break;
  }
  PC+=m_inst_size;
  if (D[m_dest_reg].dword==0) FLAG.z=1; else FLAG.z=0;
  FLAG.c=0;
  FLAG.v=0;
  if(D[m_dest_reg].dword & 0x80000000 !=0) FLAG.n=1; else FLAG.n=0;
}

void NBCD(void)
{
  unsigned int utemp;
  unsigned char temp;

  m_effective_address();
  utemp=memory[PC]<<8 + memory[PC+1];
  if(utemp & 0x4800 !=0x4800 ) return;
  switch(find_mode()){
	case 0:
		   temp=D[m_source_reg].dword;
		   D[m_source_reg].dword=100-FLAG.x-temp;
		   temp=D[m_source_reg].dword;
		   break;
	case 1:
		   break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		   memory[m_EA]=100-FLAG.x-memory[m_EA];
		   temp=memory[m_EA];
		   break;
	case 9:
	case 10:
	case 11:
		   break;
  }
  PC+=m_inst_size;
  if (temp==0) FLAG.z=1 ;else FLAG.z=0;
  if(temp>127) FLAG.c=1; else FLAG.c=0;
  FLAG.c=FLAG.x;
}
void NEG(void)
{
  unsigned long int ltemp,source;
  unsigned int utemp;
  unsigned char temp,factor,size,i;

  m_effective_address();
  temp=memory[PC];
  if(temp ==0x44 ) factor=0;
  else if (temp==0x40) factor=1;
  else return;
  size=memory[PC+1]>>6;
  switch(find_mode()){
	case 0:
		   switch(size){
			 case 0:
					temp=D[m_source_reg].dword;
					source=temp+FLAG.x*factor;
					D[m_source_reg].dword=-(temp+FLAG.x*factor);
					break;
			 case 1:
					source=D[m_source_reg].word+FLAG.x*factor;
					D[m_source_reg].dword=-(D[m_source_reg].word+FLAG.x*factor);
					break;
			 case 2:
					source=D[m_source_reg].dword+FLAG.x*factor;
					D[m_source_reg].dword=-(D[m_source_reg].dword+FLAG.x*factor);
					break;
			}
			flag_for_NEG(size,source,D[m_source_reg].dword);
	case 1:
		   break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
           switch(size){
			 case 0:
					source=memory[m_EA]+FLAG.x*factor;
					memory[m_EA]=-(memory[m_EA]+FLAG.x*factor);
					flag_for_NEG(size,source,memory[m_EA]);
					break;
			 case 1:
					utemp=memory[m_EA]<<8 + memory[m_EA+1];
					source=utemp+FLAG.x*factor;
					utemp=-utemp-FLAG.x*factor;
					memory[m_EA+1]=utemp;
					memory[m_EA]=utemp>>8;
					flag_for_NEG(size,source,utemp);
					break;
			 case 2:
					ltemp=memory[m_EA]<<24 + memory[m_EA+1]<<16+memory[m_EA+2]<<8+memory[m_EA];
					source=ltemp+FLAG.x*factor;
					ltemp=-ltemp-FLAG.x*factor;
					for(i=0;i<3;i++) memory[PC+3-i]=ltemp>>(i*8);
					flag_for_NEG(size,source,ltemp);
					break;
			}
	case 9:
	case 10:
	case 11:
		   break;
  }
  PC+=m_inst_size;
}
void flag_for_NEG(unsigned char size,unsigned long int result,unsigned long int source)
{
  int temp1;
  char temp2;
  long int temp3;
  switch(size){
		case 0:
			   temp2=result;
			   if(temp2<0) FLAG.n=1;else FLAG.n=0;
			   if(temp2==0) FLAG.z=1;else if(memory[PC]==44) FLAG.z=0;
			   if(memory[PC]==0x44){
				 if(temp2==0) FLAG.c=0 ; else FLAG.c=1;
			   }
			   else{
				 if(source>0) FLAG.c=1; else FLAG.c=0;
			   }
			   if(source==128) FLAG.v=1; else FLAG.v=0;
			   FLAG.x=FLAG.c;
			   break;
		case 1:
			   temp1=result;
			   if(temp1<0) FLAG.n=1;else FLAG.n=0;
			   if(temp1==0) FLAG.z=1;else if(memory[PC]==44) FLAG.z=0;
			   if(memory[PC]==0x44){
				 if(temp1==0) FLAG.c=0 ; else FLAG.c=1;
			   }
			   else{
				 if(source>0) FLAG.c=1; else FLAG.c=0;
			   }
			   if((int)source==0x7FFF) FLAG.v=1; else FLAG.v=0;
			   FLAG.x=FLAG.c;
			   break;
		 case 2:
			   temp3=result;
			   if(temp3<0) FLAG.n=1;else FLAG.n=0;
			   if(temp3==0) FLAG.z=1;else if(memory[PC]==44) FLAG.z=0;
			   if(memory[PC]==0x44){
				 if(temp3==0) FLAG.c=0 ; else FLAG.c=1;
			   }
			   else{
				 if(source>0) FLAG.c=1; else FLAG.c=0;
			   }
			   if((long int)source==0x7FFFFFFF) FLAG.v=1; else FLAG.v=0;
			   FLAG.x=FLAG.c;
			   break;
		}
	  }
void NOT(void)
{
  unsigned long int ltemp;
  unsigned int utemp;
  unsigned char temp,size,i;

  m_effective_address();
  temp=memory[PC];
  if(temp !=0x46 )  return;
  size=memory[PC+1]>>6;
  switch(find_mode()){
	case 0:
		   switch(size){
			 case 0:
					temp=D[m_source_reg].dword;
					D[m_source_reg].dword=~temp;
					break;
			 case 1:
					D[m_source_reg].dword=~(D[m_source_reg].word);
					break;
			 case 2:
					D[m_source_reg].dword=~(D[m_source_reg].dword);
					break;
			}
			flag_for_NOT(size,D[m_source_reg].dword);
	case 1:
		   break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
           switch(size){
			 case 0:
					memory[m_EA]=~(memory[m_EA]);
					flag_for_NOT(size,memory[m_EA]);
					break;
			 case 1:
					utemp=memory[m_EA]<<8 + memory[m_EA+1];
					utemp=~utemp;
					memory[m_EA+1]=utemp;
					memory[m_EA]=utemp>>8;
					flag_for_NOT(size,utemp);
					break;
			 case 2:
					ltemp=memory[m_EA]<<24 + memory[m_EA+1]<<16+memory[m_EA+2]<<8+memory[m_EA];
					ltemp=~ltemp;
					for(i=0;i<3;i++) memory[PC+3-i]=ltemp>>(i*8);
					flag_for_NOT(size,ltemp);
					break;
			}
	case 9:
	case 10:
	case 11:
		   break;
  }
  PC+=m_inst_size;
}
void flag_for_NOT(unsigned char size,unsigned long int result)
{
  int temp1;
  char temp2;
  long int temp3;
  switch(size){
		case 0:
			   temp2=result;
			   if(temp2<0) FLAG.n=1;else FLAG.n=0;
			   if(temp2==0) FLAG.z=1;else FLAG.z=0;
			   FLAG.c=0;
			   FLAG.v=0;
			   break;
		case 1:
			   temp1=result;
			   if(temp1<0) FLAG.n=1;else FLAG.n=0;
			   if(temp1==0) FLAG.z=1;else FLAG.z=0;
			   FLAG.c=0;
			   FLAG.v=0;
			   break;
		 case 2:
			   temp3=result;
			   if(temp3<0) FLAG.n=1;else FLAG.n=0;
			   if(temp3==0) FLAG.z=1;else FLAG.z=0;
			   FLAG.c=0;
			   FLAG.v=0;
			   break;
		}
	  }
void NOP(void)
{
  PC=PC+2;
}
/*main()
{
  unsigned long int i1=40000,i2=5;
  unsigned long int i;

  i=i1*i2;
  FLAG.c=1;
} */

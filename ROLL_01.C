#include "defina_n.h"

int		ByteToWord(char );
long int	WordToLong(int );
unsigned long int	EffAddr(char );
long int	GetData(long int ,char );
long int	GetDataReg(char ,char ,char );
void 		PutData(long int,char ,long int );
void		PutDataReg(char ,char ,char ,long int );
long int	GetExtended();
int			AddBcd(char ,char );
void		SetZrSgn(long int ,char );
void		SetCarOvr(long int,long int,char,char);
void ADD();
void ADDA();
void ADDI();
void ADDQ();
void ABCD();
void ADDX();


ByteToWord(char byte) /*Change Byte to Word for sign Extension*/
{
	if (byte & 0x80) return (byte | 0xff00);
		else return (byte & 0xff);
}

long int WordToLong(int word)  /*Change Word to Longword for sign Extension*/
{
	if (word & 0x8000) return (word | 0xffff0000);
		else return (word & 0xffff);
}

long int GetData(long int addr,char len)
{             /* read data from memory at specified length.Make Word */
			/*or LongWord from Byte*/
	long int data=0,temp = 0;

	switch(len) {
	case 0: return memory[addr]; /* byte */

	case 1: temp = memory[addr];
			data += (temp << 8);     /* word */
			data += memory[addr+1];
			return data;

	case 2: temp = memory[addr];
			data += (temp << 24);  /* long word */
			temp = memory[addr+1];
			data += (temp << 16);
			temp = memory[addr+2];
			data += (temp << 8);
			data += memory[addr+3];
	}
	return data;
}
void PutData(long int addr,char len,long int data)
{             /* write data to memory at specified length */

	switch(len) {
	case 0: memory[addr] = data; /* byte */
			break;

	case 1: memory[addr+1]  = data & 0x00ff;     /* word */
			memory[addr] = (data & 0xff00) >> 8;
			break;

	case 2: memory[addr+3] = data & 0x000000ff;  /* long word */
			memory[addr+2] = (data & 0x0000ff00) >> 8;
			memory[addr+1] = (data & 0x00ff0000) >> 16;
			memory[addr] = (data & 0xff000000) >> 24;
			break;
	}
}

long int GetDataReg(char type,char no,char len)
{	/* Get Data from register as required[Byte,Word,Long].The register may*/
			/*data or address type */
	if (type) {
		switch(len) {
			case 01: return WordToLong(A[no].word);
			case  2: return A[no].dword;
		}
	}
	else {
	switch(len) {
		case 00: return WordToLong(ByteToWord(D[no].byte));
		case 01: return WordToLong(D[no].word);
		case 02: return D[no].dword;
		}
	}
}

void	PutDataReg(char type,char no,char len,long int data)
{	/* Put data in address or data register*/
	if (type) {
		switch(len) {
			case 01: A[no].word = data;
					 break;
			case 02: A[no].dword = data;
					 break;
		}
	}
	else {
	switch(len) {
		case 00: D[no].byte = data;
				 break;
		case 01: D[no].word = data;
				 break;
		case 02: D[no].dword = data;
				 break;
		}
	}
}

long int GetExtended()/* for index register */
{
	long int Addr,addr;
	char high;

	if (!(memory[PC] & 0xf1)) { /* immediate data or not */
		switch((memory[PC+1] & 0xc0) >> 6) {
			case  0:
			case  1: addr = PC+4;  /* one word immediate data */
					 PC += 6;
					 break;
			case  2: addr = PC + 6; /* long word data */
					 PC += 8;
		}
	}
	else {
		addr = PC + 2;  /* no immediate data */
		PC += 4;
	}
	high = memory[addr];
	if (high & 0x80) {   /* data or address register */
		if (high & 8 )  /* word or longword */
			Addr = A[(high & 0x70) >> 4].dword;
		else
			Addr = WordToLong(A[(high & 0x70) >> 4].word);
		}
	else {
		if (high & 8 )  /* word or longword */
			Addr = D[(high & 0x70) >> 4].dword;
		else
			Addr = WordToLong(D[(high & 0x70) >> 4].word);
		}
	  return Addr + WordToLong(ByteToWord(memory[addr+1]));
	  /* sign extension */
}

unsigned long int EffAddr(char byte) /* calculate EFF address*/
{
	char mode,regno,len;
	unsigned long int addr,Addr;

	mode  = (byte & 0x38) >> 3;
	regno = byte & 7;
	len   = (byte & 0xc0) >> 6;
	switch(mode) {
	case 2: PC += 2;  /* address register indirect */
			return A[regno].dword;

	case 3: addr = A[regno].dword;  /* post increment */
			PC += 2;
			if (len) A[regno].dword += len*2;
			else A[regno].dword += 1;
			return addr;

	case 4:              /* pre decrement */
			if (len) A[regno].dword -= 2*len;
			else A[regno].dword -= 1;
			PC += 2;
			return (A[regno].dword);

	case 5: addr = A[regno].dword+WordToLong(GetData(PC+2,1));
			PC += 4;
			return addr;

	case 6:	addr = A[regno].dword; /* indexed mode*/
			return addr+GetExtended();

	case 7: switch(regno) {

			case 0: addr = WordToLong(GetData(PC+2,1));
					PC += 4;
					return addr;

			case 1: addr = GetData(PC+2,1);
					Addr = WordToLong(GetData(PC+4,1));
					PC += 6;
					return (addr<<16)+Addr;

			case 2: addr = WordToLong(GetData(PC+2,1));
					Addr = PC + addr;
					PC += 4;
					return Addr;

			case 3: Addr = GetExtended();
					addr = Addr + PC;
					return addr;

			case 4: addr = PC;
					PC += 2;
					if (len) PC += len*2;
					else PC += 1;
					return(addr+2);
			}
	}
}


void ADD()
{
	char  high,low,len;
	long int data1,data2,sum;
	unsigned long int eff;

	high = memory[PC]; /* higher byte of instruction */
	low = memory[PC+1]; /* lower byte of instruction */
	len = ( low & 0xc0) >> 6; /* length of operand */
	if ((low & 0x38) == 0){ /*check mode */
		 data1 = GetDataReg(0,low & 7,len);
		/* data register direct mode */
		PC += 2;
	}
	else if ((low & 0x38) == 8) {
			data1 = GetDataReg(1,low & 7,len);
			/* address register direct mode */
			PC += 2;
	}
	else {
		eff = EffAddr(low);
		data1 = GetData(eff,len);     /* read data from memory */
	}
	data2 = GetDataReg(0,(high &0x0e) >> 1,len); /* 2nd operand from register*/
	sum = data1 + data2;
	if (high & 1) {
			if ((low & 0x38) == 0) PutDataReg(0,(high &0x0e) >> 1,len,sum);
		/* data register direct mode */
	else if ((low & 0x38) == 8) PutDataReg(1,(high &0x0e) >> 1,len,sum);
			/* address register direct mode */
	else	PutData(eff,len,sum);
	}
		else PutDataReg(0,(high &0x0e) >> 1,len,sum);
    SetZrSgn(sum,(low & 0xc0)>>6);
	SetCarOvr(data1,data2,len,FLAG.x);
}

void ADDA()
{
	char  high,low,len;
	long int data1,data2,sum;
	unsigned long int eff;

	high = memory[PC];
	low = memory[PC+1];
	len = high & 1 + 1;
	if ((low & 0x38) == 0) {
		data1 = GetDataReg(0,low & 7,len);
		/* data register direct mode */
		PC += 2;
	}
	else if ((low & 0x38) == 8) {
			data1 = GetDataReg(1,low & 7,len);
			/* address register direct mode */
			PC += 2;
	}
	else {
		eff = EffAddr(low);
		data1 = GetData(eff,len);
	}
	data2 = GetDataReg(0,(high &0x0e) >> 1,len);
	sum = data1 + data2;
	PutDataReg(1,low & 7,len,sum);
    SetZrSgn(sum,(low & 0xc0)>>6);
	SetCarOvr(data1,data2,len,FLAG.x);
}

void ADDI()
{
	char  low,len;
	long int data1,data2,sum;
	unsigned long int eff;
	REG32 pc;

	low = memory[PC+1];
	pc = PC;
	len = ( low & 0xc0) >> 6;
	if ((low & 0x38) == 0) {
		data1 = GetDataReg(0,low & 7,len);
		/* data register direct mode */
		PC += 2;
		if (len==2)  PC +=4;
			else PC+=2;
	}
	else {
		eff = EffAddr(low);
		data1 = GetData(eff,len);
	}
	data2 = GetData(pc+2,len);
	sum = data1 + data2;
	if ((low & 0x38) == 0) PutDataReg(0,low & 7,len,sum);
		else	PutData(eff,len,sum);
    SetZrSgn(sum,(low & 0xc0)>>6);
	SetCarOvr(data1,data2,len,FLAG.x);
}

void ADDQ()
{
	char  low,len,high;
	long int data1,data2,sum;
    unsigned long int eff;

	high = memory[PC];
	low = memory[PC+1];
	len = ( low & 0xc0) >> 6;
	if ((low & 0x38) == 0) {
		data1 = GetDataReg(0,low & 7,len);
		/* data register direct mode */
		PC += 2;
	}
	else if ((low & 0x38) == 8) {
			data1 = GetDataReg(1,low & 7,len);
			/* address register direct mode */
			PC += 2;
	}
	else {
		eff = EffAddr(low);
		data1 = GetData(eff,len);

	}
	data2 = (high & 0x0e) >> 1;
	sum = data1 + data2;
	if ((low & 0x38) == 0) PutDataReg(0,low & 7,len,sum);
		/* data register direct mode */
	else if ((low & 0x38) == 8) PutDataReg(1,low & 7,len,sum);
			/* address register direct mode */
	else PutData(eff,len,sum);
	SetZrSgn(sum,(low & 0xc0)>>6);
	SetCarOvr(data1,data2,len,FLAG.x);
}

void ABCD() /*add BCD number.It can be only byte */
{
	char  high,low,regno;
	int data1,data2,sum;

	high = memory[PC];
	low = memory[PC+1];
	regno = (high & 0x0e) >> 1;
	if (low & 0x08 ) {
		A[low & 0x07].dword  -= 1;
		data1 = GetData(A[low & 0x07].dword,0);
		A[regno].dword -= 1;
		data2 = GetData(A[regno].dword,0);
		sum = AddBcd(data1,data2);
		PutData(A[regno].dword,0,sum);
	}
	else {
		data1 = D[low & 0x07].byte;
		data2 = D[regno].byte;
		sum = AddBcd(data1,data2);
		PutDataReg(0,regno,0,sum);
	}
	PC += 2;
}

AddBcd(char data1,char data2) /* Adjust result in BCD */
{
	char d1,d2,c;
	int	result;

	c = FLAG.x;
	d1 = data1 %  10 ; /* separate digits*/
	d2 = data2 %  10;
	result = (d1 + d2 + c) % 10; /*result for first digit*/
	c = (d1+d2+c) / 10;
	d1 = data1 / 10;
	d2 = data2  / 10;
	result += (((d1 + d2 + c) % 10) << 4);
	c = (d1+d2+c) / 10;
	if (result == 0) FLAG.z = 0; /*set flag*/
	FLAG.x = FLAG.c = c;
	return result;
}

void SetZrSgn(long int sum,char len) /*set ZERO or SIGN flag */
{
	switch(len) {
		case 0 : if (sum & 0x80) FLAG.n = 1;
					else FLAG.n = 0;
				 break;
		case 1 : if (sum & 0x8000) FLAG.n = 1;
					else FLAG.n = 0;
				  break;
		case 2 : if (sum & 0x80000000) FLAG.n = 1;
					else FLAG.n = 0;
	}
	if (sum) FLAG.z = 0;
		else FLAG.z = 1;
}

void SetCarOvr(long int data1,long int data2,char mode,char extnd)
{   /*Set CARRY or OVERFLOW flag. These are calculated using*/
	/* CARRY LOOK AHEAD conception*/
	long int mask = 1;
	register char N,i;

	struct {
	unsigned x 		: 1 ;		/* bit of first operand  */
	unsigned y 		: 1 ;		/* bit of 2nd operand */
	unsigned c0		: 1 ;		/* previous carry */
	unsigned c1		: 1 ;		/* present carry */
	unsigned unused : 4 ;		/* unused bits */
}Data;

	switch(mode){
		case 0: N = 8; break;
		case 1: N = 16; break;
		case 2: N = 32 ;break;
	}
	if (!extnd ) Data.c1 = 0;
		else Data.c1 = 1;
	for (i=0; i < N; i++) {
		Data.c0 = Data.c1;
		if (data1 & mask ) Data.x = 1;
			else Data.x = 0;
		if (data2 & mask) Data.y = 1;
			else Data.y = 0;
		mask <<= 1;
		Data.c1 = (Data.x & Data.y ) | ((Data.x | Data.y)&Data.c0);
	}
	FLAG.v = Data.c0 ^ Data.c1;
	FLAG.c = FLAG.x = Data.c1;
}

void ADDX() /*Add with x FLAG */
{
	char  high,low,regno,len;
	long int data1,data2,sum;

	high = memory[PC];
	low = memory[PC+1];
	regno = (high & 0x0e)>>1;
	len = (low & 0xc0)>>6;
	if (low & 0x08 ) {
		if (len) A[low & 0x07].dword -= 2*len;
		else A[low & 0x07].dword -= 1;
		data1 = GetData(A[low & 0x07].dword,len);
		if (len) A[regno].dword -= 2*len;
		else A[regno].dword -= 1;
		data2 = GetData(A[regno].dword,len);
		sum = data1+data2+FLAG.x;
		PutData(A[regno].dword,len,sum);
	}
	else {
		data1 = GetDataReg(0,low & 0x07,len);
		data2 = GetDataReg(0,regno,len);
		sum = data1+data2+FLAG.x;
		PutDataReg(0,regno,len,sum);
	}
	SetZrSgn(sum,len);
	SetCarOvr(data1,data2,len,FLAG.x);
	PC += 2;
}


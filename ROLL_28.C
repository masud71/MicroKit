/*********************************************************************

		MD. REZAUL HUQ  (MASUD)
		ROLL NO.  : 28
		INSTRUCTION NAME  :
				1.  UNLINK
				2.  TST
		CSE, 3rd YEAR.

**********************************************************************/


#include  "defina_n.h"

void UNLINK(void) ;
void TST(void)   ;
int func1_28(int temp) ;
int func2_28(int temp1[],int loop);
int func3_28(int temp1[])  ;

void UNLINK(void)   {
	int  by_wd_ln,i,j,sum,temp;

	temp=memory[PC + 1];
	by_wd_ln=temp/64;	/* 1=BYTE,3=WORD,2=LONG WORD      */
	PC += 2;
	if (by_wd_ln==1 || by_wd_ln==3)  {
		A[7].word = A[func1_28(temp)].word;
									/* LOAD VALUE OF A[0..8] REG IN USP  */
		A[7].word += 4;              	/* USP IS INCREMENTED BY 4 TO CONTAIN
						   THE VALUE OF USP PRIOR TO USING THE
						   LINK INSTRUCTION IS RESTORED.     */
	}
	else {
		A[7].dword = A[func1_28(temp)].dword;
									/* LOAD VALUE OF A[0..8] REG IN USP  */
		A[7].dword += 4;              	/* USP IS INCREMENTED BY 4 TO CONTAIN
						   THE VALUE OF USP PRIOR TO USING THE
						   LINK INSTRUCTION IS RESTORED.     */
	}
}

void TST(void)   {
	int sum[4],val=0;
	int mode,reg,temp,by_wd_ln;

	temp=memory[PC+1];

/***************	FINDING EFFECTIVE ADDRESS     ***************/

	by_wd_ln=temp/64;	/* 1=BYTE,3=WORD,2=LONG WORD      */
	mode=func1_28(temp/8);	/* OPCODE : 0100 1010 SSEE EEEE   */
	reg=func1_28(temp);
	switch(mode)  {
		case 0 :
			switch(by_wd_ln)  {
				case 1 : val=D[reg].byte;
					break;
				case 3 : val=D[reg].word;
					break;
				case 2 : val=D[reg].dword;
					break;
			}
			PC +=2;
			break;
		case 2 :
		case 3 :
		case 4 :
		case 5 :
		case 7 :
			if (mode == 5 || (mode==7 && reg==0)) {  	/* FETCHING DISPLACEMENT */
				PC +=2;
				sum[1] = memory[PC++];
				sum[0] = memory[PC++];
				val = func2_28(sum,2);
			}
			else
			if (mode ==7 && reg==1) {
				PC +=2;
				sum[3] = memory[PC++];
				sum[2] = memory[PC++];
				sum[1] = memory[PC++];
				sum[0] = memory[PC++];
				val = func2_28(sum,4);
			}
			else   PC += 2;
			switch(by_wd_ln)  {
						/* 1=BYTE,3=WORD,2=LONG WORD      */
				case 3: temp=A[reg].word;
					break;
				case 2: temp=A[reg].dword ;
					break;
			}
			temp=temp + val;
			if (mode ==7 && reg==2) {  /* FOR PC RELATIVE WITH
						      DISPLACEMENT  */
				sum[1] = memory[PC++];
				sum[0] = memory[PC++];
				temp = PC - 4 + func2_28(sum,2);
			}
			sum[1] = memory[temp];
			sum[0] = memory[temp+1];
			if(sum[1] > 127)
				val = func3_28(sum);
					/* NEG DATA STORED IN MEMORY*/
			else	val = func2_28(sum,2);
			if (mode ==3) {
				switch(by_wd_ln)  {
                                	/* 1=BYTE,3=WORD,2=LONG WORD      */
					case 3: A[reg].word += 2;
						break;
					case 2: A[reg].dword +=4;
						break;
				}
			}	else
			if (mode ==4) {
				switch(by_wd_ln)  {
                                	/* 1=BYTE,3=WORD,2=LONG WORD      */
					case 3: A[reg].word -= 2;
						break;
					case 2: A[reg].dword -=4;
						break;
				}
			}
	}
	FLAG.v  =0;		/* V AND C FLAGS ARE ALWAYS CLEARED TO 0. */
	FLAG.c  =0;
	if (val > 0)  {
		FLAG.n = 1;	/* N FLAG IS SET  TO 1  */
		FLAG.z = 0;     /* Z FLAG IS CLEARED TO 0  */
	}
	else if( val < 0)  {
		FLAG.n = 0;     /* N FLAG IS CLEARED TO 0  */
		FLAG.z = 0;     /* Z FLAG IS CLEARED TO 0  */
	}
	else  {
		FLAG.n = 1;	/* N FLAG IS SET  TO 1  */
		FLAG.z = 1;     /* Z FLAG IS SET  TO 1  */
	}
	PC++;
}

int func1_28(int temp)  {
	int i,j,sum;
	for(i=0 , j=1 , sum = 0; i<2 ; i++)  {
		sum += j*(temp%2);
		j *=2;
		temp /= 2;
	}
	return sum;
}

int func2_28(int temp1[],int loop)  {
	int i,j,k,temp,sum;

	for(k=0,j=1,sum=0; k<loop; k++)  {
		temp=temp1[k];
		for(i=0 ; i<8 && temp !=0 ; i++)  {
			sum += j*(temp % 2);
			j *=2;
			temp /= 2;
		}
	}
	return sum;
}

int func3_28(int temp1[])  {
	int i,j,k,sum,temp;

	if(temp1[0]>0)
		temp1[0]--;
	else temp1[1]--;
	for(k=0,j=1,sum=0; k<2; k++)  {
		temp=temp1[k];
		for(i=0 ; i<8 ;i++)  {
			if((temp%2)==0)
				sum += j;
			j *=2;
			temp /= 2;
		}
	}
	sum = -sum;
	return sum;
}

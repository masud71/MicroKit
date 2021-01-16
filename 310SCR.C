/* Screen control, display & disk i/o functions for Microkit Simulator.
				Progremmed By :
						Masud Karim Khan
						3rd Year, CSE
						Roll : 17			*/
#define MAIN

#include <alloc.h>
#include <ctype.h>
#include "defina_n.h"
#include <dos.h>
#include <graphics.h>
#include <stdio.h>
#include <string.h>

#define AW		6
#define BS 		8
#define BYTE 	unsigned char
#define DW   	4
#define ENTER 	13
#define ESC 	27
#define F2   	60
#define F3   	61
#define QUIT 	26

BYTE *fonts[96];
char list[17] = "1234567890ABCDEF";
int g_driver,g_mode,Ln_ht,Card,Ch_wd,Ab_lx,Ab_rx,Ab_ty,Ab_by,Color[16];

void button(int,int,int,int,char *);
void button_flat(int, int, int, int, BYTE);
void button_up(int, int, int, int, BYTE);
void ch_mode(int *,char *);
void clear(int ,int ,int ,int ,int );
void clr_ram(void);
 int confirm(char *,int);
void deposite(int mode,char *cont);
void display(char *,int);
void disp_regs(void);
 int getstring(int,int,char *);
void load(char *);
 int load_bit_font( void );
 int lswin(char,char *);
void mycursor(int);
void Outtextxy( int , int ,char *, int );
void save(char *);
void screen();
void setup();
void seven_seg(int,int,char);
 int Textwidth( BYTE *);

void main()
{
    union {
		int dt;
		char ch[2];
	}key;
	int d_or_a=0,pos;
	int lx,ty,rx,by;
	register int i;
	char ws=AW,edited=0,loaded=0,ss = 0;
	char content[AW+1] = "000000",fname[60],tfname[60],temp[12] ;

	setup();
	initgraph(&g_driver,&g_mode,"");
	screen();
    lx = Ab_lx + 18.5*Ch_wd;
	ty = Ab_ty + 9.5*Ln_ht;
	rx = Ab_lx + 38.5*Ch_wd;
	by = Ab_ty + 19.5*Ln_ht;
	do {
		key.dt = bioskey(0);
		if  (toupper(key.ch[0]) != 'S' && ss) {
			clear(lx,ty,rx,by,Color[8]);
			ss = 0;
		}
		if (key.ch[0]) {
			key.ch[0] = toupper(key.ch[0]);
			if (strchr(list,key.ch[0])) {      /* Check if an input */
				pos = strchr(list,key.ch[0]) - list;
				button_flat(Ab_rx-(20-3*(pos%4))*Ch_wd,Ab_ty+(5+2*(pos/4))*Ln_ht+1,Ab_rx-(18-3*(pos%4))*Ch_wd,Ab_ty+(6.5+2*(pos/4))*Ln_ht-1,2);
				delay(150);
				button_up(Ab_rx-(20-3*(pos%4))*Ch_wd,Ab_ty+(5+2*(pos/4))*Ln_ht+1,Ab_rx-(18-3*(pos%4))*Ch_wd,Ab_ty+(6.5+2*(pos/4))*Ln_ht-1,2);
				/* right entry of the new input */
				for (pos=1;pos<ws;pos++) content[pos-1] = content[pos];
				content[ws-1] = key.ch[0];
				display(content,d_or_a);   /* diplay content to proper location */
				if (d_or_a) edited = 1;
			}
			else
				switch (key.ch[0]) {
					case 'P' : deposite(d_or_a,content); break;
					case 'R' :  button_flat(Ab_rx-8*Ch_wd,Ab_ty+7*Ln_ht+1,Ab_rx-3*Ch_wd-1,Ab_ty+8.5*Ln_ht-1,2);
								delay(150);
								button_up(Ab_rx-8*Ch_wd,Ab_ty+7*Ln_ht+1,Ab_rx-3*Ch_wd-1,Ab_ty+8.5*Ln_ht-1,2);
								run();
                                if (PC > 0xffffff) PC = PC % 0xc00;
								sprintf(temp,"%06X",PC);
								display(temp,0);
								if (! d_or_a) {
									for (i=0;i< AW;i++) content[i] = temp[i];
									content[i] = 0x0;
								}
								sprintf(temp,"%02X%02X",memory[PC%MAXMEM],memory[(PC+1)%MAXMEM]);
										  /* load data of current location to cont     */
								display(temp,1);
								if (d_or_a) {
									for (i=0;i< DW;i++) content[i] = temp[i];
									content[i] = 0x0;
								}
								break;
					case 'S' : button_flat(Ab_rx-20*Ch_wd,Ab_ty+13*Ln_ht+1,Ab_rx-12*Ch_wd,Ab_ty+14.5*Ln_ht-1,2);
								delay(150);
								button_up(Ab_rx-20*Ch_wd,Ab_ty+13*Ln_ht+1,Ab_rx-12*Ch_wd,Ab_ty+14.5*Ln_ht-1,2);
								single_step();
                               	if (PC > 0xffffff) PC = PC % 0xc00;
								sprintf(temp,"%06X",PC);
								display(temp,0);
								if (! d_or_a) {
									for (i=0;i< AW;i++) content[i] = temp[i];
									content[i] = 0x0;
								}
								sprintf(temp,"%02X%02X",memory[PC%MAXMEM],memory[(PC+1)%MAXMEM]);
										  /* load data of current location to cont     */
								display(temp,1);
								if (d_or_a) {
									for (i=0;i< DW;i++) content[i] = temp[i];
									content[i] = 0x0;
								}
								if (!ss) {
                                    clear(lx,ty,rx,by,Color[0]);
									setcolor(Color[15]);
									rectangle(lx+2,ty+2,rx-2,by-2);
									Outtextxy(lx+Ch_wd,ty+0.5*Ln_ht,"Data Reg",Color[15]);
									Outtextxy(lx+11*Ch_wd,ty+0.5*Ln_ht,"Addr Reg",Color[15]);
									ss = 1;
								}
								clear(lx+Ch_wd,ty+2*Ln_ht,rx-Ch_wd,by-3,Color[0]);
								for (i = 0;i < 8;i++) {
									sprintf(temp,"%08X",D[i]);
									Outtextxy(lx+Ch_wd,ty+(2+i)*Ln_ht,temp,Color[15]);
									sprintf(temp,"%08X",A[i]);
									Outtextxy(lx+11*Ch_wd,ty+(2+i)*Ln_ht,temp,Color[15]);
								}
								break;
					case 'T' : button_flat(Ab_rx-11*Ch_wd,Ab_ty+13*Ln_ht+1,Ab_rx-3*Ch_wd-1,Ab_ty+14.5*Ln_ht-1,2);
							delay(150);
							button_up(Ab_rx-11*Ch_wd,Ab_ty+13*Ln_ht+1,Ab_rx-3*Ch_wd-1,Ab_ty+14.5*Ln_ht-1,2);
							if (edited)
								if (!confirm(fname,loaded)) break;
							clr_ram();
                            strcpy(content,"000000");
							loaded = 0;
							edited = 0;
							break;
					case 'G' :button_flat(Ab_rx-8*Ch_wd,Ab_ty+9*Ln_ht+1,Ab_rx-3*Ch_wd-1,Ab_ty+10.5*Ln_ht-1,2);
							delay(150);
							button_up(Ab_rx-8*Ch_wd,Ab_ty+9*Ln_ht+1,Ab_rx-3*Ch_wd-1,Ab_ty+10.5*Ln_ht-1,2);
							disp_regs();break;
					case 'M' : ch_mode(&d_or_a,content);
							   if (d_or_a) ws = 4;
							   else ws = 6;
							   break;
					case QUIT: if (edited)
								if (!confirm(fname,loaded)) key.ch[0] = 0;
								break;
				}
		}
		else
			switch (key.ch[1]) {
				case F2 : if (!loaded) pos = lswin(0,fname);
						else pos = ENTER;
						if (pos == ENTER) {
							save(fname);
							edited = 0;
							loaded = 1;
						}
						break;
				case F3 : strcpy(tfname,fname);
						pos = lswin(1,fname);
						if (pos == ESC) {
							strcpy(fname,tfname);
							break;
						}
						if (edited)
							if (!confirm(tfname,loaded)) break;
						load(fname);
						loaded = 1;
						edited = 0;
                        if (d_or_a)
                           sprintf(content,"%02X%02X",memory[PC],memory[PC+1]);
                        else strcpy(content,"000000");
						break;
			}
	} while (key.ch[0] != QUIT);
	closegraph();
}

void button(int lx,int ty,int rx,int by,char *text)
/* Draw a button containing text in specified location */
{
	int color,offset;
	struct fillsettingstype fs;

	color = getcolor();
	getfillsettings(&fs);
	offset = (Card == 50 || Card == 70) ? 5 : 8;
	setfillstyle(SOLID_FILL,Color[8]);
	setcolor( Color[0] );
	rectangle(lx,ty,rx,by);
	bar(lx+1,ty+1,rx-1,by-1);
	/* Write the text to the midpoint of the button */
	Outtextxy(lx+(rx-lx-Textwidth(text))/2,ty+offset,text,BLACK);
	/* Give shed to make the button high */
	setcolor(Color[7]);
	line(lx+2,by-2,rx-2,by-2);
	line(rx-2,ty+2,rx-2,by-2);
	line(lx+1,by-1,rx-1,by-1);
	line(rx-1,ty+1,rx-1,by-1);
	setcolor(Color[15]);
	line(lx+2,ty+2,rx-2,ty+2);
	line(lx+2,ty+2,lx+2,by-2);
	line(lx+1,ty+1,rx-1,ty+1);
	line(lx+1,ty+1,lx+1,by-1);
	setcolor(color);
	setfillstyle(fs.pattern,fs.color);
}

void button_flat( int x1, int y1, int x2, int y2 , BYTE code)
{
	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	setcolor( Color[8] );
	line( x1 + 1, y1, x2 - 1, y1);
	line( x1, y1, x1, y2-1);
	if( code & 2 ) {
		line( x1 + 2, y1+1, x2 - 2, y1+1);
		line( x1+1, y1 +1, x1+1, y2-2);
	}
	line( x1 , y2, x2, y2);
	line( x2, y1, x2, y2-1);
	if( code ) {
		line( x1 + 1, y2-1, x2 - 1, y2-1);
		line( x2-1, y1 +1, x2-1, y2-2);
	}
}

void button_up( int x1, int y1, int x2, int y2 , BYTE code )
/* 	code 0 : one line border,
	code 1 : one line in upper side and two line in lower side,
	code 2 : two line border */
{
	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	setcolor( Color[15] );
	line( x1 + 1, y1, x2 - 1, y1);
	line( x1, y1, x1, y2-1);
	if( code & 2 ) {
		line( x1 + 2, y1+1, x2 - 2, y1+1);
		line( x1+1, y1 +1, x1+1, y2-2);
	}
	setcolor( Color[7] );
	line( x1 , y2, x2, y2);
	line( x2, y1, x2, y2-1);
	if( code ) {
		line( x1 + 1, y2-1, x2 - 1, y2-1);
		line( x2-1, y1 +1, x2-1, y2-2);
	}
}

void ch_mode(int *mode,char *cont)
/* Changes input modes ; from data to address & address to data.
	No global variable used
	Subroutine called : button_flat, button_up
*/
{
	button_flat(Ab_rx-8*Ch_wd,Ab_ty+11*Ln_ht+1,Ab_rx-3*Ch_wd-1,Ab_ty+12.5*Ln_ht-1,2);
	delay(150);
	button_up(Ab_rx-8*Ch_wd,Ab_ty+11*Ln_ht+1,Ab_rx-3*Ch_wd-1,Ab_ty+12.5*Ln_ht-1,2);
	if (*mode) {
		/* if in data mode load the address to the cont variable */
		sprintf(cont,"%06X",PC);   /* from memory pointer */
		setcolor(Color[8]);
		rectangle(Ab_lx+3*Ch_wd,Ab_ty+9*Ln_ht,Ab_lx+18*Ch_wd,Ab_ty+14*Ln_ht);
		setcolor(Color[6]);
		rectangle(Ab_lx+3*Ch_wd,Ab_ty+4*Ln_ht,Ab_lx+24*Ch_wd,Ab_ty+9*Ln_ht);
	}
	else {
		sscanf(cont,"%X",&PC);
		sprintf(cont,"%02X%02X",memory[PC%MAXMEM],memory[(PC+1)%MAXMEM]);
			  /* load data of current location to cont     */
		display(cont,1);
		setcolor(Color[8]);
		rectangle(Ab_lx+3*Ch_wd,Ab_ty+4*Ln_ht,Ab_lx+24*Ch_wd,Ab_ty+9*Ln_ht);
		setcolor(Color[6]);
		rectangle(Ab_lx+3*Ch_wd,Ab_ty+9*Ln_ht,Ab_lx+18*Ch_wd,Ab_ty+14*Ln_ht);
	}
	*mode = !(*mode);
}

void clear(int lx,int ty,int rx,int by,int color)
/* fills an aria by specified color */
{
	struct fillsettingstype fs;

	getfillsettings(&fs);
	setfillstyle(SOLID_FILL,color);
	bar(lx,ty,rx,by);
	setfillstyle(fs.pattern,fs.color);
}

void clr_ram()
{
	register int i;

	for (i=0;i<MAXMEM;i++) memory[i] = 0;
    PC = 0;
    clear(Ab_lx+4*Ch_wd,Ab_ty+5*Ln_ht,Ab_lx+23*Ch_wd,Ab_ty+8*Ln_ht,Color[0]);
	for (i=0;i<AW;i++)
		seven_seg(Ab_lx+(5+3*i)*Ch_wd,Ab_ty+5.5*Ln_ht,63);
	clear(Ab_lx+4*Ch_wd,Ab_ty+10*Ln_ht,Ab_lx+17*Ch_wd,Ab_ty+13*Ln_ht,Color[0]);
	for (i=0;i<DW;i++)
		seven_seg(Ab_lx+(5+3*i)*Ch_wd,Ab_ty+10.5*Ln_ht,63);
}

int confirm(char *filename,int loaded)
{
	void *dummy;
	int pos;

	dummy = (void *)malloc(imagesize(Ab_lx+10*Ch_wd,Ab_by-5*Ln_ht,Ab_lx+45*Ch_wd,Ab_by-3.5*Ln_ht));
	if (dummy) getimage(Ab_lx+10*Ch_wd,Ab_by-5*Ln_ht,Ab_lx+45*Ch_wd,Ab_by-3.5*Ln_ht,dummy);
	else return 0;
	setcolor(Color[0]);
	rectangle(Ab_lx+10*Ch_wd,Ab_by-5*Ln_ht,Ab_lx+45*Ch_wd,Ab_by-3.5*Ln_ht);
	Outtextxy(Ab_lx+11*Ch_wd,Ab_by-4.5*Ln_ht,"Current File is not saved.Save (Y/N)",Color[0]);
	pos = toupper(getch());
	if (!pos) pos = getch();
	putimage(Ab_lx+10*Ch_wd,Ab_by-5*Ln_ht,dummy,COPY_PUT);
	free(dummy);
	if (pos == ESC) return 0;
	if (pos != 'N') {
		if (loaded) pos = ENTER;
		else pos = lswin(0,filename);
		if (pos == ENTER) save(filename);
		else return 0;
	}
	return 1;
}


void deposite(int mode,char *cont)
{
	char temp[7]={0x00,0x00,0x00};
	int dump;

	button_flat(Ab_rx-8*Ch_wd,Ab_ty+5*Ln_ht+1,Ab_rx-3*Ch_wd-1,Ab_ty+6.5*Ln_ht-1,2);
	delay(150);
	button_up(Ab_rx-8*Ch_wd,Ab_ty+5*Ln_ht+1,Ab_rx-3*Ch_wd-1,Ab_ty+6.5*Ln_ht-1,2);
	if (!mode) return;
	temp[0] = cont[0];
	temp[1] = cont[1];
	sscanf(temp,"%x",&dump);
	memory[PC%MAXMEM] = dump;
	temp[0] = cont[2];
	temp[1] = cont[3];
	sscanf(temp,"%2x",&dump);
	memory[(PC+1)%MAXMEM] = dump;
	PC +=2;
	if (PC > 0xffffff) PC = PC % 0xc00;
	sprintf(temp,"%06X",PC);
	display(temp,0);
	sprintf(cont,"%02X%02X",memory[PC%MAXMEM],memory[(PC+1)%MAXMEM]);
			  /* load data of current location to cont     */
	display(cont,1);
}

void display(char *str,int mode)
/* Converts a hex coded string to seven segment display. Display the digits
at appropriate location with appropriate no. of nibbles accoring to the mode.
mode = 0 indicates address aria & mode = 1 indicates data aria.
	No  global variable used.
	Subroutine called : seven_seg
*/
{
	int i,pos,ws;
	char table[16]={6,91,79,102,109,125,7,127,111,63,119,124,57,94,121,113};

	if (mode) ws = DW;    /* if data mode then ws = 4 nibble    */
	else ws = AW;         /* if address mode then ws = 6 nibble */
	for (i=0;i<ws;i++) {
		pos = strchr(list,str[i]) - list;
		seven_seg(Ab_lx+(5+3*i)*Ch_wd,Ab_ty+(5.5+5*mode)*Ln_ht,table[pos]);
	}
}

void disp_regs(void)
{
	int lx,ty,rx,by,color;
	register int i;
	char dump[20];
	void *dummy;

	lx = Ab_lx + Ch_wd;
	rx = Ab_rx - Ch_wd;
	ty = Ab_by - 6.5*Ln_ht;
	by = Ab_by - 0.5*Ln_ht;
	dummy = (void *)malloc(imagesize(lx,ty,rx,by));
	if (dummy) getimage(lx,ty,rx,by,dummy);
	else return;
	clear(lx,ty,rx,by,Color[0]);
	color = getcolor();
	setcolor(Color[15]);
	rectangle(lx+2,ty+2,rx-2,by-2);
	for (i=0;i<8;i++) {
		sprintf(dump,"D[%d]= %08X",i,D[i]);
		Outtextxy(lx+(((rx-lx)/(4*Ch_wd))*(i%4)+1)*Ch_wd,ty+(i/4 + 0.5)*Ln_ht,dump,Color[15]);
	}
    for (i=0;i<8;i++) {
		sprintf(dump,"A[%d]= %08X",i,A[i]);
		Outtextxy(lx+(((rx-lx)/(4*Ch_wd))*(i%4)+1)*Ch_wd,ty+(i/4 + 3.5)*Ln_ht,dump,Color[15]);
	}
	getch();
	putimage(lx,ty,dummy,COPY_PUT);
	free(dummy);
	setcolor(color);
}

int getstring(int x,int y,char *str)
/* Take input of a string from keyboard with echo */
{
	char ch[2]=" ",temp;
	int i;

	i = strlen(str);
	do {
		moveto(x,y);
		mycursor(Color[0]);
		temp = getch();
		mycursor(Color[8]);
		if (temp>=32 && temp<=127) {
			ch[0] = temp;
			Outtextxy(getx(),gety(),ch,getcolor());
			str[i++]=ch[0];
			x += Textwidth(ch);
		}
		else if (temp==BS && i > 0) {
			ch[0] = str[i-1];
			x -= Textwidth(ch);
			clear(x,y,x+Textwidth(ch),y+Ln_ht-1,Color[8]);
			i--;
		}
		if (!temp) temp = getch();
	}while(temp !=ENTER && temp !=ESC);
	str[i]='\0';
	return temp;
}

void load(char *fname)
{
	FILE *fp;
	char dummy[22];
	int temp,done=0;
	register int i;

	fp = fopen(fname,"rb");
	if (!fp) return;
	fgets(dummy,22,fp);
	if (strcmp(dummy,"MC68000 machine code.")) return;
	getc(fp);
	getw(fp);
	clr_ram();
	for (;!done;) {
		i = getw(fp);
		for (;i<MAXMEM && !done;i++) {
			temp = getw(fp);
			if (temp == EOF) done = 1;
			else memory[i] = temp;
			if (memory[i] == 0) break;
		}
	}
	fclose(fp);
    sprintf(dummy,"%02X%02X",memory[PC%MAXMEM],memory[(PC+1)%MAXMEM]);
	display(dummy,1);
}

int load_bit_font( void )
{
	FILE *fp;
	char tot, str[14];
	int i;

	if( (fp=fopen( "FONT.BIT", "rb")) == NULL ) return 0;
	fgets( str, 14, fp );
	if( strcmp( str, "310 FONT FILE")) return 0;
	for( i = 0; i < 96; i++ ) {
		tot = getc( fp );
		if( !tot ) continue;
		if( (fonts[i] = (unsigned char *) malloc( tot )) == NULL ) return 0;
		fonts[i][0] = tot;
		fread( &fonts[i][1], 1, tot-1, fp);
		if( ferror(fp) ) {
			fclose(fp);
			return 0;
		}
	}
	fclose( fp );
	return 1;
}

int lswin(char op,char *fname)
{
	int lx,ty,rx,by,color,temp;
	register int i,j;
	char tempd[40],ch;
    union {
		int dt;
		char ch[2];
	}key;
	void *dummy;

	if (op) strcpy(tempd,"Load File Name");
	else strcpy(tempd,"Save File Name");
	lx = Ab_lx + 5*Ch_wd;
	rx = Ab_lx + 39*Ch_wd;
	ty = Ab_by - 6*Ln_ht;
	by = ty + 2.5*Ln_ht;
	dummy = (void *)malloc(imagesize(lx,ty,rx,by));
	if (dummy) getimage(lx,ty,rx,by,dummy);
	color = getcolor();
	setcolor(Color[0]);
	rectangle(lx,ty+0.5*Ln_ht,rx,by);
	temp = Textwidth(tempd);
	clear((lx+rx-temp)/2,ty,(lx+rx+temp)/2,ty+Ln_ht,Color[8]);
	Outtextxy((lx+rx-temp)/2,ty,tempd,Color[0]);
	getcurdir(0,tempd);
	if (op) sprintf(fname,"%c:\\%s\\*.MC",getdisk()+'A',tempd);
	else sprintf(fname,"%c:\\%s\\NONAME.MC",getdisk()+'A',tempd);
	if (strlen(fname) <25) {
    	i = lx+Ch_wd+Textwidth(fname);
		clear(lx+0.5*Ch_wd,ty+Ln_ht-1,i,ty+2*Ln_ht-2,Color[0]);
		Outtextxy(lx+Ch_wd,ty+Ln_ht+1,fname,Color[15]);
		strcpy(tempd,fname);
	}
	else {
		for (i=strlen(fname);fname[i] !='\\';i--);
		for (i=i-1;fname[i] !='\\';i--);
		for(j=0;j<3;j++) tempd[j] = fname[j];
		tempd[3] = tempd[4] = tempd[5] = '.';
		for (j=6;fname[i];j++,i++) tempd[j] = fname[i];
		tempd[j] = 0x0;
		i = lx+Ch_wd+Textwidth(tempd);
		clear(lx+0.5*Ch_wd,ty+Ln_ht-1,i,ty+2*Ln_ht-2,Color[0]);
		Outtextxy(lx+Ch_wd,ty+Ln_ht+1,tempd,Color[15]);
	}
	j = ty+Ln_ht+1;
	do {
		key.dt = bioskey(0);
		if ((key.ch[0] >=32 && key.ch[0] <= 127) || key.ch[0] == BS) {
			if (key.ch[0] == BS) {
				clear(lx+0.5*Ch_wd,ty+Ln_ht-1,i,ty+2*Ln_ht-2,Color[8]);
				Outtextxy(lx+Ch_wd,ty+Ln_ht+1,tempd,Color[0]);
			}
			else {
				fname[0] = 0x0;
				clear(lx+0.5*Ch_wd,ty+Ln_ht-1,i,ty+2*Ln_ht-2,Color[8]);
                i = lx + Ch_wd;
			}
			ungetch(key.ch[0]);
			ch = getstring(i,j,fname);
			key.ch[0] = ch;
		}
	} while(key.ch[0] != ENTER && key.ch[0] != ESC);
	putimage(lx,ty,dummy,COPY_PUT);
	free(dummy);
	setcolor(color);
	return key.ch[0];
}

void mycursor(int color)
/* Draw a cursor during string input */
{
	int x,y,tempc;

	tempc = getcolor();
	setcolor(color);
	x = getx();
	y = gety();
	line(x,y+Ln_ht-1,x+Ch_wd-1,y+Ln_ht-1);
	line(x,y+Ln_ht,x+Ch_wd-1,y+Ln_ht);
	setcolor(tempc);
}

void Outtextxy( int x, int y , char *s, int clr)
{
	register BYTE bite, t;
	char ch;
	int  height, width, i, j;

	while( *s ) {
		if( ( *s > 127 ) || ( *s < 32 ) ) ch = 0;
			else ch = *s - 32;
		height = y + fonts[ch][1];
		width = x + fonts[ch][2];
		bite = 3;
		t = 128;
		for( j = y; j < height; j++)
			for( i = x; i < width ; i++ ) {
				if( fonts[ch][bite] & t ) putpixel( i, j, clr);
				t >>= 1;
				if( !t ) {
					t = 128;
					bite++;
				}
			}
		x = width + 1;
		s++;
	}
}

void save(char *fname)
{
	FILE *fp;
	register int i=0;

	fp = fopen(fname,"wb");
	if (!fp) return;
	fputs("MC68000 machine code.",fp);
	putc(26,fp);
	do {
		for (;i<MAXMEM && !memory[i];i++);
		putw(0,fp);
		putw(i,fp);
		for (;i<MAXMEM && memory[i];i++) putw(memory[i],fp);
	} while (i<MAXMEM);
	fclose(fp);
}

void screen(void)
/* Draws the display screen of the MicroKit */
{
	int i, j;
	int lx,ty,rx,by;
	char my_pat[8] = {0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA};
	char tmp[2]={0x00,0x00},strings[4][5] ={"deP","Run","reG","Mode"};

	lx = Ab_lx;
	ty = Ab_ty;
	rx = Ab_rx;
	by = Ab_by;
	setcolor( Color[9] );
	if (Card==50 || Card==70) setfillpattern(my_pat,1);
	else setfillstyle( SOLID_FILL, Color[15]);
	bar3d(lx,ty,rx,by,0,0);
	setfillstyle(SOLID_FILL,Color[8]);
	lx+=3;rx-=3;ty+=2;by-=2;
	bar3d(lx,ty,rx,by,0,0);
	for (i = 0;i<4;i++)
		for (j=0;j<4;j++) {     /* draw the input buttons */
			tmp[0] = list[i*4+j];
			button(Ab_rx-(20-3*j)*Ch_wd-1,Ab_ty+(5+2*i)*Ln_ht,Ab_rx-(18-3*j)*Ch_wd+1,Ab_ty+(6.5+2*i)*Ln_ht,tmp);
		}
	for (i = 0;i<4;i++)
		button(Ab_rx-8*Ch_wd-1,Ab_ty+(5+2*i)*Ln_ht,Ab_rx-3*Ch_wd,Ab_ty+(6.5+2*i)*Ln_ht,strings[i]);
	button(Ab_rx-20*Ch_wd-1,Ab_ty+13*Ln_ht,Ab_rx-12*Ch_wd+1,Ab_ty+14.5*Ln_ht,"S. step");
	button(Ab_rx-11*Ch_wd-1,Ab_ty+13*Ln_ht,Ab_rx-3*Ch_wd,Ab_ty+14.5*Ln_ht,"reseT");
	/* draw the address window */
	clear(Ab_lx+4*Ch_wd,Ab_ty+5*Ln_ht,Ab_lx+23*Ch_wd,Ab_ty+8*Ln_ht,Color[0]);
	for (i=0;i<AW;i++)
		seven_seg(Ab_lx+(5+3*i)*Ch_wd,Ab_ty+5.5*Ln_ht,63);
	/* draw the data window */
	clear(Ab_lx+4*Ch_wd,Ab_ty+10*Ln_ht,Ab_lx+17*Ch_wd,Ab_ty+13*Ln_ht,Color[0]);
	for (i=0;i<DW;i++)
		seven_seg(Ab_lx+(5+3*i)*Ch_wd,Ab_ty+10.5*Ln_ht,63);
	/* rectangle indicates currently active window is address window */
	setcolor(Color[6]);
	rectangle(Ab_lx+3*Ch_wd,Ab_ty+4*Ln_ht,Ab_lx+24*Ch_wd,Ab_ty+9*Ln_ht);
	setcolor( Color[15] );
	setfillstyle(SOLID_FILL, Color[8]);
	setlinestyle(SOLID_LINE, 0, THICK_WIDTH);
	bar3d(Ab_lx,Ab_by+4*Ln_ht,Ab_rx,Ab_by+5.5*Ln_ht,0,0);
	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	setcolor(Color[9]);
	rectangle(Ab_lx-1,Ab_by+4*Ln_ht-1,Ab_rx+1,Ab_by+5.5*Ln_ht+1);
	rectangle(Ab_lx+2,Ab_by+4*Ln_ht+2,Ab_rx-2,Ab_by+5.5*Ln_ht-2);
	Outtextxy(Ab_lx+12*Ch_wd,Ab_by+4.5*Ln_ht,"F2 - Save     F3 - Load     ^Z - Exit",Color[0]);
}

void setup(void)   /*-- Setup different Video & Display parameters --*/
{
	int i;

	detectgraph(&g_driver,&g_mode);
	switch (g_driver) {
		case 3 :
			switch (g_mode) {
				case 0 : /* 640 x 200 ;16 color,4 page ; EGA */
						Card = 30;
						Ln_ht = 8;
						Ch_wd = 8;
						break;
				case 1 : /* 640 x 350 ;16 color,2 page ; EGA */
						Card = 31;
						Ln_ht = 12;
						Ch_wd = 8;
						break;
			}
			break;
		case 5 : /* 640 x 350 ; 4 page ; EGAMONO  */
				Card = 50;
				Ln_ht = 12;
				Ch_wd = 8;
				break;
		case 7 : /* 720 x 348 ; 2 page ; HERCMONO */
				Card = 70;
				Ln_ht = 12;
				Ch_wd = 9;
				break;
		case 9 :
			switch (g_mode) {
				case 0 : /* 640 x 200 ;16 color,4 page ; VGA */
						Card = 90;
						Ln_ht = 8;
						Ch_wd = 8;
						break;
				case 1 : /* 640 x 350 ;16 color,2 page ; VGA */
						Card = 91;
						Ln_ht = 12;
						Ch_wd = 8;
						break;
				case 2 : /* 640 x 480 ;16 color,1 page ; VGA */
						Card = 92;
						Ln_ht = 16;
						Ch_wd = 8;
						break;
			}
			break;
		default : puts("Graphics card not supported."); exit(0);
	}
	switch (Card) {     /* Setup different colors */
		case 30 :
		case 31 :
		case 90 :
		case 91 :
		case 92 : 	Color[0]  = EGA_BLACK;
					Color[1]  = EGA_BLUE;
					Color[2]  = EGA_GREEN;
					Color[3]  = EGA_CYAN;
					Color[4]  = EGA_RED;
					Color[5]  = EGA_MAGENTA;
					Color[6]  = EGA_BROWN;
					Color[7]  = EGA_DARKGRAY;
					Color[8]  = EGA_LIGHTGRAY;
					Color[9]  = EGA_LIGHTBLUE;
					Color[10] = EGA_LIGHTGREEN;
					Color[11] = EGA_LIGHTCYAN;
					Color[12] = EGA_LIGHTRED;
					Color[13] = EGA_LIGHTMAGENTA;
					Color[14] = EGA_YELLOW;
					Color[15] = EGA_WHITE;
					break;
		case 50 :
		case 70 :   for( i = 0; i < 8; i++ ) Color[i] = BLACK;
					for( ; i < 16; i++ ) Color[i] = WHITE;
					if( Card == 50 ) {
						Color[5] = Color[7] = Color[9] = 1;
						Color[6] = 0;
						}
					break;
	}
	Ab_lx = 10 * Ch_wd;    /* Screen co ordinates for */
	Ab_rx = 70 * Ch_wd;
	Ab_ty = 3 * Ln_ht;     /* the Microkit window     */
	Ab_by = 23 * Ln_ht;
	PC = 0;                /* Set current address location  0 */
	load_bit_font();       /* Load font file */
	/* Invokes appropriate graphic dirver in the program */
	if (Card == 70) registerbgidriver(Herc_driver);
	else registerbgidriver(EGAVGA_driver);
}

void seven_seg(int lx,int ty,char code)
/* This function drives a seven segment display at specified location.
The display is formed according to the parameter 'code'. The code of
each segment is same as standard seven segment drivers.
	No global variable used.
	Subroutine called : clear
*/
{
	setcolor(Color[10]);

	clear(lx-1,ty,lx+1.5*Ch_wd+3,ty+2*Ln_ht+2,Color[0]);
	if (code >= 64) {
		line(lx+1,ty+Ln_ht,lx+1.5*Ch_wd+1,ty+Ln_ht);
		line(lx,ty+Ln_ht+1,lx+1.5*Ch_wd+2,ty+Ln_ht+1);
		line(lx+1,ty+Ln_ht+2,lx+1.5*Ch_wd+1,ty+Ln_ht+2);
		code -=64;
	}
	if (code >= 32) {
		line(lx-1,ty+1,lx-1,ty+Ln_ht);
		line(lx,ty+2,lx,ty+Ln_ht-1);
		line(lx+1,ty+3,lx+1,ty+Ln_ht-2);
		code -= 32;
	}
	if (code >= 16) {
		line(lx-1,ty+Ln_ht+2,lx-1,ty+2*Ln_ht+1);
		line(lx,ty+Ln_ht+3,lx,ty+2*Ln_ht);
		line(lx+1,ty+Ln_ht+4,lx+1,ty+2*Ln_ht-1);
		code -=16;
	}
	if (code >= 8) {
		line(lx+2,ty+2*Ln_ht,lx+1.5*Ch_wd,ty+2*Ln_ht);
		line(lx+1,ty+2*Ln_ht+1,lx+1.5*Ch_wd+1,ty+2*Ln_ht+1);
		line(lx,ty+2*Ln_ht+2,lx+1.5*Ch_wd+2,ty+2*Ln_ht+2);
		code -= 8;
	}
	if (code >= 4) {
		line(lx+1.5*Ch_wd+3,ty+Ln_ht+2,lx+1.5*Ch_wd+3,ty+2*Ln_ht+1);
		line(lx+1.5*Ch_wd+2,ty+Ln_ht+3,lx+1.5*Ch_wd+2,ty+2*Ln_ht);
		line(lx+1.5*Ch_wd+1,ty+Ln_ht+4,lx+1.5*Ch_wd+1,ty+2*Ln_ht-1);
		code -= 4;
	}
	if (code >= 2) {
		line(lx+1.5*Ch_wd+3,ty+1,lx+1.5*Ch_wd+3,ty+Ln_ht);
		line(lx+1.5*Ch_wd+2,ty+2,lx+1.5*Ch_wd+2,ty+Ln_ht-1);
		line(lx+1.5*Ch_wd+1,ty+3,lx+1.5*Ch_wd+1,ty+Ln_ht-2);
		code -= 2;
	}
	if (code==1) {
		line(lx,ty,lx+1.5*Ch_wd+2,ty);
		line(lx+1,ty+1,lx+1.5*Ch_wd+1,ty+1);
		line(lx+2,ty+2,lx+1.5*Ch_wd,ty+2);
	}
}

int Textwidth( BYTE *s )
{
	int i = 0;
	BYTE ch;

	while( *s ) {
        if( ( *s > 127 ) || ( *s < 32 ) ) ch = 0;
			else ch = *s - 32;
		i += (fonts[ch][2]+1);
		s++;
	}
	return i;
}

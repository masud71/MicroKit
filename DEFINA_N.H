/*****************************************
				Constant defination
*****************************************/

#define	MAXMEM 		3072		/* Maximum memory size */


/*****************************************
				Type Definations
*****************************************/

typedef unsigned long int	REG32 ;
typedef unsigned int		REG16 ;
typedef unsigned char		REG8 ;

					/* for DATA REGISTERS */

typedef union {
	REG32	dword ;  	/* 32 bit */
	REG16	word  ;		/* 16 bit */
	REG8	byte  ;		/* 8  bit */
} D_REGS;

					/* for FLAG REGISTER */

typedef struct {
	unsigned c 		: 1 ;		/* carry */
	unsigned v 		: 1 ;		/* overflow */
	unsigned z 		: 1 ;		/* zero */
	unsigned n 		: 1 ;		/* negative */
	unsigned x 		: 1 ;		/* extend */
	unsigned unused : 3 ;		/* unused bits */
	unsigned system : 5 ;		/* system bits(unused also,so not defined */
	unsigned s      : 1 ;       /* supervisor bit */
	unsigned rest   : 2 ;       /* not needed  */
}FLAG_REGS;

					/* for ADDRESS REGISTERS */

typedef union {
	REG32 	dword ;		/* for 24 bit address */
	REG16	word  ;		/* for 16 bit address */
} A_REGS;

/*********************************************************
			Global Variables
				( each register type variable in  UPPERCASE)
*********************************************************/
#ifdef MAIN

A_REGS		A[8] ;		/* A0...A[7] Address registers */
D_REGS		D[8] ;		/* D0...D[7]    Data registers */
FLAG_REGS   FLAG ;		/* flag register */
REG32		PC	 ;      /* program counter */

unsigned char memory[MAXMEM] ;

#else

extern A_REGS		A[8] ;		/* A0...A[7] Address registers */
extern D_REGS		D[8] ;		/* D0...D[7]    Data registers */
extern FLAG_REGS   FLAG ;		/* flag register */
extern REG32		PC	 ;      /* program counter */

extern unsigned char memory[MAXMEM] ;

#endif
/***************End of defination ***********************/
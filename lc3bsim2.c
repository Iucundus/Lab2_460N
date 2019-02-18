/*
    Remove all unnecessary lines (including this one) 
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Full name of the first partner 
    Name 2: Full name of the second partner
    UTEID 1: UT EID of the first partner
    UTEID 2: UT EID of the second partner
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

/*
 * Pick a range of bits from the whole instruction
 * start and end are indexes as in the printed handout
 * start < end
 */
short getBits(int whole, int start, int end) {
	return (whole >> end) & (0xFFFF >> (15 - start + end));
}

/*
 * Set NZP bits based on argument value
 */
void setCC(int value) {
	NEXT_LATCHES.N = (value < 0);
	NEXT_LATCHES.Z = (value == 0);
	NEXT_LATCHES.P = (value > 0);
}

/*
 * Sign-extend argument x of length len bits
 */
short SEXT(short x, int len) {
	if (x >> (len-1)) { //Is negative
		return x | (0xFFFF << len);
	} else
		return x;
}

/*
 * Read two bytes from memory
 */
int readWord(int addr) {
	int rval = MEMORY[addr/2][1];
	rval = rval << 8;
	rval |= MEMORY[addr/2][0];
	return rval;
}

void add(int op) {
	short dr;
	short sr1;
	short sr2;
	short imm5;

	dr = getBits(op, 11, 9);
	sr1 = getBits(op, 8, 6);

	if (op & 0x0020) { // Immediate
		imm5 = getBits(op, 4, 0);
		NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] + SEXT(imm5, 5);
	} else { // Register
		sr2 = getBits(op, 2, 0);
		NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] + CURRENT_LATCHES.REGS[sr2];
	}
	setCC(NEXT_LATCHES.REGS[dr]);
}

void and(int op) {
	short dr;
	short sr1;
	short sr2;
	short imm5;

	dr = getBits(op, 11, 9);
	sr1 = getBits(op, 8, 6);

	if (op & 0x0020) { // Immediate
		imm5 = getBits(op, 4, 0);
		NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] & SEXT(imm5, 5);
	} else { // Register
		sr2 = getBits(op, 2, 0);
		NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] & CURRENT_LATCHES.REGS[sr2];
	}
	setCC(NEXT_LATCHES.REGS[dr]);
}

void br(int op) {
	short n = getBits(op, 11, 11);
	short z = getBits(op, 10, 10);
	short p = getBits(op, 9, 9);
	short PCoffset9 = getBits(op, 8, 0);

	if ((n && CURRENT_LATCHES.N) ||
		(z && CURRENT_LATCHES.Z) ||
		(p && CURRENT_LATCHES.P))
		NEXT_LATCHES.PC = NEXT_LATCHES.PC + (SEXT(PCoffset9, 9) << 1); //Probably should not left shift? PC is stored as /2
}

void jmp(int op) {
	short BaseR = getBits(op, 8, 6);
	NEXT_LATCHES.PC = BaseR;
}

void jsr(int op) {
	int TEMP = NEXT_LATCHES.PC;
	if (op & 0x0800) { // PC offset
		short PCoffset11 = getBits(op, 10, 0);
		NEXT_LATCHES.PC = NEXT_LATCHES.PC + (SEXT(PCoffset11, 11) << 1); //Left shift, again?
	} else { // Register
		short BaseR = getBits(op, 8, 6);
		NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BaseR];
	}
	NEXT_LATCHES.REGS[7] = TEMP;
}

void ldb(int op) {
	short dr = getBits(op, 11, 9);
	short BaseR = getBits(op, 8, 6);
	short boffset6 = getBits(op, 5, 0);

	int addr = BaseR + SEXT(boffset6, 6);

	NEXT_LATCHES.REGS[dr] = SEXT(MEMORY[addr/2][addr%2], 8);
	setCC(NEXT_LATCHES.REGS[dr]);
}

void ldw(int op) {
	short dr = getBits(op, 11, 9);
	short BaseR = getBits(op, 8, 6);
	short offset6 = getBits(op, 5, 0);

	/*
	 * if (CURRENT_LATCHES[BaseR] & 0x01)
	 * 		illegal operand exception
	 */

	NEXT_LATCHES.REGS[dr] = MEMORY[BaseR + (SEXT(offset6, 6) << 1)][1];
	NEXT_LATCHES.REGS[dr] = NEXT_LATCHES.REGS[dr] << 8;
	NEXT_LATCHES.REGS[dr] |= MEMORY[BaseR + (SEXT(offset6, 6) << 1)][0];

	setCC(NEXT_LATCHES.REGS[dr]);
}

void lea(int op) {
	short dr = getBits(op, 11, 9);
	short PCoffset9 = getBits(op, 8, 0);

	NEXT_LATCHES.REGS[dr] = NEXT_LATCHES.PC + (SEXT(PCoffset9, 9) << 1);
	setCC(NEXT_LATCHES.REGS[dr]);
}

void rti(int op) {
	//Check psr[15] for privilege mode violation
	NEXT_LATCHES.PC = readWord(CURRENT_LATCHES.REGS[6]);
	CURRENT_LATCHES.REGS[6] += 2;
	int TEMP = readWord(CURRENT_LATCHES.REGS[6]);
	CURRENT_LATCHES.REGS[6] += 2;
	//TODO: put PSR value into non-existent PSR variable
}

void shf(int op) {
	short dr = getBits(op, 11, 9);
	short sr = getBits(op, 8, 6);
	short amount4 = getBits(op, 3, 0);

	if (op & 0x10) { // Right shift
		if (op & 0x20) { // Arithmetic shift
			CURRENT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr] >> amount4;
			if (CURRENT_LATCHES.REGS[sr] & 0x8000) {
				CURRENT_LATCHES.REGS[dr] |= 0xFFFF << (16 - amount4);
			}
		} else { // Logical shift
			CURRENT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr] >> amount4;
		}
	} else { // Left shift
		CURRENT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr] << amount4;
	}

	setCC(CURRENT_LATCHES.REGS[dr]);
}

void stb(int op) {
	int addr = getBits(op, 8, 6) + SEXT(getBits(op, 5, 0), 6);
	int val = CURRENT_LATCHES.REGS[getBits(op, 11, 9)] & 0x00FF;
	MEMORY[addr/2][addr%2] = val;
}

void stw(int op) {
	int addr = getBits(op, 8, 6) + SEXT(getBits(op, 5, 0), 6);
	//if (addr & 0x01) //illegal operand exception
	int val = CURRENT_LATCHES.REGS[getBits(op, 11, 9)];
	MEMORY[addr/2][0] = val & 0x00FF;
	MEMORY[addr/2][1] = val >> 8;
}

void trap(int op) {
	NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
	NEXT_LATCHES.PC = readWord(getBits(op, 7, 0) << 1);
}

void xor(int op) {
	short dr = getBits(op, 11, 9);
	short sr1 = getBits(op, 8, 6);
	if (op & 20) { // Immediate
		short imm5 = getBits(op, 4, 0);
		CURRENT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] ^ SEXT(imm5, 5);
	} else { // Register
		short sr2 = getBits(op, 2, 0);
		CURRENT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] ^ CURRENT_LATCHES.REGS[sr2];
	}

	setCC(CURRENT_LATCHES.REGS[dr]);
}

void nop(int op) {

}

void (*ops[22]) (int op) = {&br, &add, &ldb, &stb, &jsr, &and, &ldw, &stw, &rti, &xor, &nop, &nop, &jmp, &shf, &lea, &trap};

void process_instruction(){
  /*  function: process_instruction
   *
   *    Process one instruction at a time
   *       -Fetch one instruction
   *       -Decode
   *       -Execute
   *       -Update NEXT_LATCHES
   */
	int CurrentInstruction = MEMORY[CURRENT_LATCHES.PC/2][1];
	CurrentInstruction = CurrentInstruction << 8;
	CurrentInstruction |= MEMORY[CURRENT_LATCHES.PC/2][0];

	NEXT_LATCHES.PC ++;
	ops[CurrentInstruction>>12](CurrentInstruction);
}

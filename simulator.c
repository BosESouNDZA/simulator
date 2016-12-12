/* instruction-level simulator */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stdint.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

typedef union {
    struct {
        int32_t offset: 16;
        uint32_t regB: 3;
        uint32_t regA: 3;
        uint32_t opcode: 3;
        uint32_t empty: 7;
    } field;
    int32_t intRepresentation; 
}  bitset;

void printState(stateType *);
int32_t convertNum(int32_t num);
int32_t decimalToBinary(int32_t n);

int32_t main(int32_t  argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state,*statePtr;
    FILE *filePtr;
    

    if (argc != 2) {
    printf("error: usage: %s <machine-code file>\n", argv[0]);
    exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
    printf("error: can't open file %s", argv[1]);
    perror("fopen");
    exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
    state.numMemory++) {
    if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
        printf("error in reading address %d\n", state.numMemory);
        exit(1);
    }
    printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
    //initialize registers and pc
    statePtr = &state;
    statePtr->pc = 0;
    for (int i=0; i<NUMREGS; i++) {
      statePtr->reg[i] =0;
    }
    printState(statePtr);
    int countinst=0;
    //run inst
    while(1)
    {
        bitset inst;
        inst.intRepresentation = statePtr->mem[statePtr->pc]; 
        if(inst.field.opcode == 2) //lw
        {
            inst.field.offset = convertNum(inst.field.offset);
            statePtr->reg[inst.field.regB] = statePtr->mem[inst.field.offset + statePtr->reg[inst.field.regA]]; 
            statePtr->pc++;
            countinst++;
            printState(statePtr);
        }
        else if (inst.field.opcode == 0) //add
        {
           statePtr->reg[inst.field.offset] = statePtr->reg[inst.field.regA] + statePtr->reg[inst.field.regB]; 
           statePtr->pc++;
           countinst++;
           printState(statePtr);
        }
        else if(inst.field.opcode == 4) //beq
        {
            inst.field.offset = convertNum(inst.field.offset);
            if(statePtr->reg[inst.field.regA] == statePtr->reg[inst.field.regB] )
                statePtr->pc+=1+inst.field.offset;
            else
                statePtr->pc++;
            countinst++;
            printState(statePtr);
        }
        else if(inst.field.opcode == 7) //noop
        {
            statePtr->pc++;
            countinst++;
            printState(statePtr);
        }
        else if(inst.field.opcode == 6) //halt
        {
            countinst++;
            statePtr->pc++;
            printf("machine halted\ntotal of %d instructions executed\nfinal state of machine:",countinst);
            printState(statePtr);
            exit(0);
        }
        else if(inst.field.opcode == 3) //sw
        {
            inst.field.offset = convertNum(inst.field.offset);
            statePtr->mem[inst.field.offset + statePtr->reg[inst.field.regA]] = statePtr->reg[inst.field.regB];
            statePtr->pc++;
            countinst++;
            printState(statePtr);
        }
        
        else if(inst.field.opcode == 1) //nand
        {
            statePtr->reg[inst.field.offset] = ~statePtr->reg[inst.field.regA]|~statePtr->reg[inst.field.regB];
            statePtr->pc++;
            countinst++;
            printState(statePtr);
        }
        
        else if(inst.field.opcode == 5) //jalr
        {
            if(inst.field.regB == inst.field.regA)
            {
                statePtr->pc++;
                statePtr->reg[inst.field.regB]=statePtr->pc;
            }
            else
            {
                statePtr->pc++;
                statePtr->reg[inst.field.regB]=statePtr->pc;
                statePtr->pc = statePtr->reg[inst.field.regA];
            }
            countinst++;
            printState(statePtr);
        }
    }
    return(0);
}
int32_t convertNum(int32_t num)
{
    /* convert a 16-bit number into a 32-bit integer */
    if (num & (1<<15) ) 
        num -= (1<<16);
    return(num);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
} 
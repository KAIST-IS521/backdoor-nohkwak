// This is where you put your VM code.
// I am trying to follow the coding style of the original.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "minivm.h"


#define NUM_REGS   (256)
#define NUM_FUNCS  (256)

// Global variable that indicates if the process is running.
static bool is_running = true;

void usageExit() {
    // show usage
    printf( "USAGE: interpreter [FILE]\n" ); 

    exit(1);
}

void *halt(struct VMContext* ctx, const uint32_t instr) {
    printf( "\n\n\n process is halted....\n" ); 

    exit(1);
}
/*
void *load(struct VMcontext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    //const uint8_t c = EXTRACT_B3(instr); //Not used
    ctx->r[a].value = b;
}
*/

void *move(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    // const uint8_t c = EXTRACT_B3(instr);
    ctx->r[a].value = ctx->r[b].value;
    printf("%d = %d\n", ctx->r[a].value , ctx->r[b].value );
}


void *add(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    const uint8_t c = EXTRACT_B3(instr);
    ctx->r[a].value = ctx->r[b].value + ctx->r[c].value;
    printf("%d = %d + %d\n", ctx->r[a].value , ctx->r[b].value , ctx->r[c].value );
}

void *sub(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    const uint8_t c = EXTRACT_B3(instr);
    ctx->r[a].value = ctx->r[b].value - ctx->r[c].value;
    printf("%d = %d - %d\n", ctx->r[a].value , ctx->r[b].value , ctx->r[c].value );
}

void *gt(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    const uint8_t c = EXTRACT_B3(instr);
    if ( ctx->r[b].value >  ctx->r[c].value )
        ctx->r[a].value = 1;
    else 
        ctx->r[a].value = 0;
    printf("%d = ( %d > %d )\n", ctx->r[a].value , ctx->r[b].value , ctx->r[c].value );
}

void *ge(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    const uint8_t c = EXTRACT_B3(instr);
    if ( ctx->r[b].value >=  ctx->r[c].value )
        ctx->r[a].value = 1;
    else 
        ctx->r[a].value = 0;
    printf("%d = ( %d >= %d )\n", ctx->r[a].value , ctx->r[b].value , ctx->r[c].value );
}

void *eq(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    const uint8_t c = EXTRACT_B3(instr);
    if ( ctx->r[b].value ==  ctx->r[c].value )
        ctx->r[a].value = 1;
    else 
        ctx->r[a].value = 0;
    printf("%d = ( %d == %d )\n", ctx->r[a].value , ctx->r[b].value , ctx->r[c].value );
}

void initFuncs(FunPtr *f, uint32_t cnt) {
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        f[i] = NULL;
    }

    // TODO: initialize function pointers
    f[0x00] = halt;
/*  f[0x10] = load;
/*    f[0x11] = store;
*/
    f[0x30] = move;
    // f[0x13] = puti;
    f[0x50] = add;
    f[0x60] = sub;
    f[0x70] = gt;
    f[0x80] = ge;
    f[0x90] = eq;
    /*
    f[0x19] = ite;
    f[0x1a] = jump;
    f[0x1b] = puts; 
    f[0x1c] = gets; */
}

void initRegs(Reg *r, uint32_t cnt)
{
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        r[i].type = 0;
        r[i].value = 0;
    }
}


int main(int argc, char** argv) {
    int i; 
    VMContext vm;
    Reg r[NUM_REGS];
    FunPtr f[NUM_FUNCS];
    FILE* bytecode;
    char heap[4096]; 
    uint32_t* pc;

    // There should be at least one argument.
    if (argc < 2) usageExit();

    // Initialize registers.
    initRegs(r, NUM_REGS);
    // Initialize interpretation functions.
    initFuncs(f, NUM_FUNCS);
    // Initialize VM context.
    initVMContext(&vm, NUM_REGS, NUM_FUNCS, r, f);

    // Load bytecode file
    bytecode = fopen(argv[1], "rb");
    if (bytecode == NULL) {
        perror("fopen");
        return 1;
    }

    i = 0;
    pc = (uint32_t*) &heap; 

    while (is_running) {

        // Read 4-byte bytecode, and set the pc accordingly
        fread((void*)pc, 1, 4, bytecode); 

        // Debugging message for instr
        printf("Instr: %d -> '%d', '%d', '%d', '%d'\n", i, EXTRACT_B0(*pc), EXTRACT_B1(*pc), EXTRACT_B2(*pc), EXTRACT_B3(*pc));
    
        stepVMContext(&vm, &pc);
        i++;
    }

    fclose(bytecode);

    // Zero indicates normal termination.
    return 0;
}

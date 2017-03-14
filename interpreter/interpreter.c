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

// memory 
char text[8192]; 
char heap[8192]; 

// program count 
uint32_t* pc;

void usageExit() {
    // show usage
    printf( "USAGE: interpreter [FILE]\n" ); 

    exit(1);
}

void *halt(struct VMContext* ctx, const uint32_t instr) {
    printf( "\n\n\n process is halted....\n" ); 
    exit(1);
}

void *load(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    //const uint8_t c = EXTRACT_B3(instr); //Not used
    ctx->r[a].value = heap[b];
}

void *store(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    //const uint8_t c = EXTRACT_B3(instr); //Not used
    heap[a] = ctx->r[b].value;
}

void *puti(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    //const uint8_t c = EXTRACT_B3(instr); //Not used
    uint32_t tmp = 0xFF; 
    tmp &= b; 
    ctx->r[a].value = tmp;
}

void *move(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    // const uint8_t c = EXTRACT_B3(instr);
    ctx->r[a].value = ctx->r[b].value;
    printf("%x = %x\n", ctx->r[a].value , ctx->r[b].value );
}


void *add(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    const uint8_t c = EXTRACT_B3(instr);
    ctx->r[a].value = ctx->r[b].value + ctx->r[c].value;
    printf("%x = %x + %x\n", ctx->r[a].value , ctx->r[b].value , ctx->r[c].value );
}

void *sub(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    const uint8_t c = EXTRACT_B3(instr);
    ctx->r[a].value = ctx->r[b].value - ctx->r[c].value;
    printf("%x = %x - %x\n", ctx->r[a].value , ctx->r[b].value , ctx->r[c].value );
}

void *gt(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    const uint8_t c = EXTRACT_B3(instr);
    if ( ctx->r[b].value >  ctx->r[c].value )
        ctx->r[a].value = 1;
    else 
        ctx->r[a].value = 0;
    printf("%x = ( %x > %x )\n", ctx->r[a].value , ctx->r[b].value , ctx->r[c].value );
}

void *ge(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    const uint8_t c = EXTRACT_B3(instr);
    if ( ctx->r[b].value >=  ctx->r[c].value )
        ctx->r[a].value = 1;
    else 
        ctx->r[a].value = 0;
    printf("%x = ( %x >= %x )\n", ctx->r[a].value , ctx->r[b].value , ctx->r[c].value );
}

void *eq(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    const uint8_t c = EXTRACT_B3(instr);
    if ( ctx->r[b].value ==  ctx->r[c].value )
        ctx->r[a].value = 1;
    else 
        ctx->r[a].value = 0;
    printf("%x = ( %x == %x )\n", ctx->r[a].value , ctx->r[b].value , ctx->r[c].value );
}

void *ite(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    const uint8_t b = EXTRACT_B2(instr);
    const uint8_t c = EXTRACT_B3(instr);

    char* tmp = &text;

    if ( ctx->r[a].value > 0 ) {
        // -4 and +4 because pc is incremented by the stepVMContext function
        pc = (uint32_t*) (tmp + b - 4);
        printf("%x = ( %x + %x )\n", tmp + b, tmp, b );
    }
    else {
        pc = (uint32_t*) (tmp + c - 4);
        printf("%x = ( %x + %x )\n", tmp + c, tmp, c );
    }
}

void *jump(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t a = EXTRACT_B1(instr);
    // const uint8_t b = EXTRACT_B2(instr);
    // const uint8_t c = EXTRACT_B3(instr);

    char* tmp = &text;

    // -4 and +4 because pc is incremented by the stepVMContext function 
    pc = (uint32_t*) (tmp + a - 4);
    printf("%x = ( %x + %x )\n", tmp + a, tmp, a );
}

void initFuncs(FunPtr *f, uint32_t cnt) {
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        f[i] = NULL;
    }

    // TODO: initialize function pointers
    f[0x00] = halt;
    f[0x10] = load;
    f[0x20] = store;
    f[0x30] = move;
    f[0x40] = puti;
    f[0x50] = add;
    f[0x60] = sub;
    f[0x70] = gt;
    f[0x80] = ge;
    f[0x90] = eq;
    f[0xa0] = ite;
    f[0xb0] = jump;
    /*
    f[0xc0] = puts_inst; 
    f[0xd0] = gets_inst;*/
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

    fread((void*)text, 1, 8192, bytecode);

    i = 0;
    pc = (uint32_t*) &text; 

    while (is_running) {
        // Read 4-byte bytecode, and set the pc accordingly
        // I assume that the binary is already loaded. 

        // Debugging message for instr
        printf("Instr: %d -> '%x', '%x', '%x', '%x'\n", i, EXTRACT_B0(*pc), EXTRACT_B1(*pc), EXTRACT_B2(*pc), EXTRACT_B3(*pc));
    
        stepVMContext(&vm, &pc);
        i++;
    }

    fclose(bytecode);

    // Zero indicates normal termination.
    return 0;
}

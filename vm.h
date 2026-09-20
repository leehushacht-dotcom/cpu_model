#ifndef VM_H
#define VM_H

// machine work

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>



typedef enum reg_names{
    AX, CX, DX, BX, SP, BP, SI, DI
} REG;

typedef enum reg8_names{
    AL, CL, DL, BL, AH, CH, DH, BH
} REG8;

typedef enum seg_reg_names{
     ES, CS, SS, DS
} SEG_REG;

typedef enum flag_reg_names {
    FLAG_CF = 0, 
    FLAG_PF = 2, 
    FLAG_AF = 4, 
    FLAG_ZF = 6, 
    FLAG_SF = 7, 
    FLAG_TF = 8,
    FLAG_IF = 9,
    FLAG_DF = 10,
    FLAG_OF = 11
} FLAG_REG;

typedef struct VM VM;
VM* create_vm(void);
void destroy_vm(VM* vm);


// need a check
void turn_on_vm(VM* vm);
void turn_off_vm(VM* vm);
bool is_vm_running(VM* vm);

void halt_vm(VM* vm);
void resume_vm(VM* vm);
bool is_vm_halted(VM* vm);

uint8_t fetch_byte(VM* vm);
uint16_t fetch_word(VM* vm);

//in progress
void PUSH_16(VM* vm, uint16_t value);
uint16_t POP_16(VM* vm);



void run_vm(VM* vm);

#endif

#ifndef OPCODES_H
#define OPCODES_H
#include "vm.h"

// opcodes handle

uint32_t get_final_address(VM* vm, uint8_t mod, uint8_t rm); // call if mod != 3 (reg)*

void init_opcode_table();

// חתימה לפונקציות הפקודות
typedef void (*InstructionHandler)(VM* vm);

extern InstructionHandler opcode_main_handle_list[256];

#endif

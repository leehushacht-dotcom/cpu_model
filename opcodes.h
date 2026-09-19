#ifndef OPCODES_H
#define OPCODES_H
#include "vm.h"

// opcodes handle

void init_opcode_table();

// חתימה לפונקציות הפקודות
typedef void (*InstructionHandler)(VM* vm, uint8_t cur_opcode);
typedef uint8_t (*alu_operands_handle8)(VM* vm, uint8_t a, uint8_t b);
typedef uint16_t (*alu_operands_handle16)(VM* vm, uint16_t a, uint16_t b);

extern InstructionHandler opcode_main_handle_list[256];

#endif

#ifndef OPCODES_H
#define OPCODES_H
#include "vm.h"

// opcodes handle

void init_opcode_table();

// חתימה לפונקציות הפקודות
typedef void (*InstructionHandler)(VM* vm, uint8_t cur_opcode);


extern InstructionHandler opcode_main_handle_list[256];

#endif

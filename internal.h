#ifndef INTERNAL_H
#define INTERNAL_H

#include "vm.h" 

#define MEMORY_SIZE (1024*1024) 
#define PROGRAM_AREA 0x01000
#define IO_AREA 0xFA000
#define KERNEL_AREA 0xFFFF0


struct VM {
    uint8_t memory[MEMORY_SIZE]; 
    uint16_t reg[8]; 
    uint16_t seg_reg[4]; 
    uint16_t flags_reg; 
    uint16_t ip;
    bool is_running;
};

#endif
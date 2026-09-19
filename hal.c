#include "hal.h"
#include "internal.h"

uint32_t get_address(uint16_t seg_reg, uint16_t offset){
    return ((uint32_t)seg_reg)*16+offset;
}

uint16_t read_reg16(const VM* vm, REG reg_index){
    return vm->reg[reg_index & 0x07];
}
void write_reg16(VM* vm, REG reg_index, uint16_t value){
    vm->reg[reg_index & 0x07] = value;
}
uint8_t read_reg8(const VM* vm, REG8 reg_index){
    if (reg_index < 4) {
        // Low bits (AL, CL, DL, BL)
        return (uint8_t)(vm->reg[reg_index & 0x07] & 0x00ff);
    } 
    else {
        // High bits (AH, CH, DH, BH)
        return (uint8_t)(vm->reg[(reg_index - 4) & 0x07] >> 8);
    }
}
void write_reg8(VM* vm, REG8 reg_index, uint8_t value){
    if (reg_index > BH) return;  // not a real 8-bit register: ignore
    if (reg_index < AH){
        // low bits
        vm->reg[reg_index] = (vm->reg[reg_index] & 0xFF00) | value;
    }
    else{
        // high bits
        uint8_t i = reg_index - 4;
        vm->reg[i] = (vm->reg[i] & 0x00FF) | ((uint16_t)value << 8);
    }
}
void write_mem16(VM* vm, uint32_t address, uint16_t value) {
    vm->memory[address & 0xFFFFF] = value & 0xFF;
    vm->memory[(address + 1) & 0xFFFFF] = (value >> 8) & 0xFF;
}
uint16_t read_mem16(const VM* vm, uint32_t physical_address) {
    uint8_t low = vm->memory[physical_address & 0xFFFFF];
    uint8_t high = vm->memory[(physical_address + 1) & 0xFFFFF];
    return (uint16_t)low | ((uint16_t)high << 8);
}
void write_mem8(VM* vm, uint32_t address, uint8_t value) {
    vm->memory[address & 0xfffff] = value;
}
uint8_t read_mem8(const VM* vm, uint32_t address) {
    return vm->memory[address & 0xfffff];
}

void write_segreg16(VM* vm, SEG_REG seg_reg_index, uint16_t value){
    vm->seg_reg[seg_reg_index & 0x03] = value;
}
uint16_t read_segreg16(const VM* vm, SEG_REG seg_reg_index){
    return vm->seg_reg[seg_reg_index & 0x03];
}
void write_ip16(VM* vm, uint16_t value){
    vm->ip = value;
}
uint16_t read_ip16(const VM* vm){
    return vm->ip;
}

void set_flag(VM* vm, FLAG_REG flag_i, bool lit){
    if (lit){
        vm->flags_reg = vm->flags_reg | (0x0001 << flag_i);
    }
    else{
        vm->flags_reg = vm->flags_reg & ~(0x0001 << flag_i);
    }
}
bool get_flag(const VM* vm, FLAG_REG flag_i){
    return ((vm->flags_reg >> flag_i) & 0x0001) == 1;
}


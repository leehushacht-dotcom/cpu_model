#ifndef HAL_H
#define HAL_H
#include "vm.h"

// hardware access

// reg api
uint16_t read_reg16(const VM* vm, REG reg_index); // R
void write_reg16(VM* vm, REG reg_index, uint16_t value); // W
uint8_t read_reg8(const VM* vm, REG8 reg_index); // R
void write_reg8(VM* vm, REG8 reg_index, uint8_t value); // W
// memory api
void write_mem16(VM* vm, uint32_t address, uint16_t value); // W
uint16_t read_mem16(const VM* vm, uint32_t physical_address); // R
void write_mem8(VM* vm, uint32_t address, uint8_t value); // W
uint8_t read_mem8(const VM* vm, uint32_t address); // R
// segment : offset calc
uint32_t get_address(uint16_t seg_reg, uint16_t reg);


// need a check
void write_segreg16(VM* vm, SEG_REG seg_reg_index, uint16_t value); // W
uint16_t read_segreg16(const VM* vm, SEG_REG seg_reg_index); // R
void write_ip16(VM* vm, uint16_t value); // W
uint16_t read_ip16(const VM* vm); // R

void set_flag(VM* vm, FLAG_REG flag_i, bool lit); // W
bool get_flag(const VM* vm, FLAG_REG flag_i); // R

#endif
#ifndef ALU_H
#define ALU_H
#include "vm.h"


uint16_t alu_add16(VM* vm, uint16_t a, uint16_t b);
uint8_t alu_add8(VM* vm, uint8_t a, uint8_t b);
uint16_t alu_adc16(VM* vm, uint16_t a, uint16_t b);
uint8_t alu_adc8(VM* vm, uint8_t a, uint8_t b);

uint16_t alu_sub16(VM* vm, uint16_t a, uint16_t b);
uint8_t alu_sub8(VM* vm, uint8_t a, uint8_t b);
uint16_t alu_sbb16(VM* vm, uint16_t a, uint16_t b);
uint8_t alu_sbb8(VM* vm, uint8_t a, uint8_t b);

uint8_t alu_cmp8(VM* vm, uint8_t a, uint8_t b);
uint16_t alu_cmp16(VM* vm, uint16_t a, uint16_t b);

uint8_t alu_xor8(VM* vm, uint8_t a, uint8_t b);
uint16_t alu_xor16(VM* vm, uint16_t a, uint16_t b);

uint8_t alu_or8(VM* vm, uint8_t a, uint8_t b);
uint16_t alu_or16(VM* vm, uint16_t a, uint16_t b);

uint8_t alu_and8(VM* vm, uint8_t a, uint8_t b);
uint16_t alu_and16(VM* vm, uint16_t a, uint16_t b);

#endif
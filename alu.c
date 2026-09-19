#include "alu.h"
#include "vm.h"
#include "hal.h"


static void update_flags_result_only16(VM* vm, uint16_t result){
    // zf
    set_flag(vm, FLAG_ZF, (result & 0xffff) == 0);
    // sf
    set_flag(vm, FLAG_SF, (result & 0x8000) != 0);
    // pf
    uint8_t p = result & 0xFF;
    p ^= p >> 4;
    p ^= p >> 2;
    p ^= p >> 1;
    set_flag(vm, FLAG_PF, (~p) & 1);
}
static void update_flags_result_only8(VM* vm, uint8_t result){
    // zf
    set_flag(vm, FLAG_ZF, (result & 0xff) == 0);
    // sf
    set_flag(vm, FLAG_SF, (result & 0x80) != 0);
    // pf
    uint8_t p = result & 0xFF;
    p ^= p >> 4;
    p ^= p >> 2;
    p ^= p >> 1;
    set_flag(vm, FLAG_PF, (~p) & 1);
}


static void add_flags16(VM* vm, uint16_t a, uint16_t b, uint32_t result){
    update_flags_result_only16(vm, result);
    // of
    set_flag(vm, FLAG_OF, ((a ^ result) & (b ^ result) & 0x8000) != 0);
    // cf
    set_flag(vm, FLAG_CF, result > 0xffff);
    // AF
    set_flag(vm, FLAG_AF, (a ^ b ^ result) & 0x10);
}
static void add_flags8(VM* vm, uint8_t a, uint8_t b, uint16_t result){
    update_flags_result_only8(vm, result);
    // of
    set_flag(vm, FLAG_OF, ((a ^ result) & (b ^ result) & 0x80) != 0);
    // cf
    set_flag(vm, FLAG_CF, result > 0xff);
    // AF
    set_flag(vm, FLAG_AF, (a ^ b ^ result) & 0x10);
}

uint16_t alu_add16(VM* vm, uint16_t a, uint16_t b){
    add_flags16(vm, a, b, (uint32_t)a +(uint32_t)b);
    return a + b;
}
uint8_t alu_add8(VM* vm, uint8_t a, uint8_t b){
    add_flags8(vm, a, b, (uint16_t)a +(uint16_t)b);

    return a + b;
}
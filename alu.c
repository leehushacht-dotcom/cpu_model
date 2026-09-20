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

// < -------- add --------- >
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
uint16_t alu_adc16(VM* vm, uint16_t a, uint16_t b){
    uint32_t c = get_flag(vm, FLAG_CF) ? 1 : 0;
    uint32_t result = (uint32_t)a + (uint32_t)b + c;
    add_flags16(vm, a, b, result);
    return (uint16_t)result;
}
uint8_t alu_adc8(VM* vm, uint8_t a, uint8_t b){
    uint16_t c = get_flag(vm, FLAG_CF) ? 1 : 0;
    uint16_t result = (uint16_t)a + (uint16_t)b + c;
    add_flags8(vm, a, b, result);
    return (uint8_t)result;
}
// < -------- add --------- >

// < -------- sub --------- >
static void sub_flags16(VM* vm, uint16_t a, uint16_t b, uint32_t result){
    update_flags_result_only16(vm, result);
    // of
    set_flag(vm, FLAG_OF, ((a ^ b) & (a ^ result) & 0x8000) != 0);
    // cf
    set_flag(vm, FLAG_CF, result > 0xffff);
    // AF
    set_flag(vm, FLAG_AF, (a ^ b ^ result) & 0x10);
}
static void sub_flags8(VM* vm, uint8_t a, uint8_t b, uint16_t result){
    update_flags_result_only8(vm, result);
    // of
    set_flag(vm, FLAG_OF, ((a ^ b) & (a ^ result) & 0x80) != 0);
    // cf
    set_flag(vm, FLAG_CF, result > 0xff);
    // AF
    set_flag(vm, FLAG_AF, (a ^ b ^ result) & 0x10);
}
uint16_t alu_sub16(VM* vm, uint16_t a, uint16_t b){
    sub_flags16(vm, a, b, (uint32_t)a -(uint32_t)b);
    return a - b;
}
uint8_t alu_sub8(VM* vm, uint8_t a, uint8_t b){
    sub_flags8(vm, a, b, (uint16_t)a - (uint16_t)b);
    return a - b;
}
uint16_t alu_sbb16(VM* vm, uint16_t a, uint16_t b){
    uint32_t c = get_flag(vm, FLAG_CF) ? 1 : 0;
    uint32_t result = (uint32_t)a - (uint32_t)b - c;
    sub_flags16(vm, a, b, result);
    return (uint16_t)result;
}
uint8_t alu_sbb8(VM* vm, uint8_t a, uint8_t b){
    uint16_t c = get_flag(vm, FLAG_CF) ? 1 : 0;
    uint16_t result = (uint16_t)a - (uint16_t)b - c;
    sub_flags8(vm, a, b, result);
    return (uint8_t)result;
}
// < -------- sub --------- >
// < -------- cmp --------- >
uint8_t alu_cmp8(VM* vm, uint8_t a, uint8_t b){
    sub_flags8(vm, a, b, (uint16_t)a - (uint16_t)b);
    return a;
}
uint16_t alu_cmp16(VM* vm, uint16_t a, uint16_t b){
    sub_flags16(vm, a, b, (uint32_t)a - (uint32_t)b);
    return a;
}
// < -------- cmp --------- >
// < --- AND, XOR, OR ----- >
static void basic_flags_update8(VM* vm, uint16_t result){
    update_flags_result_only8(vm, result);
    set_flag(vm, FLAG_CF, 0);
    set_flag(vm, FLAG_OF, 0);
    set_flag(vm, FLAG_AF, 0);
}
static void basic_flags_update16(VM* vm, uint32_t result){
    update_flags_result_only16(vm, result);
    set_flag(vm, FLAG_CF, 0);
    set_flag(vm, FLAG_OF, 0);
    set_flag(vm, FLAG_AF, 0);
}
uint8_t alu_xor8(VM* vm, uint8_t a, uint8_t b){
    basic_flags_update8(vm, (uint16_t)a^(uint16_t)b);
    return a ^ b;
}
uint16_t alu_xor16(VM* vm, uint16_t a, uint16_t b){
    basic_flags_update16(vm, (uint32_t)a^(uint32_t)b);
    return a ^ b;
}
uint8_t alu_or8(VM* vm, uint8_t a, uint8_t b){
    basic_flags_update8(vm, (uint16_t)a|(uint16_t)b);
    return a | b;
}
uint16_t alu_or16(VM* vm, uint16_t a, uint16_t b){
    basic_flags_update16(vm, (uint32_t)a|(uint32_t)b);
    return a | b;
}
uint8_t alu_and8(VM* vm, uint8_t a, uint8_t b){
    basic_flags_update8(vm, (uint16_t)a&(uint16_t)b);
    return a & b;
}
uint16_t alu_and16(VM* vm, uint16_t a, uint16_t b){
    basic_flags_update16(vm, (uint32_t)a&(uint32_t)b);
    return a & b;
}
// < --- AND, XOR, OR ----- >
// < -------- INC --------- >
uint16_t alu_inc16(VM* vm, uint16_t a){
    bool save = get_flag(vm, FLAG_CF);
    add_flags16(vm, a, 1, (uint32_t)a+1);
    set_flag(vm, FLAG_CF, save);
    return a + 1;
}
uint8_t alu_inc8(VM* vm, uint8_t a){
    bool save = get_flag(vm, FLAG_CF);
    add_flags8(vm, a, 1, (uint16_t)a+1);
    set_flag(vm, FLAG_CF, save);
    return a + 1;
}
// < -------- INC --------- >
// < -------- DEC --------- >
uint16_t alu_dec16(VM* vm, uint16_t a){
    bool save = get_flag(vm, FLAG_CF);
    sub_flags16(vm, a, 1, (uint32_t)a-1);
    set_flag(vm, FLAG_CF, save);
    return a - 1;
}
uint8_t alu_dec8(VM* vm, uint8_t a){
    bool save = get_flag(vm, FLAG_CF);
    sub_flags8(vm, a, 1, (uint16_t)a-1);
    set_flag(vm, FLAG_CF, save);
    return a - 1;
}

// < -------- DEC --------- >
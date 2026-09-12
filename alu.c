#include "alu.h"
#include "vm.h"
#include "hal.h"

static void add_caculate_flags16(VM* vm, uint16_t a, uint16_t b, uint32_t result){
    // sf

    // of

    // zf

    // cf
    set_flag(vm, FLAG_CF, result > 0xffff);
}
static void add_caculate_flags8(VM* vm, uint8_t a, uint8_t b, uint16_t result){

}

uint16_t alu_add16(VM* vm, uint16_t a, uint16_t b){
    add_caculate_lags16(vm, a, b, (uint32_t)a +(uint32_t)b);
    return a + b;
}
uint8_t alu_add8(VM* vm, uint8_t a, uint8_t b){
    add_caculate_flags8(vm, a, b, (uint16_t)a +(uint16_t)b);

    return a + b;
}
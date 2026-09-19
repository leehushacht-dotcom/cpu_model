#include "vm.h"
#include "opcodes.h"
#include "hal.h"


static void print_state(const VM* vm){
    printf("AX=%04X BX=%04X CX=%04X DX=%04X SP=%04X\n",
        read_reg16(vm, AX), read_reg16(vm, BX), read_reg16(vm, CX),
        read_reg16(vm, DX), read_reg16(vm, SP));
    printf("CF=%d ZF=%d SF=%d OF=%d PF=%d AF=%d\n",
        get_flag(vm, FLAG_CF), get_flag(vm, FLAG_ZF), get_flag(vm, FLAG_SF),
        get_flag(vm, FLAG_OF), get_flag(vm, FLAG_PF), get_flag(vm, FLAG_AF));
}

static void test(VM* vm){
    uint32_t a = get_address(read_segreg16(vm, CS), read_ip16(vm));
    const uint8_t prog[] = {
        0xB8, 0xFF, 0xFF,   // MOV AX, FFFFh
        0xBB, 0x01, 0x00,   // MOV BX, 0001h
        0x01, 0xD8,         // ADD AX, BX      -> AX=0000, CF=1 ZF=1
        0xB9, 0x05, 0x00,   // MOV CX, 0005h
        0xBA, 0x00, 0x00,   // MOV DX, 0000h
        0x11, 0xD1,         // ADC CX, DX      -> CX=5+0+CF = 0006, CF=0
        0xB8, 0xF0, 0x00,   // MOV AX, 00F0h
        0x24, 0x3C,         // AND AL, 3Ch     -> AL=30
        0x0C, 0x0F,         // OR  AL, 0Fh     -> AL=3F
        0x31, 0xC0,         // XOR AX, AX      -> AX=0000, ZF=1 PF=1
        0x90                // NOP (unimplemented) = stop
    };
    for (size_t i = 0; i < sizeof prog; i++)
        write_mem8(vm, a + i, prog[i]);
}


static void prepare_segment_registers(VM* vm)
{
    // for now this value -> later give more thought on it
    write_segreg16(vm, CS, 0x0700);
    write_segreg16(vm, DS, 0x0700);
    write_segreg16(vm, ES, 0x0700);
    write_segreg16(vm, SS, 0x0700);
    write_reg16(vm, SP, 0xFF00); // for test
    write_ip16(vm, 0x0100);

}
static void prepare_flags(VM* vm)
{
    set_flag(vm, FLAG_CF, false);
    set_flag(vm, FLAG_SF, false);
    set_flag(vm, FLAG_ZF, false);
    set_flag(vm, FLAG_AF, false);
    set_flag(vm, FLAG_DF, false);
    set_flag(vm, FLAG_IF, false);
    set_flag(vm, FLAG_OF, false);
    set_flag(vm, FLAG_PF, false);

}
int main(){
    
    init_opcode_table();
    VM* vm = create_vm();;
    turn_on_vm(vm);

    
    // prapare all machine here!
    prepare_segment_registers(vm);
    prepare_flags(vm);
    print_state(vm);
    test(vm);

    run_vm(vm);
    print_state(vm);
    destroy_vm(vm);
    return 0;
}

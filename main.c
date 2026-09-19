#include "vm.h"
#include "opcodes.h"
#include "hal.h"

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
    set_flag(vm, FLAG_ZF, true);
    set_flag(vm, FLAG_AF, false);
    set_flag(vm, FLAG_DF, false);
    set_flag(vm, FLAG_IF, false);
    set_flag(vm, FLAG_OF, false);
    set_flag(vm, FLAG_PF, false);

}

static void test(VM* vm){
    // test
    uint32_t f_address = get_address(read_segreg16(vm, CS), read_ip16(vm));
    
   // 1. MOV AX, 1234h (אופקוד B8, ואז Low byte ואז High byte)
    write_mem8(vm, f_address,     0xB8);
    write_mem8(vm, f_address + 1, 0x34);
    write_mem8(vm, f_address + 2, 0x12);

    // 2. PUSH AX (אופקוד 0x50) - דוחף את AX למחסנית
    write_mem8(vm, f_address + 3, 0x50);

    // 3. MOV AX, 0000h (נאפס את AX כדי לוודא שה-POP באמת מחזיר את הערך)
    write_mem8(vm, f_address + 4, 0xB8);
    write_mem8(vm, f_address + 5, 0x00);
    write_mem8(vm, f_address + 6, 0x00);

    // 4. POP BX (אופקוד 0x5B) - שולף מהמחסנית הישר אל תוך BX (אינדקס 3)
    write_mem8(vm, f_address + 7, 0x5B);


   
}
int main(){
    
    init_opcode_table();
    VM* vm = create_vm();;
    turn_on_vm(vm);

    printf("AX: %04X\n", read_reg16(vm, AX));
    printf("BX: %04X\n", read_reg16(vm, BX));
    printf("SP: %04X\n -------- \n", read_reg16(vm, SP));
    // prapare all machine here!
    prepare_segment_registers(vm);
    
    test(vm);

    run_vm(vm);

    printf("AX: %04X\n", read_reg16(vm, AX));
    printf("BX: %04X\n", read_reg16(vm, BX));
    printf("SP: %04X\n", read_reg16(vm, SP));
    destroy_vm(vm);
    return 0;
}

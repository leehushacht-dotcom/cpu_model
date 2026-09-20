#include "vm.h"
#include "opcodes.h"
#include "hal.h"
#include "internal.h"

VM* create_vm(void){
    VM* vm = (VM*)calloc(1, sizeof(struct VM)); 
    // check here - > need to think how to operate false
    if (!vm) return NULL;
    return vm;
}
void destroy_vm(VM* vm)
{
    if (vm != NULL){
        free(vm);
    }
}

uint8_t fetch_byte(VM* vm){
    uint32_t cur_address = get_address(read_segreg16(vm, CS), read_ip16(vm));
    uint8_t code = read_mem8(vm, cur_address); // byte
    write_ip16(vm, read_ip16(vm) + 1);
    return code;
}
uint16_t fetch_word(VM* vm){
    uint8_t low_byte = fetch_byte(vm);
    uint8_t high_byte = fetch_byte(vm);
    return (uint16_t)low_byte | ((uint16_t)high_byte << 8);
}

void turn_on_vm(VM* vm){
    vm->is_running = true;
}
void turn_off_vm(VM* vm){
    vm->is_running = false;
}
bool is_vm_running(VM* vm){
    return vm->is_running;
}
void halt_vm(VM* vm){
    vm->is_halted = true;
}
void resume_vm(VM* vm){
    vm->is_halted = false;
}
bool is_vm_halted(VM* vm){
    return vm->is_halted;
}
void PUSH_16(VM* vm, uint16_t value){
    write_reg16(vm, SP, read_reg16(vm, SP) - 2);
    write_mem16(vm, get_address(read_segreg16(vm, SS), read_reg16(vm, SP)), value);

}
uint16_t POP_16(VM* vm){
    uint16_t val = read_mem16(vm, get_address(read_segreg16(vm, SS), read_reg16(vm, SP)));
    write_reg16(vm, SP, read_reg16(vm, SP) + 2);
    return val;
}

void run_vm(VM* vm){
    while (is_vm_running(vm))
    {
        // check for interrups

        // check hlt
        if(is_vm_halted(vm)) break; // later just continue when intterups will add
        // fetch
        uint8_t opcode = fetch_byte(vm);
        opcode_main_handle_list[opcode](vm, opcode);
    }
}
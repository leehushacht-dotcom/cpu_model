#include "vm.h"
#include "opcodes.h"
#include "hal.h"


static void print_state(const VM* vm){
    printf("AX=%04X BX=%04X CX=%04X DX=%04X SI=%04X DI=%04X SP=%04X ES=%04X IP=%04X\n",
        read_reg16(vm, AX), read_reg16(vm, BX), read_reg16(vm, CX), read_reg16(vm, DX),
        read_reg16(vm, SI), read_reg16(vm, DI), read_reg16(vm, SP),
        read_segreg16(vm, ES), read_ip16(vm));
    printf("CF=%d ZF=%d SF=%d OF=%d PF=%d AF=%d\n",
        get_flag(vm, FLAG_CF), get_flag(vm, FLAG_ZF), get_flag(vm, FLAG_SF),
        get_flag(vm, FLAG_OF), get_flag(vm, FLAG_PF), get_flag(vm, FLAG_AF));
    printf("mem[DS:0200]=%04X mem[DS:0202]=%02X mem[DS:0210]=%04X\n",
        read_mem16(vm, get_address(read_segreg16(vm, DS), 0x0200)),
        read_mem8 (vm, get_address(read_segreg16(vm, DS), 0x0202)),
        read_mem16(vm, get_address(read_segreg16(vm, DS), 0x0210)));
}

static void test(VM* vm){
    uint32_t a = get_address(read_segreg16(vm, CS), read_ip16(vm));
    const uint8_t prog[] = {
        // --- MOV immediate ---
        0xB8, 0x34, 0x12,       // MOV AX, 1234h
        0xB3, 0xAB,             // MOV BL, ABh
        0xB7, 0xCD,             // MOV BH, CDh          -> BX=CDAB
        // --- stack ---
        0x50,                   // PUSH AX
        0x53,                   // PUSH BX
        0x58,                   // POP AX               -> AX=CDAB
        0x5B,                   // POP BX               -> BX=1234   (swapped)
        // --- MOV reg<->reg via ModRM ---
        0x89, 0xC1,             // MOV CX, AX           -> CX=CDAB
        0x8A, 0xE1,             // MOV AH, CL           -> AX=ABAB
        // --- MOV to/from memory, several addressing modes ---
        0xBE, 0x00, 0x02,       // MOV SI, 0200h
        0x89, 0x0C,             // MOV [SI], CX         -> mem[0200]=CDAB
        0xC6, 0x44, 0x02, 0x5A, // MOV byte [SI+2], 5Ah -> mem[0202]=5A
        0x8B, 0x14,             // MOV DX, [SI]         -> DX=CDAB
        0xBF, 0x10, 0x02,       // MOV DI, 0210h
        0xC7, 0x05, 0x00, 0x10, // MOV word [DI], 1000h -> mem[0210]=1000
        // --- segment register moves ---
        0x8C, 0xD8,             // MOV AX, DS           -> AX=0700
        0x8E, 0xC0,             // MOV ES, AX
        0xB8, 0x00, 0x08,       // MOV AX, 0800h
        0x8E, 0xC0,             // MOV ES, AX           -> ES=0800
        // --- ALU, register forms, all 8 ops ---
        0xB8, 0xFF, 0x7F,       // MOV AX, 7FFFh
        0xBB, 0x01, 0x00,       // MOV BX, 0001h
        0x01, 0xD8,             // ADD AX, BX           -> AX=8000, OF=1
        0xB8, 0xFF, 0xFF,       // MOV AX, FFFFh
        0x01, 0xD8,             // ADD AX, BX           -> AX=0000, CF=1
        0xB9, 0x05, 0x00,       // MOV CX, 0005h
        0x11, 0xD9,             // ADC CX, BX           -> CX=5+1+CF = 0007
        0x29, 0xD9,             // SUB CX, BX           -> CX=0006
        0x39, 0xD9,             // CMP CX, BX           -> flags only
        0xB8, 0x00, 0x00,       // MOV AX, 0
        0x29, 0xD8,             // SUB AX, BX           -> AX=FFFF, CF=1
        0x19, 0xD9,             // SBB CX, BX           -> CX=6-1-CF = 0004
        0x21, 0xD9,             // AND CX, BX           -> CX=0000
        0x09, 0xD9,             // OR  CX, BX           -> CX=0001
        0x31, 0xC9,             // XOR CX, CX           -> CX=0000
        // --- ALU, reg <- memory and accumulator-immediate forms ---
        0x03, 0x0C,             // ADD CX, [SI]         -> CX=CDAB
        0x2B, 0x0D,             // SUB CX, [DI]         -> CX=BDAB
        0x04, 0x10,             // ADD AL, 10h          -> AL=0F
        0x2D, 0x0F, 0x00,       // SUB AX, 000Fh        -> AX=FF00
        // --- group 80/81/83 ---
        0x80, 0xC3, 0x10,       // ADD BL, 10h          -> BL=11
        0x81, 0xEB, 0x10, 0x00, // SUB BX, 0010h        -> BX=0001
        0x83, 0xC3, 0xFE,       // ADD BX, -2           -> BX=FFFF
        0x83, 0x3C, 0x00,       // CMP word [SI], 0
        0x80, 0x7C, 0x02, 0x5A, // CMP byte [SI+2], 5Ah -> ZF=1
        // --- INC/DEC, all three forms, CF preserved ---
        0x05, 0xFF, 0x00,       // ADD AX, 00FFh        -> AX=FFFF, CF=0
        0x40,                   // INC AX               -> AX=0000, CF still 0
        0x4B,                   // DEC BX               -> BX=FFFE
        0xFE, 0xC3,             // INC BL               -> BX=FFFF
        0xFF, 0x0D,             // DEC word [DI]        -> mem[0210]=0FFF
        0xFE, 0x4C, 0x02,       // DEC byte [SI+2]      -> mem[0202]=59
        // --- stop ---
        0xF4                    // HLT
    };
    for (size_t i = 0; i < sizeof prog; i++) write_mem8(vm, a + i, prog[i]);
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

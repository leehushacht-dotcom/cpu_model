#include "opcodes.h"
#include "hal.h"

typedef struct { 
    uint8_t mod, 
    reg, 
    rm; 
} ModRM;

// < ------------ Mod RM Area ------------ > //
static ModRM decodeModRM(VM *vm){
    // 00 = mod | 000 = reg | 000 = RM
    uint8_t cur_byte = fetch_byte(vm);
    ModRM m;
    m.mod = (cur_byte & 0xC0) >> 6;
    m.reg = (cur_byte & 0x38) >> 3;
    m.rm = (cur_byte & 0x07);
    return m;
}

static uint16_t modRMaddress(VM* vm, ModRM m){
    if (m.mod == 3){
        return 0;
    }
    return get_final_address(vm, m.mod, m.rm);
}

static uint16_t read_RM16(VM *vm, ModRM m, uint16_t address){
    if (m.mod == 3){
        return read_reg16(vm, m.reg);
    }
    return read_mem16(vm, address);
}

static void write_RM16(VM *vm, ModRM m, uint16_t address, uint16_t v){
    if (m.mod == 3){
        write_reg16(vm, m.reg, v);
    }
    else{
        write_mem16(vm, address, v);
    }
}
static uint8_t read_RM8(VM *vm, ModRM m, uint16_t address){
    if (m.mod == 3){
        return read_reg8(vm, m.reg);
    }
    return read_mem8(vm, address);
}

static void write_RM8(VM *vm, ModRM m, uint16_t address, uint8_t v){
    if (m.mod == 3){
        write_reg8(vm, m.reg, v);
    }
    else{
        write_mem8(vm, address, v);
    }
}
// < ------------ Mod RM Area ------------ > //

uint32_t get_final_address(VM* vm, uint8_t mod, uint8_t rm){
    uint16_t base_address = 0;
    uint32_t final_address = 0;
    uint8_t segment_reg_index = DS;
    switch (rm)
    {
        
        case 0: base_address = read_reg16(vm, BX) + read_reg16(vm, SI); break;
        case 1: base_address = read_reg16(vm, BX) + read_reg16(vm, DI); break;
        case 2: base_address = read_reg16(vm, BP) + read_reg16(vm, SI); segment_reg_index = SS; break;
        case 3: base_address = read_reg16(vm, BP) + read_reg16(vm, DI); segment_reg_index = SS; break;
        case 4: base_address = read_reg16(vm, SI); break;
        case 5: base_address = read_reg16(vm, DI); break;
        case 6:
        {
            if (mod == 0)
            {
                base_address = fetch_word(vm); break; // special *
            }
            else
            {
                 base_address = read_reg16(vm, BP); segment_reg_index = SS; break;
            }
        } 
        case 7: base_address = read_reg16(vm, BX); break;      
    }
    switch (mod)
    {
        case 1: {
            int8_t disp8 = (int8_t)fetch_byte(vm); // 0xFE הופך למינוס 2 אמיתי
            base_address += disp8;
            break;
        }
        case 2: {
            int16_t disp16 = (int16_t)fetch_word(vm); 
            base_address += disp16;
            break;
        }
    }
    
    final_address = get_address(read_segreg16(vm, segment_reg_index), base_address); // later do read_reg16(vm, BX) for segreg also

    return final_address;
}

// B0 -> B7
void MOV_imm8_to_REG(VM* vm){
    uint8_t reg_i = read_cur_opcode8(vm) & 0x07; // later do read current opcode also!
    uint8_t imm8 = fetch_byte(vm);
    write_reg8(vm, reg_i, imm8);
}
// B8 -> BF
void MOV_imm16_to_REG(VM* vm){
    
    uint8_t reg_i = read_cur_opcode8(vm) & 0x07;
    uint16_t imm16 = fetch_word(vm);
    write_reg16(vm, reg_i, imm16);
}

// 88
static MOV_r8_to_rm8(VM* vm){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM8(vm, m, final_address, read_reg8(vm, m.reg));
}
// 8A
void MOV_rm8_to_r8(VM* vm){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_reg8(vm, m.reg, read_RM8(vm, m, final_address));
}
// 89
void MOV_r16_to_rm16(VM* vm){
    // mod => 2 high bits | 11 = target is reg, 00 target is memory, 01 memory + jump(*[bx+5]) 8 bits, 10 big jump 16 bits
    // reg => 3 middle bits
    // target(reg/memory) => 3 low bits
    uint8_t cur_byte = fetch_byte(vm);
    uint8_t mod = (cur_byte & 0xC0) >> 6; // 11000000
    uint8_t reg_i = (cur_byte & 0x38) >> 3; // 00111000
    uint8_t target_rm = (cur_byte & 0x07); // 00000111
    uint32_t final_address = 0;
    if (mod != 3)
    {
        final_address = get_final_address(vm, mod, target_rm);
        write_mem16(vm, final_address, read_reg16(vm, reg_i));
    }
    else
    {
        write_reg16(vm, target_rm, read_reg16(vm, reg_i));
    }
}
// 8B
void MOV_rm16_to_r16(VM* vm){
    // mod => 2 high bits | 11 = target is reg, 00 target is memory, 01 memory + jump(*[bx+5]) 8 bits, 10 big jump 16 bits
    // reg => 3 middle bits
    // target(reg/memory) => 3 low bits
    uint8_t cur_byte = fetch_byte(vm);
    uint8_t mod = (cur_byte & 0xC0) >> 6; // 11000000
    uint8_t reg_i = (cur_byte & 0x38) >> 3; // 00111000
    uint8_t target_rm = (cur_byte & 0x07); // 00000111
    uint32_t final_address = 0;
    if (mod != 3)
    {
        final_address = get_final_address(vm, mod, target_rm);
        write_reg16(vm, reg_i, read_mem16(vm, final_address));
    }
    else
    {
        write_reg16(vm, reg_i, read_reg16(vm, target_rm));
    }
}
// C7
void MOV_imm16_to_rm16(VM* vm){
    // add mod/rm func instead!
    uint8_t cur_byte = fetch_byte(vm);
    uint8_t mod = (cur_byte & 0xC0) >> 6; // 11000000
    // reg has to be 000! 
    uint8_t rm = (cur_byte & 0x07); // 00000111
    uint32_t final_address = 0;
    if (mod != 3)
    {
        final_address = get_final_address(vm, mod, rm);
        uint16_t imm16 = fetch_word(vm);
        write_mem16(vm, final_address, imm16);
    }
    else
    {
        write_reg16(vm, rm, fetch_word(vm));
    }
}
// C6
void MOV_imm8_to_rm8(VM* vm){
    // C6 C0 55
    uint8_t cur_byte = fetch_byte(vm);
    uint8_t mod = (cur_byte & 0xC0) >> 6; // 11000000
    // reg has to be 000! 
    uint8_t rm = (cur_byte & 0x07); // 00000111
    uint32_t final_address = 0;
    if (mod != 3)
    {
        final_address = get_final_address(vm, mod, rm);
        uint8_t imm8 = fetch_byte(vm);
        write_mem8(vm, final_address, imm8);
    }
    else
    {
        uint8_t imm8 = fetch_byte(vm);
        write_reg8(vm, rm, imm8);     
    }
}

// 8C
void MOV_segreg_to_rm16(VM* vm){
    // mod => 2 high bits | 11 = target is reg, 00 target is memory, 01 memory + jump(*[bx+5]) 8 bits, 10 big jump 16 bits
    // reg => 3 middle bits
    // target(reg/memory) => 3 low bits
    uint8_t cur_byte = fetch_byte(vm);
    uint8_t mod = (cur_byte & 0xC0) >> 6; // 11000000
    uint8_t seg_reg_i = (cur_byte & 0x38) >> 3; // 00111000
    uint8_t target_rm = (cur_byte & 0x07); // 00000111
    uint32_t final_address = 0;
    if (mod != 3)
    {
        final_address = get_final_address(vm, mod, target_rm);
        write_mem16(vm, final_address, read_segreg16(vm, seg_reg_i));
    }
    else
    {
        write_reg16(vm, target_rm, read_segreg16(vm, seg_reg_i));
    }
}
// 8E
void MOV_rm16_to_segreg(VM* vm){
    // mod => 2 high bits | 11 = target is reg, 00 target is memory, 01 memory + jump(*[bx+5]) 8 bits, 10 big jump 16 bits
    // reg => 3 middle bits
    // target(reg/memory) => 3 low bits
    uint8_t cur_byte = fetch_byte(vm);
    uint8_t mod = (cur_byte & 0xC0) >> 6; // 11000000
    uint8_t seg_reg_i = (cur_byte & 0x38) >> 3; // 00111000
    uint8_t target_rm = (cur_byte & 0x07); // 00000111
    uint32_t final_address = 0;
    if (mod != 3)
    {
        final_address = get_final_address(vm, mod, target_rm);
        write_segreg16(vm, seg_reg_i, read_mem16(vm, final_address));
    }
    else
    {
        write_segreg16(vm, seg_reg_i, read_reg16(vm, target_rm));
    }
    // ** when mov ss or mov cs -> there are some actions -> check later!

}
// 50 - 57
void PUSH_reg16(VM* vm){
    uint8_t reg_i = (read_cur_opcode8(vm) & 0x07);
    PUSH_16(vm, read_reg16(vm, reg_i));
}

// 58 - 5F
void POP_reg16(VM* vm){
    uint8_t reg_i = (read_cur_opcode8(vm) & 0x07);
    write_reg16(vm, reg_i, POP_16(vm));
}

InstructionHandler opcode_main_handle_list[256];
void handle_unknown_opcode(VM* vm) {
    printf("Error: Unimplemented Opcode at IP: %04X\n", read_ip16(vm) - 1);
    turn_off_vm(vm);
}

// 3. פונקציית האתחול - נקראת פעם אחת בתחילת התוכנית!
void init_opcode_table() {
    // fill with error func
    for (int i = 0; i < 256; i++) {
        opcode_main_handle_list[i] = handle_unknown_opcode;
    }
    // fill with opcodes
    for (int i = 0xB0; i <= 0xB7; i++){
        // complete
        opcode_main_handle_list[i] = MOV_imm8_to_REG; 
    }
    for (int i = 0xB8; i <= 0xBF; i++){
        // complete
        opcode_main_handle_list[i] = MOV_imm16_to_REG;
    }
    for (int i = 0x50; i <= 0x57; i++){
        // complete
        opcode_main_handle_list[i] = PUSH_reg16;
    }
    for (int i = 0x58; i <= 0x5F; i++){
        // complete
        opcode_main_handle_list[i] = POP_reg16;
    }
    // complete
    opcode_main_handle_list[0x89] = MOV_r16_to_rm16;
    opcode_main_handle_list[0x8B] = MOV_rm16_to_r16;
    opcode_main_handle_list[0xC7] = MOV_imm16_to_rm16;
    opcode_main_handle_list[0x88] = MOV_r8_to_rm8;
    opcode_main_handle_list[0x8A] = MOV_rm8_to_r8;
    opcode_main_handle_list[0xC6] = MOV_imm8_to_rm8;
    opcode_main_handle_list[0x8C] = MOV_segreg_to_rm16;
    opcode_main_handle_list[0x8E] = MOV_rm16_to_segreg;


    // ...
}
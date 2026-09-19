#include "opcodes.h"
#include "hal.h"
#include "alu.h"



typedef struct { 
    uint8_t mod, 
    reg, 
    rm; 
} ModRM;

static alu_operands_handle8  alu_op8[8];    // add8, or8, adc8, sbb8, and8, sub8, xor8, cmp8
static alu_operands_handle16 alu_op16[8];   // same, 16-bit

static uint32_t get_final_address(VM* vm, uint8_t mod, uint8_t rm){
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

static uint32_t modRMaddress(VM* vm, ModRM m){
    if (m.mod == 3){
        return 0;
    }
    return get_final_address(vm, m.mod, m.rm);
}

static uint16_t read_RM16(VM *vm, ModRM m, uint32_t address){
    if (m.mod == 3){
        return read_reg16(vm, m.rm);
    }
    return read_mem16(vm, address);
}

static void write_RM16(VM *vm, ModRM m, uint32_t address, uint16_t v){
    if (m.mod == 3){
        write_reg16(vm, m.rm, v);
    }
    else{
        write_mem16(vm, address, v);
    }
}
static uint8_t read_RM8(VM *vm, ModRM m, uint32_t address){
    if (m.mod == 3){
        return read_reg8(vm, m.rm);
    }
    return read_mem8(vm, address);
}

static void write_RM8(VM *vm, ModRM m, uint32_t address, uint8_t v){
    if (m.mod == 3){
        write_reg8(vm, m.rm, v);
    }
    else{
        write_mem8(vm, address, v);
    }
}
// < ------------ Mod RM Area ------------ > //


// B0 -> B7
void MOV_imm8_to_REG(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = cur_opcode & 0x07;
    uint8_t imm8 = fetch_byte(vm);
    write_reg8(vm, reg_i, imm8);
}
// B8 -> BF
void MOV_imm16_to_REG(VM* vm, uint8_t cur_opcode){ 
    uint8_t reg_i = cur_opcode & 0x07;
    uint16_t imm16 = fetch_word(vm);
    write_reg16(vm, reg_i, imm16);
}

// 88
void MOV_r8_to_rm8(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM8(vm, m, final_address, read_reg8(vm, m.reg));
}
// 8A
void MOV_rm8_to_r8(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_reg8(vm, m.reg, read_RM8(vm, m, final_address));
}
// 89
void MOV_r16_to_rm16(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM16(vm, m, final_address, read_reg16(vm, m.reg));
}
// 8B
void MOV_rm16_to_r16(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_reg16(vm, m.reg, read_RM16(vm, m, final_address));
}
// C7
void MOV_imm16_to_rm16(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM16(vm, m, final_address, fetch_word(vm));
}
// C6
void MOV_imm8_to_rm8(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM8(vm, m, final_address, fetch_byte(vm));
}

// 8C
void MOV_segreg_to_rm16(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM16(vm, m, final_address, read_segreg16(vm, m.reg));
}
// 8E
void MOV_rm16_to_segreg(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_segreg16(vm, m.reg, read_RM16(vm, m, final_address));

    // ** when mov ss or mov cs -> there are some actions -> check later!
}
// 50 - 57
void PUSH_reg16(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = (cur_opcode & 0x07);
    PUSH_16(vm, read_reg16(vm, reg_i));
}

// 58 - 5F
void POP_reg16(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = (cur_opcode & 0x07);
    write_reg16(vm, reg_i, POP_16(vm));
}

InstructionHandler opcode_main_handle_list[256];
void handle_unknown_opcode(VM* vm, uint8_t cur_opcode) {
    (void)cur_opcode; // later add to the print
    printf("Error: Unimplemented Opcode at IP: %04X\n", read_ip16(vm) - 1);
    turn_off_vm(vm);
}


// alu operands:
uint8_t all_alu_operands_handle8(VM* vm, uint8_t cur_opcode, uint8_t a, uint8_t b){
    return alu_op8[(cur_opcode >> 3) & 0x07](vm, a, b);
}
uint16_t all_alu_operands_handle16(VM* vm, uint8_t cur_opcode, uint16_t a, uint16_t b){
    return alu_op16[(cur_opcode >> 3) & 0x07](vm, a, b);
}

void alu_rm8_r8(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM8(vm, m, final_address, all_alu_operands_handle8(vm, cur_opcode, read_RM8(vm, m, final_address), read_reg8(vm, m.reg)));
}
void alu_rm16_r16(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM16(vm, m, final_address, all_alu_operands_handle16(vm, cur_opcode, read_RM16(vm, m, final_address), read_reg16(vm, m.reg)));
}
void alu_r8_rm8(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_reg8(vm, m.reg, all_alu_operands_handle8(vm, cur_opcode, read_reg8(vm, m.reg), read_RM8(vm, m, final_address)));
}
void alu_r16_rm16(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_reg16(vm, m.reg, all_alu_operands_handle16(vm, cur_opcode, read_reg16(vm, m.reg), read_RM16(vm, m, final_address)));
}
void alu_al_imm8(VM* vm, uint8_t cur_opcode){
    write_reg8(vm, AL, all_alu_operands_handle8(vm, cur_opcode, read_reg8(vm, AL), fetch_byte(vm)));
}
void alu_ax_imm16(VM* vm, uint8_t cur_opcode){
    write_reg16(vm, AX, all_alu_operands_handle16(vm, cur_opcode, read_reg16(vm, AX), fetch_word(vm)));

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
    // in progress: 
    // -------------------
    alu_op8[0] = alu_add8;
    alu_op8[1] = alu_or8;
    alu_op8[2] = alu_adc8;
    alu_op8[3] = alu_sbb8;
    alu_op8[4] = alu_and8;
    alu_op8[5] = alu_sub8;
    alu_op8[6] = alu_xor8;
    alu_op8[7] = alu_cmp8;

    alu_op16[0] = alu_add16;
    alu_op16[1] = alu_or16;
    alu_op16[2] = alu_adc16;
    alu_op16[3] = alu_sbb16;
    alu_op16[4] = alu_and16;
    alu_op16[5] = alu_sub16;
    alu_op16[6] = alu_xor16;
    alu_op16[7] = alu_cmp16;
    // --------------------
    for (int row = 0; row < 8; row++){
        int base = row * 8;
        opcode_main_handle_list[base + 0] = alu_rm8_r8;
        opcode_main_handle_list[base + 1] = alu_rm16_r16;
        opcode_main_handle_list[base + 2] = alu_r8_rm8;
        opcode_main_handle_list[base + 3] = alu_r16_rm16;
        opcode_main_handle_list[base + 4] = alu_al_imm8;
        opcode_main_handle_list[base + 5] = alu_ax_imm16;
    }
    // opcode_main_handle_list[0xF4] = HLT;
}
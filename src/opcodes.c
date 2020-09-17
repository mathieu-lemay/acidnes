#include "opcodes.h"
#include "common.h"

uint8_t add(cpu_t *cpu, uint8_t reg, uint8_t val);
uint8_t sub(cpu_t *cpu, uint8_t reg, uint8_t val);
void cmp(cpu_t *cpu, uint8_t reg, uint8_t val);
void branch(cpu_t *cpu, int8_t offset);
uint8_t shift_left(cpu_t *cpu, uint8_t val);
uint8_t shift_right(cpu_t *cpu, uint8_t val);
uint8_t rotate_left(cpu_t *cpu, uint8_t val);
uint8_t rotate_right(cpu_t *cpu, uint8_t val);

uint8_t cpu_load_value(cpu_t *cpu);
void cpu_store_value(cpu_t *cpu, uint8_t val);

void ADC(cpu_t *cpu) {
    uint8_t val = cpu_load_value(cpu);

    cpu->A = add(cpu, cpu->A, val);
}

void SBC(cpu_t *cpu) {
    uint8_t val = cpu_load_value(cpu);

    cpu->A = sub(cpu, cpu->A, val);

    cpu_increment_cycles_if_page_crossed(cpu);
}

void AND(cpu_t *cpu) {
    cpu->A &= cpu_load_value(cpu);
    cpu_set_zero(cpu, cpu->A);
    cpu_set_negative(cpu, cpu->A);

    cpu_increment_cycles_if_page_crossed(cpu);
}

void ORA(cpu_t *cpu) {
    cpu->A |= cpu_load_value(cpu);
    cpu_set_zero(cpu, cpu->A);
    cpu_set_negative(cpu, cpu->A);

    cpu_increment_cycles_if_page_crossed(cpu);
}

void EOR(cpu_t *cpu) {
    cpu->A ^= cpu_load_value(cpu);
    cpu_set_zero(cpu, cpu->A);
    cpu_set_negative(cpu, cpu->A);

    cpu_increment_cycles_if_page_crossed(cpu);
}

void BIT(cpu_t *cpu) {
    uint8_t val = cpu_load_value(cpu);

    cpu_set_zero(cpu, cpu->A & val);
    cpu_set_negative(cpu, val & 0x80u);
    cpu_set_overflow(cpu, val & 0x40u);
}

void CMP(cpu_t *cpu) {
    uint8_t val = cpu_load_value(cpu);
    cmp(cpu, cpu->A, val);

    cpu_increment_cycles_if_page_crossed(cpu);
}

void CPX(cpu_t *cpu) {
    uint8_t val = cpu_load_value(cpu);
    cmp(cpu, cpu->X, val);
}

void CPY(cpu_t *cpu) {
    uint8_t val = cpu_load_value(cpu);
    cmp(cpu, cpu->Y, val);
}

void BEQ(cpu_t *cpu) {
    int8_t offset = (int8_t) cpu_load_value(cpu);
    if (cpu_flag_is_set(cpu, Z)) {
        branch(cpu, offset);
    }
}

void BMI(cpu_t *cpu) {
    int8_t offset = (int8_t) cpu_load_value(cpu);
    if (cpu_flag_is_set(cpu, N)) {
        branch(cpu, offset);
    }
}

void BNE(cpu_t *cpu) {
    int8_t offset = (int8_t) cpu_load_value(cpu);
    if (!cpu_flag_is_set(cpu, Z)) {
        branch(cpu, offset);
    }
}

void BPL(cpu_t *cpu) {
    int8_t offset = (int8_t) cpu_load_value(cpu);
    if (!cpu_flag_is_set(cpu, N)) {
        branch(cpu, offset);
    }
}

void BCC(cpu_t *cpu) {
    int8_t offset = (int8_t) cpu_load_value(cpu);
    if (!cpu_flag_is_set(cpu, C)) {
        branch(cpu, offset);
    }
}

void BCS(cpu_t *cpu) {
    int8_t offset = (int8_t) cpu_load_value(cpu);
    if (cpu_flag_is_set(cpu, C)) {
        branch(cpu, offset);
    }
}

void BVC(cpu_t *cpu) {
    int8_t offset = (int8_t) cpu_load_value(cpu);
    if (!cpu_flag_is_set(cpu, V)) {
        branch(cpu, offset);
    }
}

void BVS(cpu_t *cpu) {
    int8_t offset = (int8_t) cpu_load_value(cpu);
    if (cpu_flag_is_set(cpu, V)) {
        branch(cpu, offset);
    }
}

void CLC(cpu_t *cpu) {
    cpu_unset_flag(cpu, C);
}

void CLD(cpu_t *cpu) {
    cpu_unset_flag(cpu, D);
}

void CLI(cpu_t *cpu) {
    cpu_unset_flag(cpu, I);
}

void CLV(cpu_t *cpu) {
    cpu_unset_flag(cpu, V);
}

void JMP(cpu_t *cpu) {
    uint16_t addr = cpu_get_addr(cpu);

    _debug_log("JMP", "Jumping to: 0x%04x\n", addr);
    cpu->PC = addr;
}

void JSR(cpu_t *cpu) {
    /* Fetching address should increment PC by two, we would then need to decrement by one before pushing to
       the stack. Instead, we shortcut a bit and just increment by one. */
    uint16_t addr = cpu_get_u16(cpu, cpu->PC);
    cpu->PC++;

    cpu_push_u16(cpu, cpu->PC);

    _debug_log("JSR", "Jumping to sub-routine at: 0x%04x\n", addr);
    cpu->PC = addr;
}

void RTS(cpu_t *cpu) {
    uint16_t addr = cpu_pop_u16(cpu);
    _debug_log("RTS", "Return from sub-routine to: 0x%04x\n", addr);
    cpu->PC = addr;
    cpu->PC++;
}

void RTI(cpu_t *cpu) {
    uint8_t p = cpu_pop_u8(cpu);

    // Keep Unused (U) flag as is.
    p |= (uint8_t) (cpu->P & (uint8_t) U);
    cpu->P = p;

    cpu->PC = cpu_pop_u16(cpu);
}

void LDA(cpu_t *cpu) {
    uint8_t val = cpu_load_value(cpu);
    cpu->A = val;
    cpu_set_zero(cpu, cpu->A);
    cpu_set_negative(cpu, cpu->A);

    cpu_increment_cycles_if_page_crossed(cpu);
}

void LDX(cpu_t *cpu) {
    uint8_t val = cpu_load_value(cpu);
    cpu->X = val;
    cpu_set_zero(cpu, cpu->X);
    cpu_set_negative(cpu, cpu->X);

    cpu_increment_cycles_if_page_crossed(cpu);
}

void LDY(cpu_t *cpu) {
    uint8_t val = cpu_load_value(cpu);
    cpu->Y = val;
    cpu_set_zero(cpu, cpu->Y);
    cpu_set_negative(cpu, cpu->Y);

    cpu_increment_cycles_if_page_crossed(cpu);
}

void PHA(cpu_t *cpu) {
    cpu_push_u8(cpu, cpu->A);
}

void PHP(cpu_t *cpu) {
    // PHP always pushes the Break (B) flag as a `1` to the stack.
    cpu_push_u8(cpu, cpu->P | (uint8_t) B);
}

void PLA(cpu_t *cpu) {
    uint8_t val = cpu_pop_u8(cpu);
    cpu->A = val;
    cpu_set_zero(cpu, cpu->A);
    cpu_set_negative(cpu, cpu->A);
}

void PLP(cpu_t *cpu) {
    uint8_t p = cpu_pop_u8(cpu);

    // Remove the Break (B) flag.
    p &= (uint8_t) ~((uint8_t) B);

    // Keep Unused (U) flag as is.
    p |= (uint8_t) (cpu->P & (uint8_t) U);

    cpu->P = p;
}

void SEC(cpu_t *cpu) {
    cpu_set_flag(cpu, C);
}

void SED(cpu_t *cpu) {
    cpu_set_flag(cpu, D);
}

void SEI(cpu_t *cpu) {
    cpu_set_flag(cpu, I);
}

void STA(cpu_t *cpu) {
    cpu_store_value(cpu, cpu->A);
}

void STX(cpu_t *cpu) {
    cpu_store_value(cpu, cpu->X);
}

void STY(cpu_t *cpu) {
    cpu_store_value(cpu, cpu->Y);
}

void TAX(cpu_t *cpu) {
    cpu->X = cpu->A;
    cpu_set_zero(cpu, cpu->X);
    cpu_set_negative(cpu, cpu->X);
}

void TAY(cpu_t *cpu) {
    cpu->Y = cpu->A;
    cpu_set_zero(cpu, cpu->Y);
    cpu_set_negative(cpu, cpu->Y);
}

void TSX(cpu_t *cpu) {
    cpu->X = cpu->SP;
    cpu_set_zero(cpu, cpu->X);
    cpu_set_negative(cpu, cpu->X);
}

void TXA(cpu_t *cpu) {
    cpu->A = cpu->X;
    cpu_set_zero(cpu, cpu->A);
    cpu_set_negative(cpu, cpu->A);
}

void TXS(cpu_t *cpu) {
    cpu->SP = cpu->X;
}

void TYA(cpu_t *cpu) {
    cpu->A = cpu->Y;
    cpu_set_zero(cpu, cpu->A);
    cpu_set_negative(cpu, cpu->A);
}

void INC(cpu_t *cpu) {
    uint16_t addr = cpu_get_addr(cpu);
    uint8_t val = cpu_get_u8(cpu, addr) + 1;
    cpu_set_u8(cpu, addr, val);

    cpu_set_zero(cpu, val);
    cpu_set_negative(cpu, val);
}

void INX(cpu_t *cpu) {
    cpu->X++;
    cpu_set_zero(cpu, cpu->X);
    cpu_set_negative(cpu, cpu->X);
}

void INY(cpu_t *cpu) {
    cpu->Y++;
    cpu_set_zero(cpu, cpu->Y);
    cpu_set_negative(cpu, cpu->Y);
}

void DEC(cpu_t *cpu) {
    uint16_t addr = cpu_get_addr(cpu);
    uint8_t val = cpu_get_u8(cpu, addr) - 1;
    cpu_set_u8(cpu, addr, val);

    cpu_set_zero(cpu, val);
    cpu_set_negative(cpu, val);
}

void DEX(cpu_t *cpu) {
    cpu->X--;
    cpu_set_zero(cpu, cpu->X);
    cpu_set_negative(cpu, cpu->X);
}

void DEY(cpu_t *cpu) {
    cpu->Y--;
    cpu_set_zero(cpu, cpu->Y);
    cpu_set_negative(cpu, cpu->Y);
}

void ASL(cpu_t *cpu) {
    uint8_t val;
    uint16_t addr = 0;

    if (cpu->addr_mode == ACCUMULATOR) {
        val = cpu->A;
    } else {
        addr = cpu_get_addr(cpu);
        val = cpu_get_u8(cpu, addr);
    }

    val = shift_left(cpu, val);

    if (cpu->addr_mode == ACCUMULATOR) {
        cpu->A = val;
    } else {
        cpu_set_u8(cpu, addr, val);
    }
}

void LSR(cpu_t *cpu) {
    uint8_t val;
    uint16_t addr = 0;

    if (cpu->addr_mode == ACCUMULATOR) {
        val = cpu->A;
    } else {
        addr = cpu_get_addr(cpu);
        val = cpu_get_u8(cpu, addr);
    }

    val = shift_right(cpu, val);

    if (cpu->addr_mode == ACCUMULATOR) {
        cpu->A = val;
    } else {
        cpu_set_u8(cpu, addr, val);
    }
}

void ROL(cpu_t *cpu) {
    uint8_t val;
    uint16_t addr = 0;

    if (cpu->addr_mode == ACCUMULATOR) {
        val = cpu->A;
    } else {
        addr = cpu_get_addr(cpu);
        val = cpu_get_u8(cpu, addr);
    }

    val = rotate_left(cpu, val);

    if (cpu->addr_mode == ACCUMULATOR) {
        cpu->A = val;
    } else {
        cpu_set_u8(cpu, addr, val);
    }
}

void ROR(cpu_t *cpu) {
    uint8_t val;
    uint16_t addr = 0;

    if (cpu->addr_mode == ACCUMULATOR) {
        val = cpu->A;
    } else {
        addr = cpu_get_addr(cpu);
        val = cpu_get_u8(cpu, addr);
    }

    val = rotate_right(cpu, val);

    if (cpu->addr_mode == ACCUMULATOR) {
        cpu->A = val;
    } else {
        cpu_set_u8(cpu, addr, val);
    }
}

void NOP(cpu_t *cpu) {
    /* Does nothing, just uses cycles and increment PC */
    cpu_get_addr(cpu);

    cpu_increment_cycles_if_page_crossed(cpu);
}

void BRK(cpu_t *cpu) {
    cpu->PC++;
    cpu_push_u16(cpu, cpu->PC);

    cpu_set_flag(cpu, B);

    cpu_push_u8(cpu, cpu->P);

    cpu->PC = cpu_get_u16(cpu, IRQ_VECTOR);
}

void LAX(cpu_t *cpu) {
    uint8_t val = cpu_load_value(cpu);
    cpu->A = val;
    cpu->X = val;
    cpu_set_zero(cpu, cpu->A);
    cpu_set_negative(cpu, cpu->A);

    cpu_increment_cycles_if_page_crossed(cpu);
}

void SAX(cpu_t *cpu) {
    cpu_store_value(cpu, cpu->A & cpu->X);
}

void DCP(cpu_t *cpu) {
    uint16_t addr = cpu_get_addr(cpu);
    uint8_t val = cpu_get_u8(cpu, addr) - 1;
    cpu_set_u8(cpu, addr, val);

    cmp(cpu, cpu->A, val);
}

void ISB(cpu_t *cpu) {
    uint16_t addr = cpu_get_addr(cpu);
    uint8_t val = cpu_get_u8(cpu, addr) + 1;
    cpu_set_u8(cpu, addr, val);

    cpu->A = sub(cpu, cpu->A, val);
}

void RLA(cpu_t *cpu) {
    uint16_t addr = cpu_get_addr(cpu);
    uint8_t val = cpu_get_u8(cpu, addr);

    val = rotate_left(cpu, val);

    cpu_set_u8(cpu, addr, val);

    cpu->A &= val;
    cpu_set_zero(cpu, cpu->A);
    cpu_set_negative(cpu, cpu->A);
}

void RRA(cpu_t *cpu) {
    uint16_t addr = cpu_get_addr(cpu);
    uint8_t val = cpu_get_u8(cpu, addr);

    val = rotate_right(cpu, val);

    cpu_set_u8(cpu, addr, val);

    cpu->A = add(cpu, cpu->A, val);
}

void SLO(cpu_t *cpu) {
    uint16_t addr = cpu_get_addr(cpu);
    uint8_t val = cpu_get_u8(cpu, addr);

    val = shift_left(cpu, val);

    cpu_set_u8(cpu, addr, val);

    cpu->A |= val;
    cpu_set_zero(cpu, cpu->A);
    cpu_set_negative(cpu, cpu->A);
}

void SRE(cpu_t *cpu) {
    uint16_t addr = cpu_get_addr(cpu);
    uint8_t val = cpu_get_u8(cpu, addr);

    val = shift_right(cpu, val);

    cpu_set_u8(cpu, addr, val);

    cpu->A ^= val;
    cpu_set_zero(cpu, cpu->A);
    cpu_set_negative(cpu, cpu->A);
}

uint8_t add(cpu_t *cpu, uint8_t reg, uint8_t val) {
    uint8_t res = reg + val;
    if (cpu_flag_is_set(cpu, C)) {
        res++;
    }

    cpu_set_carry(cpu, res <= reg && val > 0 ? 1 : 0);

    // http://www.6502.org/tutorials/vflag.html
    uint8_t sign_a = get_bit_at(reg, 7);
    uint8_t sign_b = get_bit_at(val, 7);
    uint8_t sign_r = get_bit_at(res, 7);
    cpu_set_overflow(cpu, (sign_a == sign_b) && (sign_a != sign_r) ? 1 : 0);

    cpu_set_zero(cpu, res);
    cpu_set_negative(cpu, res);

    return res;
}

uint8_t sub(cpu_t *cpu, uint8_t reg, uint8_t val) {
    uint8_t res = reg - val;
    if (!cpu_flag_is_set(cpu, C)) {
        res--;
    }

    // If we needed to borrow, clear the flag, otherwise, set it.
    cpu_set_carry(cpu, val > reg ? 0 : 1);

    // http://www.6502.org/tutorials/vflag.html
    uint8_t sign_a = get_bit_at(reg, 7);
    uint8_t sign_r = get_bit_at(res, 7);
    cpu_set_overflow(cpu, (int8_t) res > (int8_t) reg && sign_a != sign_r ? 1 : 0);

    cpu_set_zero(cpu, res);
    cpu_set_negative(cpu, res);

    return res;
}

void cmp(cpu_t *cpu, uint8_t reg, uint8_t val) {
    uint8_t res = reg - val;

    cpu_set_carry(cpu, reg >= val ? 1 : 0);
    cpu_set_zero(cpu, res);
    cpu_set_negative(cpu, res);
}

void branch(cpu_t *cpu, int8_t offset) {
    uint16_t pc = cpu->PC;
    cpu->PC += offset;

    if (!addr_are_same_page(pc, cpu->PC)) {
        cpu->instr_cycles += 2;
    } else {
        cpu->instr_cycles++;
    }
}

uint8_t shift_left(cpu_t *cpu, uint8_t val) {
    uint8_t carry = val & 0x80u;
    val <<= 1u;

    cpu_set_zero(cpu, val);
    cpu_set_negative(cpu, val);
    cpu_set_carry(cpu, carry);

    return val;
}

uint8_t shift_right(cpu_t *cpu, uint8_t val) {
    uint8_t carry = val & 0x01u;
    val >>= 1u;

    cpu_set_zero(cpu, val);
    cpu_set_negative(cpu, val);
    cpu_set_carry(cpu, carry);

    return val;
}

uint8_t rotate_left(cpu_t *cpu, uint8_t val) {
    uint8_t carry = val & 0x80u;
    val <<= 1u;
    if (cpu_flag_is_set(cpu, C)) {
        val |= (uint8_t) 0x01u;
    }

    cpu_set_zero(cpu, val);
    cpu_set_negative(cpu, val);
    cpu_set_carry(cpu, carry);

    return val;
}

uint8_t rotate_right(cpu_t *cpu, uint8_t val) {
    uint8_t carry = val & 0x01u;
    val >>= 1u;
    if (cpu_flag_is_set(cpu, C)) {
        val |= (uint8_t) 0x80u;
    }

    cpu_set_zero(cpu, val);
    cpu_set_negative(cpu, val);
    cpu_set_carry(cpu, carry);

    return val;
}

uint8_t cpu_load_value(cpu_t *cpu) {
    uint16_t addr = cpu_get_addr(cpu);
    return cpu_get_u8(cpu, addr);
}

void cpu_store_value(cpu_t *cpu, uint8_t val) {
    uint16_t addr = cpu_get_addr(cpu);
    cpu_set_u8(cpu, addr, val);
}


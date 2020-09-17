#ifndef __CPU_H__
#define __CPU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "ppu.h"

enum addr_mode {
    IMMEDIATE,
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    IMPLIED,
    ACCUMULATOR,
    INDIRECT,
    INDIRECT_X,
    INDIRECT_Y,
    RELATIVE
};
typedef enum addr_mode addr_mode_t;

struct cpu_s {
    uint16_t PC; /* Program Counter */
    uint8_t SP; /* Stack Pointer */
    uint8_t A;  /* Accumulator */
    uint8_t X;  /* Index Register X */
    uint8_t Y;  /* Index Register Y */
    uint8_t P;  /* Processor Status (see Flags) */

    uint8_t ram[0x0800];
    uint8_t sram[0x2000];

    ppu_t *ppu;
    uint64_t clock;

    addr_mode_t addr_mode;
    uint8_t instr_cycles;
};
typedef struct cpu_s cpu_t;

enum cpu_flags {
    C = 0x01, /* Carry */
    Z = 0x02, /* Zero */
    I = 0x04, /* Interrupt Disable */
    D = 0x08, /* Decimal Mode */
    B = 0x10, /* Break Command */
    U = 0x20, /* Unused */
    V = 0x40, /* Overflow */
    N = 0x80  /* Negative */
};
typedef enum cpu_flags Flag;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
static const uint16_t STACK_OFFSET = 0x100;
static const uint16_t NMI_VECTOR = 0xFFFA;
#pragma clang diagnostic pop
static const uint16_t RESET_VECTOR = 0xFFFC;
static const uint16_t IRQ_VECTOR = 0xFFFE;

static const char *OPCODES[256] = {
//      00     01     02     03     04     05     06     07     08     09     0A     0B     0C     0D     0E     0F
/*00*/  "BRK", "ORA", "BAD", "SLO", "NOP", "ORA", "ASL", "SLO", "PHP", "ORA", "ASL", "NOP", "NOP", "ORA", "ASL", "SLO",  /*00*/
/*10*/  "BPL", "ORA", "BAD", "SLO", "NOP", "ORA", "ASL", "SLO", "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",  /*10*/
/*20*/  "JSR", "AND", "BAD", "RLA", "BIT", "AND", "ROL", "RLA", "PLP", "AND", "ROL", "NOP", "BIT", "AND", "ROL", "RLA",  /*20*/
/*30*/  "BMI", "AND", "BAD", "RLA", "NOP", "AND", "ROL", "RLA", "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",  /*30*/
/*40*/  "RTI", "EOR", "BAD", "SRE", "NOP", "EOR", "LSR", "SRE", "PHA", "EOR", "LSR", "NOP", "JMP", "EOR", "LSR", "SRE",  /*40*/
/*50*/  "BVC", "EOR", "BAD", "SRE", "NOP", "EOR", "LSR", "SRE", "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",  /*50*/
/*60*/  "RTS", "ADC", "BAD", "RRA", "NOP", "ADC", "ROR", "RRA", "PLA", "ADC", "ROR", "NOP", "JMP", "ADC", "ROR", "RRA",  /*60*/
/*70*/  "BVS", "ADC", "BAD", "RRA", "NOP", "ADC", "ROR", "RRA", "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",  /*70*/
/*80*/  "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX", "DEY", "NOP", "TXA", "NOP", "STY", "STA", "STX", "SAX",  /*80*/
/*90*/  "BCC", "STA", "BAD", "NOP", "STY", "STA", "STX", "SAX", "TYA", "STA", "TXS", "NOP", "NOP", "STA", "NOP", "NOP",  /*90*/
/*A0*/  "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX", "TAY", "LDA", "TAX", "NOP", "LDY", "LDA", "LDX", "LAX",  /*A0*/
/*B0*/  "BCS", "LDA", "BAD", "LAX", "LDY", "LDA", "LDX", "LAX", "CLV", "LDA", "TSX", "NOP", "LDY", "LDA", "LDX", "LAX",  /*B0*/
/*C0*/  "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP", "INY", "CMP", "DEX", "NOP", "CPY", "CMP", "DEC", "DCP",  /*C0*/
/*D0*/  "BNE", "CMP", "BAD", "DCP", "NOP", "CMP", "DEC", "DCP", "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",  /*D0*/
/*E0*/  "CPX", "SBC", "NOP", "ISB", "CPX", "SBC", "INC", "ISB", "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISB",  /*E0*/
/*F0*/  "BEQ", "SBC", "BAD", "ISB", "NOP", "SBC", "INC", "ISB", "SED", "SBC", "NOP", "ISB", "NOP", "SBC", "INC", "ISB",  /*F0*/
};

cpu_t *cpu_init(ppu_t *ppu);
void cpu_free(cpu_t *cpu);
void cpu_reset(cpu_t *cpu);
uint8_t cpu_tick(cpu_t *cpu);
void cpu_interrupt(cpu_t *cpu);

/* Read / Write RAM */
uint8_t cpu_get_u8(cpu_t *cpu, uint16_t addr);
uint16_t cpu_get_u16(cpu_t *cpu, uint16_t addr);
void cpu_set_u8(cpu_t *cpu, uint16_t addr, uint8_t val);
uint16_t cpu_get_addr(cpu_t *cpu);

/* Stack */
void cpu_push_u8(cpu_t *cpu, uint8_t val);
void cpu_push_u16(cpu_t *cpu, uint16_t val);
uint8_t cpu_pop_u8(cpu_t *cpu);
uint16_t cpu_pop_u16(cpu_t *cpu);

/* Flags */
void cpu_set_carry(cpu_t *cpu, uint8_t val);
void cpu_set_overflow(cpu_t *cpu, uint8_t val);
void cpu_set_negative(cpu_t *cpu, uint8_t val);
void cpu_set_zero(cpu_t *cpu, uint8_t val);
void cpu_set_flag(cpu_t *cpu, Flag flag);
void cpu_unset_flag(cpu_t *cpu, Flag flag);
bool cpu_flag_is_set(cpu_t *cpu, Flag flag);

/* Paging */
void cpu_increment_cycles_if_page_crossed(cpu_t *cpu);
bool addr_are_same_page(uint16_t val_a, uint16_t addr_b);

#ifdef __cplusplus
}
#endif
#endif /* __CPU_H__ */

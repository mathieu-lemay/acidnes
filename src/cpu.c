#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "opcodes.h"
#include "common.h"
#include "mapper.h"

bool page_crossed;

uint8_t get_opcode(cpu_t *cpu);
uint8_t get_addr_page(uint16_t addr);

/* Debug */
void dump_state(cpu_t *cpu);

cpu_t *cpu_init(ppu_t *ppu) {
    cpu_t *cpu = malloc(sizeof(cpu_t));

    if (cpu == NULL) {
        return cpu;
    }

    cpu->PC = 0;
    cpu->SP = 0xff;
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->P = 0;

    cpu->clock = 0;

    cpu->ppu = ppu;

    return cpu;
}

void cpu_reset(cpu_t *cpu) {
    cpu->SP = 0xfd;
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->P = (uint8_t) U | (uint8_t) I;

    memset(cpu->ram, 0x00, 0x0800);
    memset(cpu->sram, 0xff, 0x2000);

    cpu->PC = cpu_get_u16(cpu, RESET_VECTOR);
}

void cpu_free(cpu_t *cpu) {
    free(cpu);
}

uint8_t cpu_tick(cpu_t *cpu) {
    cpu->instr_cycles = 0;

    cpu_interrupt(cpu);

    dump_state(cpu);

    uint8_t opcode = get_opcode(cpu);
    switch (opcode) {
        /* Add With Carry */
        case 0x69: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; ADC(cpu); break;
        case 0x65: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; ADC(cpu); break;
        case 0x75: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 4; ADC(cpu); break;
        case 0x6d: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; ADC(cpu); break;
        case 0x7d: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 4; ADC(cpu); break;
        case 0x79: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 4; ADC(cpu); break;
        case 0x61: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 6; ADC(cpu); break;
        case 0x71: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 5; ADC(cpu); break;

        /* Subtract With Carry */
        case 0xe9: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; SBC(cpu); break;
        case 0xe5: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; SBC(cpu); break;
        case 0xf5: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 4; SBC(cpu); break;
        case 0xed: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; SBC(cpu); break;
        case 0xfd: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 4; SBC(cpu); break;
        case 0xf9: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 4; SBC(cpu); break;
        case 0xe1: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 6; SBC(cpu); break;
        case 0xf1: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 5; SBC(cpu); break;

        /* Bitwise AND memory with accumulator */
        case 0x29: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; AND(cpu); break;
        case 0x25: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; AND(cpu); break;
        case 0x35: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 4; AND(cpu); break;
        case 0x2d: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; AND(cpu); break;
        case 0x3d: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 4; AND(cpu); break;
        case 0x39: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 4; AND(cpu); break;
        case 0x21: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 6; AND(cpu); break;
        case 0x31: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 5; AND(cpu); break;

        /* Bitwise OR memory with accumulator */
        case 0x09: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; ORA(cpu); break;
        case 0x05: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; ORA(cpu); break;
        case 0x15: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 4; ORA(cpu); break;
        case 0x0d: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; ORA(cpu); break;
        case 0x1d: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 4; ORA(cpu); break;
        case 0x19: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 4; ORA(cpu); break;
        case 0x01: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 6; ORA(cpu); break;
        case 0x11: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 5; ORA(cpu); break;

        /* Bitwise XOR memory with accumulator */
        case 0x49: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; EOR(cpu); break;
        case 0x45: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; EOR(cpu); break;
        case 0x55: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 4; EOR(cpu); break;
        case 0x4d: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; EOR(cpu); break;
        case 0x5d: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 4; EOR(cpu); break;
        case 0x59: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 4; EOR(cpu); break;
        case 0x41: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 6; EOR(cpu); break;
        case 0x51: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 5; EOR(cpu); break;

        /* Compate bits in memory with accumulator */
        case 0x24: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; BIT(cpu); break;
        case 0x2c: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; BIT(cpu); break;

        /* Compare with Accumulator */
        case 0xc9: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; CMP(cpu); break;
        case 0xc5: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; CMP(cpu); break;
        case 0xd5: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 4; CMP(cpu); break;
        case 0xcd: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; CMP(cpu); break;
        case 0xdd: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 4; CMP(cpu); break;
        case 0xd9: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 4; CMP(cpu); break;
        case 0xc1: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 6; CMP(cpu); break;
        case 0xd1: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 5; CMP(cpu); break;

        /* Compare with X Register */
        case 0xe0: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; CPX(cpu); break;
        case 0xe4: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; CPX(cpu); break;
        case 0xec: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; CPX(cpu); break;

        /* Compare with Y Register */
        case 0xc0: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; CPY(cpu); break;
        case 0xc4: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; CPY(cpu); break;
        case 0xcc: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; CPY(cpu); break;

        /* Branching */
        case 0xf0: cpu->addr_mode = RELATIVE;       cpu->instr_cycles += 2; BEQ(cpu); break;
        case 0x30: cpu->addr_mode = RELATIVE;       cpu->instr_cycles += 2; BMI(cpu); break;
        case 0xd0: cpu->addr_mode = RELATIVE;       cpu->instr_cycles += 2; BNE(cpu); break;
        case 0x10: cpu->addr_mode = RELATIVE;       cpu->instr_cycles += 2; BPL(cpu); break;
        case 0x90: cpu->addr_mode = RELATIVE;       cpu->instr_cycles += 2; BCC(cpu); break;
        case 0xb0: cpu->addr_mode = RELATIVE;       cpu->instr_cycles += 2; BCS(cpu); break;
        case 0x50: cpu->addr_mode = RELATIVE;       cpu->instr_cycles += 2; BVC(cpu); break;
        case 0x70: cpu->addr_mode = RELATIVE;       cpu->instr_cycles += 2; BVS(cpu); break;

        /* Clear Flags */
        case 0x18: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; CLC(cpu); break;
        case 0xd8: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; CLD(cpu); break;
        case 0x58: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; CLI(cpu); break;
        case 0xb8: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; CLV(cpu); break;

        /* Jump to New Location */
        case 0x4c: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 3; JMP(cpu); break;
        case 0x6c: cpu->addr_mode = INDIRECT;       cpu->instr_cycles += 5; JMP(cpu); break;

        /* Jump to Subroutine */
        case 0x20: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; JSR(cpu); break;
        case 0x60: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; RTS(cpu); break;
        case 0x40: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; RTI(cpu); break;

        /* Load Accumulator */
        case 0xa9: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; LDA(cpu); break;
        case 0xa5: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; LDA(cpu); break;
        case 0xb5: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 4; LDA(cpu); break;
        case 0xad: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; LDA(cpu); break;
        case 0xbd: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 4; LDA(cpu); break;
        case 0xb9: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 4; LDA(cpu); break;
        case 0xa1: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 6; LDA(cpu); break;
        case 0xb1: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 5; LDA(cpu); break;

        /* Load X Register */
        case 0xa2: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; LDX(cpu); break;
        case 0xa6: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; LDX(cpu); break;
        case 0xb6: cpu->addr_mode = ZERO_PAGE_Y;    cpu->instr_cycles += 4; LDX(cpu); break;
        case 0xae: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; LDX(cpu); break;
        case 0xbe: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 4; LDX(cpu); break;

        /* Load Y Register */
        case 0xa0: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; LDY(cpu); break;
        case 0xa4: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; LDY(cpu); break;
        case 0xb4: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 4; LDY(cpu); break;
        case 0xac: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; LDY(cpu); break;
        case 0xbc: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 4; LDY(cpu); break;

        /* Push Processor Status */
        case 0x48: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 3; PHA(cpu); break;

        /* Push Processor Status */
        case 0x08: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 3; PHP(cpu); break;

        /* Pull Accumulator */
        case 0x68: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 4; PLA(cpu); break;

        /* Pull Processor Status */
        case 0x28: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 4; PLP(cpu); break;

        /* Set Flags */
        case 0x38: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; SEC(cpu); break;
        case 0xf8: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; SED(cpu); break;
        case 0x78: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; SEI(cpu); break;

        /* Store Accumulator */
        case 0x85: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; STA(cpu); break;
        case 0x95: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 4; STA(cpu); break;
        case 0x8d: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; STA(cpu); break;
        case 0x9d: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 5; STA(cpu); break;
        case 0x99: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 5; STA(cpu); break;
        case 0x81: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 6; STA(cpu); break;
        case 0x91: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 6; STA(cpu); break;

        /* Store X Register */
        case 0x86: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; STX(cpu); break;
        case 0x96: cpu->addr_mode = ZERO_PAGE_Y;    cpu->instr_cycles += 4; STX(cpu); break;
        case 0x8e: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; STX(cpu); break;

        /* Store Y Register */
        case 0x84: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; STY(cpu); break;
        case 0x94: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 4; STY(cpu); break;
        case 0x8c: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; STY(cpu); break;

        /* Transfer */
        case 0xaa: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; TAX(cpu); break;
        case 0xa8: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; TAY(cpu); break;
        case 0xba: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; TSX(cpu); break;
        case 0x8a: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; TXA(cpu); break;
        case 0x9a: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; TXS(cpu); break;
        case 0x98: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; TYA(cpu); break;

        /* Increment Memory */
        case 0xe6: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; INC(cpu); break;
        case 0xf6: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; INC(cpu); break;
        case 0xee: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; INC(cpu); break;
        case 0xfe: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; INC(cpu); break;

        /* Increment Registers */
        case 0xe8: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; INX(cpu); break;
        case 0xc8: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; INY(cpu); break;

        /* Decrement Memory */
        case 0xc6: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; DEC(cpu); break;
        case 0xd6: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; DEC(cpu); break;
        case 0xce: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; DEC(cpu); break;
        case 0xde: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; DEC(cpu); break;

        /* Decrement Registers */
        case 0xca: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; DEX(cpu); break;
        case 0x88: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; DEY(cpu); break;

        /* Shift one bit left */
        case 0x0a: cpu->addr_mode = ACCUMULATOR;    cpu->instr_cycles += 2; ASL(cpu); break;
        case 0x06: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; ASL(cpu); break;
        case 0x16: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; ASL(cpu); break;
        case 0x0e: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; ASL(cpu); break;
        case 0x1e: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; ASL(cpu); break;

        /* Shift one bit right */
        case 0x4a: cpu->addr_mode = ACCUMULATOR;    cpu->instr_cycles += 2; LSR(cpu); break;
        case 0x46: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; LSR(cpu); break;
        case 0x56: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; LSR(cpu); break;
        case 0x4e: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; LSR(cpu); break;
        case 0x5e: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; LSR(cpu); break;

        /* Rotate one bit left */
        case 0x2a: cpu->addr_mode = ACCUMULATOR;    cpu->instr_cycles += 2; ROL(cpu); break;
        case 0x26: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; ROL(cpu); break;
        case 0x36: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; ROL(cpu); break;
        case 0x2e: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; ROL(cpu); break;
        case 0x3e: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; ROL(cpu); break;

        /* Rotate one bit right */
        case 0x6a: cpu->addr_mode = ACCUMULATOR;    cpu->instr_cycles += 2; ROR(cpu); break;
        case 0x66: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; ROR(cpu); break;
        case 0x76: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; ROR(cpu); break;
        case 0x6e: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; ROR(cpu); break;
        case 0x7e: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; ROR(cpu); break;

        /* Break */
        case 0x00: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 7; BRK(cpu); break;

        /* NOOP */
        case 0xea: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; NOP(cpu); break;

        /* UNDOCUMENTED COMMANDS */

        /* Load to Accumulator and transfer to X */
        case 0xa7: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; LAX(cpu); break;
        case 0xb7: cpu->addr_mode = ZERO_PAGE_Y;    cpu->instr_cycles += 4; LAX(cpu); break;
        case 0xaf: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; LAX(cpu); break;
        case 0xbf: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 4; LAX(cpu); break;
        case 0xa3: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 6; LAX(cpu); break;
        case 0xb3: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 5; LAX(cpu); break;

        /* Store result of A & X */
        case 0x87: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; SAX(cpu); break;
        case 0x97: cpu->addr_mode = ZERO_PAGE_Y;    cpu->instr_cycles += 4; SAX(cpu); break;
        case 0x8f: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; SAX(cpu); break;
        case 0x83: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 6; SAX(cpu); break;

        /* Subtract With Carry */
        case 0xeb: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; SBC(cpu); break;

        /* Decrement memory then compare with Acc */
        case 0xc7: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; DCP(cpu); break;
        case 0xd7: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; DCP(cpu); break;
        case 0xcf: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; DCP(cpu); break;
        case 0xdf: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; DCP(cpu); break;
        case 0xdb: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 7; DCP(cpu); break;
        case 0xc3: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 8; DCP(cpu); break;
        case 0xd3: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 8; DCP(cpu); break;

        /* Increment memory then subtract with carry */
        case 0xe7: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; ISB(cpu); break;
        case 0xf7: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; ISB(cpu); break;
        case 0xef: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; ISB(cpu); break;
        case 0xff: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; ISB(cpu); break;
        case 0xfb: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 7; ISB(cpu); break;
        case 0xe3: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 8; ISB(cpu); break;
        case 0xf3: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 8; ISB(cpu); break;

        /* Rorate one bit left then bitwise AND with Acc */
        case 0x27: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; RLA(cpu); break;
        case 0x37: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; RLA(cpu); break;
        case 0x2f: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; RLA(cpu); break;
        case 0x3f: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; RLA(cpu); break;
        case 0x3b: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 7; RLA(cpu); break;
        case 0x23: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 8; RLA(cpu); break;
        case 0x33: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 8; RLA(cpu); break;

        /* Rorate one bit right then add with carry */
        case 0x67: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; RRA(cpu); break;
        case 0x77: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; RRA(cpu); break;
        case 0x6f: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; RRA(cpu); break;
        case 0x7f: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; RRA(cpu); break;
        case 0x7b: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 7; RRA(cpu); break;
        case 0x63: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 8; RRA(cpu); break;
        case 0x73: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 8; RRA(cpu); break;

        /* Shift one bit left then bitwise OR with Acc */
        case 0x07: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; SLO(cpu); break;
        case 0x17: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; SLO(cpu); break;
        case 0x0f: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; SLO(cpu); break;
        case 0x1f: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; SLO(cpu); break;
        case 0x1b: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 7; SLO(cpu); break;
        case 0x03: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 8; SLO(cpu); break;
        case 0x13: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 8; SLO(cpu); break;

        /* Shift one bit right then bitwise XOR with Acc */
        case 0x47: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 5; SRE(cpu); break;
        case 0x57: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 6; SRE(cpu); break;
        case 0x4f: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 6; SRE(cpu); break;
        case 0x5f: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 7; SRE(cpu); break;
        case 0x5b: cpu->addr_mode = ABSOLUTE_Y;     cpu->instr_cycles += 7; SRE(cpu); break;
        case 0x43: cpu->addr_mode = INDIRECT_X;     cpu->instr_cycles += 8; SRE(cpu); break;
        case 0x53: cpu->addr_mode = INDIRECT_Y;     cpu->instr_cycles += 8; SRE(cpu); break;

        /* NOOP */
        case 0x1a:
        case 0x3a:
        case 0x5a:
        case 0x7a:
        case 0xda:
        case 0xfa: cpu->addr_mode = IMPLIED;        cpu->instr_cycles += 2; NOP(cpu); break;
        case 0x80:
        case 0x89: cpu->addr_mode = IMMEDIATE;      cpu->instr_cycles += 2; NOP(cpu); break;
        case 0x04:
        case 0x44:
        case 0x64: cpu->addr_mode = ZERO_PAGE;      cpu->instr_cycles += 3; NOP(cpu); break;
        case 0x14:
        case 0x34:
        case 0x54:
        case 0x74:
        case 0xd4:
        case 0xf4: cpu->addr_mode = ZERO_PAGE_X;    cpu->instr_cycles += 4; NOP(cpu); break;
        case 0x0c: cpu->addr_mode = ABSOLUTE;       cpu->instr_cycles += 4; NOP(cpu); break;
        case 0x1c:
        case 0x3c:
        case 0x5c:
        case 0x7c:
        case 0xdc:
        case 0xfc: cpu->addr_mode = ABSOLUTE_X;     cpu->instr_cycles += 4; NOP(cpu); break;

        default:
            _panic("Invalid OpCode: 0x%.2x (%s)\n", opcode, OPCODES[opcode]);
    }

    cpu->clock += cpu->instr_cycles;

    return cpu->instr_cycles;
}

void cpu_interrupt(cpu_t *cpu) {
    if (cpu->ppu->is_nmi) {
        cpu->ppu->is_nmi = FALSE;
        cpu->instr_cycles += 7;

        cpu_push_u16(cpu, cpu->PC);
        cpu_set_flag(cpu, B);
        cpu_push_u8(cpu, cpu->P);

        cpu->PC = cpu_get_u16(cpu, NMI_VECTOR);
    }
}

uint8_t get_opcode(cpu_t *cpu) {
    uint8_t op = cpu_get_u8(cpu, cpu->PC);
    cpu->PC++;
    return op;
}

/* Read / Write RAM */
uint16_t cpu_get_addr(cpu_t *cpu) {
    uint16_t addr = 0;
    uint8_t lo, hi;

    page_crossed = FALSE;

    switch (cpu->addr_mode) {
        case IMPLIED:
            break;
        case IMMEDIATE:
        case RELATIVE:
            addr = cpu->PC;
            cpu->PC++;
            break;
        case ZERO_PAGE:
            addr = cpu_get_u8(cpu, cpu->PC);
            cpu->PC++;
            break;
        case ZERO_PAGE_X:
            addr = (uint8_t) (cpu_get_u8(cpu, cpu->PC) + cpu->X) & 0xffu;
            cpu->PC++;
            break;
        case ZERO_PAGE_Y:
            addr = (uint8_t) (cpu_get_u8(cpu, cpu->PC) + cpu->Y) & 0xffu;
            cpu->PC++;
            break;
        case ABSOLUTE:
            addr = cpu_get_u16(cpu, cpu->PC);
            cpu->PC += 2;
            break;
        case ABSOLUTE_X:
            addr = cpu_get_u16(cpu, cpu->PC) + cpu->X;
            if (!addr_are_same_page(addr - cpu->X, addr)) {
                page_crossed = TRUE;
            }

            cpu->PC += 2;
            break;
        case ABSOLUTE_Y:
            addr = cpu_get_u16(cpu, cpu->PC) + cpu->Y;
            if (!addr_are_same_page(addr - cpu->Y, addr)) {
                page_crossed = TRUE;
            }

            cpu->PC += 2;
            break;
        case INDIRECT:
            addr = cpu_get_u16(cpu, cpu->PC);

            lo = cpu_get_u8(cpu, addr);
            hi = cpu_get_u8(cpu, (addr & 0xff00u) + ((addr + 1u) & 0xffu));

            addr = u8_to_u16(lo, hi);

            cpu->PC += 2;
            break;
        case INDIRECT_X:
            addr = cpu_get_u8(cpu, cpu->PC) + cpu->X;

            lo = cpu_get_u8(cpu, addr & 0xffu);
            hi = cpu_get_u8(cpu, (addr + 1u) & 0xffu);

            addr = u8_to_u16(lo, hi);

            cpu->PC++;
            break;
        case INDIRECT_Y:
            addr = cpu_get_u8(cpu, cpu->PC);

            lo = cpu_get_u8(cpu, addr & 0xffu);
            hi = cpu_get_u8(cpu, (addr + 1u) & 0xffu);

            addr = u8_to_u16(lo, hi);

            addr += cpu->Y;

            if (!addr_are_same_page(addr - cpu->Y, addr)) {
                page_crossed = TRUE;
            }

            cpu->PC++;
            break;
        default:
            _panic("cpu_load_value: Invalid AddrMode: %d\n", cpu->addr_mode);
    }

    return addr;
}

uint8_t cpu_get_u8(cpu_t *cpu, uint16_t addr) {
    if (addr < 0x2000) {
        /* RAM value */
        /* Addresses higher than 0x0800 are mirror of the first 0x0800 */
        return cpu->ram[addr % 0x0800];
    } else if (addr >= 0x2000 && addr <= 0x4000) {
        if (addr == 0x2002) {
            return ppu_get_status(cpu->ppu);
        } else {
            _panic("_get_u8 not implemented for addr: %04x\n", addr);
            return 0;
        }
    } else if (addr >= 0x4000 && addr < 0x4020) {
        /* APU and I/O registers */
        if (addr == 0x4016) {
            /* joy 1 */
            /*
            byte ret = (byte)(_joystick[_joy1bit] ? 1 : 0);
            _joy1bit++;
            if (_joy1bit > 7)
                _joy1bit = 0;
            return ret;
            */

            _panic("_get_u8 not implemented for addr: %04x\n", addr);
            return 0;
        } else {
            _panic("_get_u8 not implemented for addr: %04x\n", addr);
            return 0;
        }
    } else if (addr >= 0x4020 && addr < 0x6000) {
        /* Expansion ROM (MMC5) */
        _panic("_get_u8 not implemented for addr: %04x\n", addr);
        return 0;
        /*return _mapper.GetExRamByte((ushort)(addr - 0x4020));*/
    } else if (addr >= 0x6000 && addr < 0x8000) {
        /* SRAM Values */
        return cpu->sram[addr - 0x6000];
    } else {
        return get_prg_u8(addr - 0x8000);
    }
}

uint16_t cpu_get_u16(cpu_t *cpu, uint16_t addr) {
    if (addr < 0x2000) {
        /* RAM value */
        /* Addresses higher than 0x0800 are mirror of the first 0x0800 */
        return (uint16_t) ((cpu->ram[(addr % 0x800) + 1] << 8u) + cpu->ram[(addr % 0x800)]);
    } else if (addr >= 0x4020 && addr < 0x6000) {
        /* Expansion ROM (MMC5) */
        return 0;
        /*return _mapper.GetExRamShort((ushort)(addr - 0x4020));*/
    } else if (addr >= 0x6000 && addr < 0x8000) {
        /* SRAM value */
        return (uint16_t) ((cpu->sram[(addr - 0x6000) + 1] << 8u) + cpu->sram[(addr - 0x6000)]);
    } else if (addr >= 0x8000) {
        return get_prg_u16(addr - 0x8000);
    } else {
        return 0;
    }
}

void cpu_set_u8(cpu_t *cpu, uint16_t addr, uint8_t val) {
    if (addr < 0x2000) {
        /* RAM value */
        /* Addresses higher than 0x0800 are mirror of the first 0x0800 */
        cpu->ram[addr % 0x800] = val;
    } else if (addr >= 0x2000 && addr < 0x4000) {
        /* PPU Status registers */
        return;
        /*_ppu->set_u8(addr, val);*/
    } else if (addr >= 0x4000 && addr < 0x4020) {
        /* APU and I/O registers */
        if (addr == 0x4014) {
            /* sprite dma */
            /*ushort addr = (ushort)(val * 0x100);*/
            /*for (int oamAddr = 0; oamAddr < 256; oamAddr++)*/
            /*{*/
            /*_ppu._oam[oamAddr] = cpu->ram[addr + oamAddr];*/
            /*}*/
        } else if (addr == 0x4016) {
            /*if (val == 1) {*/
            /*_joy1bit = 0;*/
            /*}*/
        }
    } else if (addr >= 0x4020 && addr < 0x6000) {
        /* Expansion ROM (MMC5) */
        /*_mapper.SetExRamByte((ushort)(addr - 0x4020), val);*/
    } else if (addr >= 0x8000) {
        /*_mapper.SetPrgByte((ushort)(addr - 0x8000), val);*/
    }
}

/* Stack */
void cpu_push_u8(cpu_t *cpu, uint8_t val) {
    cpu->ram[0x100 + cpu->SP] = val;
    cpu->SP--;
}

void cpu_push_u16(cpu_t *cpu, uint16_t val) {
    _debug_log("STACK", "Pushing 0x%04x to stack\n", val);
    cpu_push_u8(cpu, (uint8_t) (val >> 8u));
    cpu_push_u8(cpu, (uint8_t) (val & 0xffu));
}

uint8_t cpu_pop_u8(cpu_t *cpu) {
    cpu->SP++;
    return cpu->ram[0x100 + cpu->SP];
}

uint16_t cpu_pop_u16(cpu_t *cpu) {
    uint16_t val = cpu_pop_u8(cpu) + (cpu_pop_u8(cpu) << 8u);
    _debug_log("STACK", "Popped 0x%04x from stack\n", val);
    return val;
}

/* Flags */
void cpu_set_carry(cpu_t *cpu, uint8_t val) {
    if (val) {
        cpu_set_flag(cpu, C);
    } else {
        cpu_unset_flag(cpu, C);
    }
}

void cpu_set_overflow(cpu_t *cpu, uint8_t val) {
    if (val) {
        cpu_set_flag(cpu, V);
    } else {
        cpu_unset_flag(cpu, V);
    }
}

void cpu_set_negative(cpu_t *cpu, uint8_t val) {
    if (val & (uint8_t) 0x80) {
        cpu_set_flag(cpu, N);
    } else {
        cpu_unset_flag(cpu, N);
    }
}

void cpu_set_zero(cpu_t *cpu, uint8_t val) {
    if (val == 0) {
        cpu_set_flag(cpu, Z);
    } else {
        cpu_unset_flag(cpu, Z);
    }
}

void cpu_set_flag(cpu_t *cpu, Flag flag) {
    cpu->P |= flag;
}

void cpu_unset_flag(cpu_t *cpu, Flag flag) {
    cpu->P &= ~flag;
}

bool cpu_flag_is_set(cpu_t *cpu, Flag flag) {
    return (cpu->P & flag) > 0 ? 1 : 0;
}

/* Paging */
void cpu_increment_cycles_if_page_crossed(cpu_t *cpu) {
    if (page_crossed) {
        cpu->instr_cycles++;
    }
}

bool addr_are_same_page(uint16_t val_a, uint16_t addr_b) {
    return get_addr_page(val_a) == get_addr_page(addr_b);
}

inline uint8_t get_addr_page(uint16_t addr) {
    return addr >> 8u;
}

/* Debug */
#ifdef DEBUG
#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
void dump_state(cpu_t *cpu) {
    uint8_t p = cpu->P & ~U;
    uint8_t op = cpu_get_u8(cpu, cpu->PC);
    char flags[9];

    flags[0] = p & N ? 'N' : '-';
    flags[1] = p & V ? 'V' : '-';
    flags[2] = p & U ? 'U' : '-';
    flags[3] = p & B ? 'B' : '-';
    flags[4] = p & D ? 'D' : '-';
    flags[5] = p & I ? 'I' : '-';
    flags[6] = p & Z ? 'Z' : '-';
    flags[7] = p & C ? 'C' : '-';
    flags[8] = '\0';

    _debug_log("CPU", "[OP: %s (0x%02x), PC: 0x%04x, SP: 0x%02x, A: 0x%02x, X: 0x%02x, Y: 0x%02x, P: 0x%02x (%s), CL: %10d]\n",
            OPCODES[op], op, cpu->PC, cpu->SP, cpu->A, cpu->X, cpu->Y, p, flags, cpu->clock);
}
#pragma clang diagnostic pop
#else
inline void dump_state(cpu_t *cpu) { }
#endif
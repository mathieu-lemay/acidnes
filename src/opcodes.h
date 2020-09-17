#ifndef __OPCODES_H__
#define __OPCODES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cpu.h"

/* OP Codes */
void ADC(cpu_t *cpu); /* Add With Carry */
void SBC(cpu_t *cpu); /* Subtract With Carry */
void AND(cpu_t *cpu); /* Bitwise AND memory with accumulator */
void ORA(cpu_t *cpu); /* Bitwise OR memory with accumulator */
void EOR(cpu_t *cpu); /* Bitwise XOR memory with accumulator */
void BIT(cpu_t *cpu); /* Compate bits in memory with accumulator */
void CMP(cpu_t *cpu); /* Compare with Accumulator */
void CPX(cpu_t *cpu); /* Compare with X Register */
void CPY(cpu_t *cpu); /* Compare with Y Register */
void BEQ(cpu_t *cpu); /* Branch if Equal */
void BMI(cpu_t *cpu); /* Branch if Negative (Minus) */
void BNE(cpu_t *cpu); /* Branch if Not Equal */
void BPL(cpu_t *cpu); /* Branch if Positive (Plus) */
void BCC(cpu_t *cpu); /* Branch on carry clear */
void BCS(cpu_t *cpu); /* Branch on carry set */
void BVC(cpu_t *cpu); /* Branch on carry clear */
void BVS(cpu_t *cpu); /* Branch on carry set */
void CLC(cpu_t *cpu); /* Clear Carry Flag */
void CLD(cpu_t *cpu); /* Clear Decimal Flag */
void CLI(cpu_t *cpu); /* Clear Interrupt Disable */
void CLV(cpu_t *cpu); /* Clear Overflow Flag */
void JMP(cpu_t *cpu); /* Jump to New Location */
void JSR(cpu_t *cpu); /* Jump to Subroutine */
void RTS(cpu_t *cpu); /* Return from Subroutine */
void RTI(cpu_t *cpu); /* Return from Interrupt */
void LDA(cpu_t *cpu); /* Load to Accumulator */
void LDX(cpu_t *cpu); /* Load to X Register */
void LDY(cpu_t *cpu); /* Load to Y Register */
void PHA(cpu_t *cpu); /* Push Accumulator Status */
void PHP(cpu_t *cpu); /* Push Processor Status */
void PLA(cpu_t *cpu); /* Pull Accumulator */
void PLP(cpu_t *cpu); /* Pull Processor Status */
void SEC(cpu_t *cpu); /* Set Carry Flag */
void SED(cpu_t *cpu); /* Set Decimal Flag */
void SEI(cpu_t *cpu); /* Set Interrupt Disable */
void STA(cpu_t *cpu); /* Store Accumulator */
void STX(cpu_t *cpu); /* Store X Register */
void STY(cpu_t *cpu); /* Store Y Register */
void TAX(cpu_t *cpu); /* Transfer Accumulator to X */
void TAY(cpu_t *cpu); /* Transfer Accumulator to Y */
void TSX(cpu_t *cpu); /* Transfer Stack Pointer to X */
void TXA(cpu_t *cpu); /* Transfer X to Accumulator */
void TXS(cpu_t *cpu); /* Transfer X to Stact Pointer */
void TYA(cpu_t *cpu); /* Transfer X to Accumulator */
void INC(cpu_t *cpu); /* Increment memory by one */
void INX(cpu_t *cpu); /* Increment X by one */
void INY(cpu_t *cpu); /* Increment Y by one */
void DEC(cpu_t *cpu); /* Decrement memory by one */
void DEX(cpu_t *cpu); /* Decrement X by one */
void DEY(cpu_t *cpu); /* Decrement Y by one */
void ASL(cpu_t *cpu); /* Shift one bit left */
void LSR(cpu_t *cpu); /* Shift one bit right */
void ROL(cpu_t *cpu); /* Rotate one bit left */
void ROR(cpu_t *cpu); /* Rotate one bit right */
void NOP(cpu_t *cpu); /* No Operation */
void BRK(cpu_t *cpu); /* Force a break */

/* Undocumented OP Codes */
void LAX(cpu_t *cpu); /* Load to Accumulator and transfer to X */
void SAX(cpu_t *cpu); /* Store result of A & X */
void DCP(cpu_t *cpu); /* Decrement memory then compare with Acc */
void ISB(cpu_t *cpu); /* Increment memory then subtract with carry */
void RLA(cpu_t *cpu); /* Rorate one bit left then bitwise AND with Acc */
void RRA(cpu_t *cpu); /* Rorate one bit right then add with carry */
void SLO(cpu_t *cpu); /* Shift one bit left then bitwise OR with Acc */
void SRE(cpu_t *cpu); /* Shift one bit right then bitwise XOR with Acc */

#ifdef __cplusplus
}
#endif
#endif //__OPCODES_H__

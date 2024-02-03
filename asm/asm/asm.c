#include <stdio.h>
#include <string.h>

int get_opcode_num(const char* opcode) {
    if (strcmp(opcode, "add") == 0)     return 0x00 ;
    if (strcmp(opcode, "sub") == 0)     return 0x01 ;
    if (strcmp(opcode, "mac") == 0)     return 0x02 ;
    if (strcmp(opcode, "and") == 0)     return 0x03 ;
    if (strcmp(opcode, "or") == 0)      return 0x04 ;
    if (strcmp(opcode, "xor") == 0)     return 0x05 ;
    if (strcmp(opcode, "sll") == 0)     return 0x06 ;
    if (strcmp(opcode, "sra") == 0)     return 0x07 ;
    if (strcmp(opcode, "srl") == 0)     return 0x08 ;
    if (strcmp(opcode, "beq") == 0)     return 0x09 ;
    if (strcmp(opcode, "bne") == 0)     return 0x0A ;
    if (strcmp(opcode, "blt") == 0)     return 0x0B ;
    if (strcmp(opcode, "bgt") == 0)     return 0x0C ;
    if (strcmp(opcode, "ble") == 0)     return 0x0D ;
    if (strcmp(opcode, "bge") == 0)     return 0x0E ;
    if (strcmp(opcode, "jal") == 0)     return 0x0F ;
    if (strcmp(opcode, "lw") == 0)      return 0x10 ;
    if (strcmp(opcode, "sw") == 0)      return 0x11 ;
    if (strcmp(opcode, "reti") == 0)    return 0x12 ;
    if (strcmp(opcode, "in") == 0)      return 0x13 ;
    if (strcmp(opcode, "out") == 0)     return 0x14 ;
    if (strcmp(opcode, "halt") == 0)    return 0x15 ;

    return -1;
}

int get_reg_num(const char* reg) {
	if (strcmp(reg, "zero") == 0)   return 0x0 ;
	if (strcmp(reg, "imm1") == 0)   return 0x1 ;
	if (strcmp(reg, "imm2") == 0)   return 0x2 ;
	if (strcmp(reg, "$v0") == 0)    return 0x3 ;
	if (strcmp(reg, "$a0") == 0)    return 0x4 ;
	if (strcmp(reg, "$a1") == 0)    return 0x5 ;
	if (strcmp(reg, "$a2") == 0)    return 0x6 ;
	if (strcmp(reg, "$t0") == 0)    return 0x7 ;
	if (strcmp(reg, "$t1") == 0)    return 0x8 ;
	if (strcmp(reg, "$t2") == 0)    return 0x9 ;
	if (strcmp(reg, "$s0") == 0)    return 0xA ;
	if (strcmp(reg, "$s1") == 0)    return 0xB ;
	if (strcmp(reg, "$s2") == 0)    return 0xC ;
	if (strcmp(reg, "$gp") == 0)    return 0xD ;
	if (strcmp(reg, "$sp") == 0)    return 0xE ;
	if (strcmp(reg, "$ra") == 0)    return 0xF ;

	return -1;
}

unsigned long long merge_opcode_and_regs(int opcode, int rd, int rs, int rt, int rm) {
	unsigned long long result = 0;
	result |= (unsigned long long)opcode << 48;
	result |= (unsigned long long)rd << 36;
	result |= (unsigned long long)rs << 32;
	result |= (unsigned long long)rt << 28;
	result |= (unsigned long long)rm << 24;
	
	return result;
}

int main() {
	unsigned long long hex1=21, hex2=0xAf, hex3=0x521A, hex4=0x0F;
	
	printf("hex1: % 012llX\n hex2 : % 012llX\n, hex3: % 012llX\n, hex4: % 012llX\n", hex1, hex2, hex3, hex4);  //writes 12 hex symbols of a number

	return 0;
}

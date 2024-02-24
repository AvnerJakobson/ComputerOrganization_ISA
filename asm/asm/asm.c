#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 500
#define MAX_LABEL_NAME 50
#define NUM_OF_REGS 16
#define MAX_INSTRUCTIONS_MEM 4096

////////////STRUCTS////////////

typedef struct label
{
    char label_name[MAX_LABEL_NAME];
    int label_addr;
} label;

typedef struct instruction
{
    int opcode;
    int rd, rs, rt, rm;
    int imm_a, imm_b;
    long long result ;
} instruction;
////////////FUNCTIONS////////////


//Checks if the line is a label, return 1 if true, 0 if false
int line_is_label(char* line) {
    char* cmnt_idx = strchr(line, '#');
    char* lbl_idx = strchr(line, ':');
    if (lbl_idx) {
        if (cmnt_idx) {
            if (lbl_idx > cmnt_idx) { //checks if the label is before the comment
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int line_is_empty(char* line) {
    while (*line != '\0') {
        if (!isspace((unsigned char)*line) && *line != '#') {
            return 0; // Line is not empty
        }
        line++;
    }
    return 1; // Line is empty
}

int line_is_word(char* line) {
    return (strstr(line, ".word") == NULL) ? 0 : 1;
}


//Checks if the line is a command, return 1 if true, 0 if false.
int line_is_command(char* line) {
    //if ((line_is_label(line))) { //if a line is a label or a word, it is not a command

    if ((line_is_word(line)) || (line_is_label(line))) { //if a line is a label or a word, it is not a command
        return 0;
    }
    int curr_char_idx = *(line);
    while ((curr_char_idx != '#') && (curr_char_idx != '\n')) { //going through the line until the end or a comment
        if (!((curr_char_idx == ' ') || (curr_char_idx == '\t'))) { //if the character is not a space or a tab
            return 1;
        }
        curr_char_idx++;
    }
    return 0;
}

//hardcoded map of opcode to its hex value
int get_opcode_num(const char* opcode) {
    if (strcmp(opcode, "add") == 0)     return 0x00;
    if (strcmp(opcode, "sub") == 0)     return 0x01;
    if (strcmp(opcode, "mac") == 0)     return 0x02;
    if (strcmp(opcode, "and") == 0)     return 0x03;
    if (strcmp(opcode, "or") == 0)      return 0x04;
    if (strcmp(opcode, "xor") == 0)     return 0x05;
    if (strcmp(opcode, "sll") == 0)     return 0x06;
    if (strcmp(opcode, "sra") == 0)     return 0x07;
    if (strcmp(opcode, "srl") == 0)     return 0x08;
    if (strcmp(opcode, "beq") == 0)     return 0x09;
    if (strcmp(opcode, "bne") == 0)     return 0x0A;
    if (strcmp(opcode, "blt") == 0)     return 0x0B;
    if (strcmp(opcode, "bgt") == 0)     return 0x0C;
    if (strcmp(opcode, "ble") == 0)     return 0x0D;
    if (strcmp(opcode, "bge") == 0)     return 0x0E;
    if (strcmp(opcode, "jal") == 0)     return 0x0F;
    if (strcmp(opcode, "lw") == 0)      return 0x10;
    if (strcmp(opcode, "sw") == 0)      return 0x11;
    if (strcmp(opcode, "reti") == 0)    return 0x12;
    if (strcmp(opcode, "in") == 0)      return 0x13;
    if (strcmp(opcode, "out") == 0)     return 0x14;
    if (strcmp(opcode, "halt") == 0)    return 0x15;

    return -1;
}

//hardcoded map of register to its hex value
int get_reg_num(const char* reg) {
    if (strcmp(reg, "$zero") == 0)   return 0x0;
    if (strcmp(reg, "$imm1") == 0)   return 0x1;
    if (strcmp(reg, "$imm2") == 0)   return 0x2;
    if (strcmp(reg, "$v0") == 0)    return 0x3;
    if (strcmp(reg, "$a0") == 0)    return 0x4;
    if (strcmp(reg, "$a1") == 0)    return 0x5;
    if (strcmp(reg, "$a2") == 0)    return 0x6;
    if (strcmp(reg, "$t0") == 0)    return 0x7;
    if (strcmp(reg, "$t1") == 0)    return 0x8;
    if (strcmp(reg, "$t2") == 0)    return 0x9;
    if (strcmp(reg, "$s0") == 0)    return 0xA;
    if (strcmp(reg, "$s1") == 0)    return 0xB;
    if (strcmp(reg, "$s2") == 0)    return 0xC;
    if (strcmp(reg, "$gp") == 0)    return 0xD;
    if (strcmp(reg, "$sp") == 0)    return 0xE;
    if (strcmp(reg, "$ra") == 0)    return 0xF;

    return -1;
}

char* clean_line(char* line, int pass_num) {
    char* head = NULL;
    char last_string[MAX_LINE_LENGTH];
    char* last = &last_string;
    char* clean_line = line;
    int i = 0;
    if (line_is_word(line)) {
        return line;
    }
    if (pass_num == 1){
		while ((*line != '\n') && (*line != '#')) {
			if (!((*line == ' ') || (*line == '\t')))
				clean_line[i++] = *line;
			line++;
		}
		clean_line[i] = 0;
		if (*clean_line == 0) return NULL;
		else return clean_line;
	}
    if (pass_num == 2) {
        while ((*line != '\n') && (*line != '#')) {
            if (!((*line == ' ') || (*line == '\t')))
                clean_line[i++] = *line;
            line++;
        }
        clean_line[i] = 0;
        if (*clean_line == 0) return NULL;
        else {
            strcpy(last, strchr(clean_line, '$'));
            head = clean_line;
            while (*(clean_line) != '$') clean_line++;
            *clean_line = ',';
            *(clean_line + 1) = 0;
            strcat(head, last);
            head[i + 1] = 0;
            return head;
        }
    }
}

int handle_imm(char* imm, label* labels, int num_of_labels) {
    printf("imm: %s\n", imm);
    for (int i = 0; i < num_of_labels; i++) {
        if (strcmp(imm, labels[i].label_name) == 0) {
            printf("returning: %012llX\n", (long long)labels[i].label_addr);
            return labels[i].label_addr;
        }
    }
    
    for (int i = 0; i < strlen(imm); i++) {
        imm[i] = tolower(imm[i]);
    }
    if (imm[0] == '0' && imm[1] == 'x') {
        printf("returning: %012llX\n", (signed long long)strtol(imm, NULL, 16));
        return (int)strtol(imm, NULL, 16);
    }
    else {
        printf("returning: %012llX\n", (signed long long)atoi(imm));
        return (signed long long)atoi(imm);
    }
}

//merges the opcode and registers into a single 48 bit number
unsigned long long calculate_instrcuion(int opcode, int rd, int rs, int rt, int rm, int imm_a, int imm_b) {
    unsigned long long result = 0;
    result |= (unsigned long long)opcode << 40; //opcode= 0xAB  -> res = 0xAB0000000000
    result |= (unsigned long long)rd << 36;     //rd    = 0xC   -> res = 0xABC000000000
    result |= (unsigned long long)rs << 32;     //rs    = 0xD   -> res = 0xABCD00000000
    result |= (unsigned long long)rt << 28;     //rt    = 0xE   -> res = 0xABCDE0000000
    result |= (unsigned long long)rm << 24;     //rm    = 0xF   -> res = 0xABCDEF000000
	
    if (imm_a < 0) {
        imm_a = imm_a & 0xFFF;
    }
    if (imm_b < 0){
        imm_b = imm_b & 0xFFF;
	}
    result |=   imm_a << 12;  //imm_a = 0x123 -> res = 0xABCDEF123000
    result |=   imm_b;        //imm_b = 0x456 -> res = 0xABCDEF123456
    return result;
}

int first_pass(FILE* fptr, label* labels) {
    char line[MAX_LINE_LENGTH + 1];
    char label[MAX_LABEL_NAME + 1];  //+1 for the null terminator
    char* label_p = &label;
    int counter = 0, addr = 0;

    while (!feof(fptr)) {
        if (!line) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        fgets(line, sizeof(line), fptr);
        if (line_is_empty(line)) {
            continue;
        }
        if (line_is_label(line)) {
            char* cleaned = clean_line(line,1);
            int idx=0;            
            labels->label_addr = addr;
            strcpy(label_p, strtok(cleaned, ":"));
            strcpy(labels->label_name, label_p);
            labels++;
            counter++;
        }
        else {
            addr++;
        }
    }
    return counter;
}

int second_pass(FILE* fptr, instruction* instructions, label* labels, int num_of_labels) {
    char line[MAX_LINE_LENGTH + 1];
    int counter = 0;

    while (!feof(fptr)) {
        if (!line) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        fgets(line, sizeof(line), fptr);
        if (line_is_empty(line)) {
            continue;
        }
        if (line_is_word(line)) {
            continue;
        }
        if (line_is_label(line)) {
            continue;
        }
        
        char* cleaned = clean_line(line,2);
        printf("%s\n", cleaned);
        
        char* line_opcode = strtok(cleaned, ",");
        instructions->opcode = get_opcode_num(line_opcode);
        printf("Opcode: %012llX\n", (long long)instructions->opcode);
        char* line_rd = strtok(NULL, ",");
        instructions->rd = get_reg_num(line_rd);
        char* line_rs = strtok(NULL, ",");
        instructions->rs = get_reg_num(line_rs);
        char* line_rt = strtok(NULL, ",");
        instructions->rt = get_reg_num(line_rt);
        char* line_rm = strtok(NULL, ",");
        instructions->rm = get_reg_num(line_rm);
        
        char* line_imm_a = strtok(NULL, ",");
        instructions->imm_a = handle_imm(line_imm_a, labels, num_of_labels);
        char* line_imm_b = strtok(NULL, ",");
        instructions->imm_b = handle_imm(line_imm_b, labels, num_of_labels);

        instructions->result= calculate_instrcuion(instructions->opcode, instructions->rd, instructions->rs, instructions->rt, instructions->rm, instructions->imm_a, instructions->imm_b);
        printf("Result: %012llX\n", instructions->result);
        instructions++;
        counter++;
    }
    return counter;
}

int main(int argc, char* argv[]) {
    unsigned long long hex1 = 21, hex2 = 0xAf, hex3 = 0x521A, hex4 = 0x0F;
    int num_of_labels;
    int num_of_instructions;
    printf("hex1: %012llX\nhex2 : %012llX\nhex3: %012llX\nhex4: %012llX\n", hex1, hex2, hex3, hex4);  //writes 12 hex symbols of a number

    label labels[10];
    instruction instructions[MAX_INSTRUCTIONS_MEM];

    const char* asm_file_name = argv[1];
    printf("%s\n", asm_file_name);

    FILE* asm_file = fopen(asm_file_name, "r");
    if (!asm_file) {
        printf("Error: File not found\n");
        return 1;
    }

    num_of_labels = first_pass(asm_file, labels);
    rewind(asm_file);
    printf("\nLabels:\n");
    for (int i = 0; i < num_of_labels; ++i) {
        printf("Label Name: %s, Address: %03X\n", labels[i].label_name, labels[i].label_addr);
    }
    FILE* imemin_file = fopen("C:\\Users\\tomer\\source\\repos\\ComputerOrganization_ISA\\asm\\asm\\imemin.txt", "w");
    num_of_instructions = second_pass(asm_file, instructions, labels, num_of_labels);
    for (int i = 0; i < num_of_instructions; ++i) {
        fprintf(imemin_file, "%012llX\n",instructions[i].result);
    }

    fclose(asm_file);

    //char input1[] = " out $zero, $zero, $imm2, $imm1, 1, 2 # enable irq2";
    //char input2[] = "\t    sll $sp, $imm1, $imm2, $zero, 1, 11        # set $sp = 1 << 11 = 2048";
    //char input3[] = "\t    out $zero, $imm1, $zero, $imm2, 6, L3        # set irqhandler as L3";

    //char* cleaned1 = clean_line(input1,1);
    //char* cleaned2 = clean_line(input2,1);
    //char* cleaned3 = clean_line(input3,1);

    //printf("Output 1: %s\n", cleaned1);
    //printf("Output 2: %s\n", cleaned2);
    //printf("Output 3: %s\n", cleaned3);

    //char imm1[] = "0x1";
    //char imm2[] = "0x1A3B";
    //char imm3[] = "0xFf2E";
    //char imm4[] = "0x4cD5";
    //char imm5[] = "0xAb9C";
    //char imm6[] = "0xE1Dd";
    //char imm7[] = "fib";


    return 0;
}
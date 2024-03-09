#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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
    while (*line != '\0' && *line!='\n') {
        if (!isblank((unsigned char)*line) && *line != '#') {
            return 0; // Line is not empty
        }
        line++;
    }
    return 1; // Line is empty
}

int line_is_word(char* line) {
    
    int r= (strstr(line, ".word") == NULL) ? 0 : 1;
    
    return r;
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
    if (pass_num == 3) {
        int length = strlen(line);
        int j = 0;
        bool last_char_not_space = true;

        for (int i = 0; i < length; i++) {
            if (!isspace(line[i]) || last_char_not_space) {

                clean_line[j] = line[i];
                j++;
            }
            if (line[i] == ' ') last_char_not_space = false;
            else last_char_not_space = true;
        }

        clean_line[j] = '\0';
        return clean_line;
    }

 }
	

int handle_imm(char* imm, label* labels, int num_of_labels) {
    long long ret;
    for (int i = 0; i < num_of_labels; i++) {
        if (strcmp(imm, labels[i].label_name) == 0) {
            return labels[i].label_addr;
        }
    }
    
    for (int i = 0; i < strlen(imm); i++) {
        imm[i] = tolower(imm[i]);
    }
    if (imm[0] == '0' && imm[1] == 'x') {
        ret = (long int)strtol(imm, NULL, 16);
        
        return ret;
    }
    else {
        ret = (long int)atoi(imm, NULL, 16);
        return ret;
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
        printf("Line |%s|\n", line);

        if (line_is_empty(line)) {
            continue;
        }
        if (line_is_word(line)) {
        	continue;
        }
        if (line_is_label(line)) {
            char* dlr_idx = strchr(line, '$');
            char* cmnt_idx = strchr(line, '#');

            char* cleaned = clean_line(line,1);
            int idx=0;            
            labels->label_addr = addr;
            strcpy(label_p, strtok(cleaned, ":"));
            strcpy(labels->label_name, label_p);
            labels++;
            counter++;
            if (dlr_idx) {
				if (cmnt_idx) {
                    if (dlr_idx > cmnt_idx) {   //command is commented
						continue;
					}
				}
                addr++;
			}
        }
        else {
            addr++;
        }
    }
    return counter;
}

int second_pass(FILE* fptr, instruction* instructions, label* labels, int num_of_labels,long int* dmem) {
    int counter = 0;
    char* cleaned;
    int mem_counter = 0;
    int mem_addr = 0;
    int mem_data = 0;
    int line_cnt = 1;
    while (!feof(fptr)) {
        char line[MAX_LINE_LENGTH + 1] = {0};

        if (!line) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        fgets(line, sizeof(line), fptr);
        printf("Line |%s|\n", line);
        if (line_is_empty(line)) {
            continue;
        }
        else if (line_is_word(line)) {
            cleaned = clean_line(line, 3);
            char* token = strtok(line, "w");
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            mem_addr = handle_imm(token, labels, 0);
            printf("mem_addr: %d\n", mem_addr);
            token = strtok(NULL, " ");
            mem_data = handle_imm(token, labels, 0);
            printf("mem_data: %d\n", mem_data);
            dmem[mem_addr] = mem_data;
            mem_counter++;
            continue;
        }
        else if (line_is_label(line)) {
            //check if it also contains a command. if it does, skip the label and get command

            char* cmnt_idx = strchr(line, '#');
            char* lbl_idx = strchr(line, ':');
            char* dlr_idx = strchr(line, '$');
            if (!dlr_idx) {
                continue;
            }
            else{
                if (cmnt_idx) {
                    if (dlr_idx > cmnt_idx) {   //command is commented
                        continue; 
                    }
                }
                lbl_idx++;
                cleaned = clean_line(lbl_idx, 2);
            }
        
        }
        else {
            cleaned = clean_line(line, 2);
            printf("Cleaned: %s\n", cleaned);
        }
        
        char* line_opcode = strtok(cleaned, ",");
        instructions->opcode = get_opcode_num(line_opcode);
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
    if (argc != 4) {
		printf("Error: Wrong number of arguments\n");
		return 1;
	}
    int num_of_labels;
    int num_of_instructions;
    
    label labels[MAX_INSTRUCTIONS_MEM+1];
    instruction instructions[MAX_INSTRUCTIONS_MEM];

    long int dmem[4096];
    for (int i = 0; i < MAX_INSTRUCTIONS_MEM; i++) {

        dmem[i] = 0;
	}


    const char* asm_file_name = argv[1];
    printf("%s\n", asm_file_name);
    // get asm_file_name without extension

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
    //FILE* imemin_file = fopen("C:\\Users\\tomer\\source\\repos\\ComputerOrganization_ISA\\asm\\asm\\imemin.txt", "w");
    FILE* imemin_file = fopen(argv[2],"w");
    if (!imemin_file) {
		printf("Error: Can't write %s\n", argv[2]);
		return 1;
	}
    num_of_instructions = second_pass(asm_file, instructions, labels, num_of_labels, dmem);
    fclose(asm_file);
    for (int i = 0; i < num_of_instructions; ++i) {
        fprintf(imemin_file, "%012llX\n",instructions[i].result);
    }
    fclose(imemin_file);

    int max_addr = 0;
    for (int i = 4095; i > 0; i--) {
		if (dmem[i] != 0) {
			max_addr = i;
			break;
		}
	}
    //FILE* dmemin_file = fopen("C:\\Users\\tomer\\source\\repos\\ComputerOrganization_ISA\\asm\\asm\\dmemin.txt", "w");
    FILE* dmemin_file = fopen(argv[3], "w");
    if (!dmemin_file) {
        printf("Error: Can't write %s\n", argv[3]);
		return 1;
    }

    if (max_addr != 0) {
        for (int i = 0; i < max_addr+1; i++) {
            fprintf(dmemin_file, "%08X\n", dmem[i]);
        }
    }
    else {
		fprintf(dmemin_file, "00000000\n");
	}
    fclose(dmemin_file);

    return 0;
}


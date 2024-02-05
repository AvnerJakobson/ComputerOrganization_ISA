// Defines and includes
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE 
#define MAX_MEMIN_LINE_SIZE			14			// max size of each line in memin.txt as defined in the project
#define MAX_LINE_DEPTH				4096		// maximum line depth as defined in the project
#define MAX_LINE_DEPTH_DISKIN 		16384		// maximum line depth of the disk as defined in the project
#define REGISTER_NUM				16			// number of registers as defined in the project
#define REGISTER_SIZE				32			// bits size for each register as defined in the project
#define MAX_POSITIVE_IMM_VALUE		2047		// maximun positive value when using 3 hex digits


#include <stdio.h>
#include <stdlib.h>


//////////////////////////////// Structs definitions for simplicity  /////////////////////////////////////
typedef struct instruction {
	int opcode;
	int rd;
	int rs;
	int rt;
	int rm;
	int imm1;
	int imm2;
} Instruction;

typedef struct input_files {
	FILE* imemin;
	FILE* dmemin;
	FILE* diskin;
	FILE* irq2in;
} Input_files;

typedef struct output_files {
	FILE* dmemout;
	FILE* regout;
	FILE* trace;
	FILE* hwregtrace;
	FILE* cycles;
	FILE* leds;
	FILE* diskout;
	FILE* display7seg;
	FILE* monitor_txt;
	FILE* monitor_yuv;
} Output_files;


/////////////////////////////////////// Function declarations /////////////////////////////////////////

// trace
void print_trace(FILE* trace, int* R, int pc, Instruction inst);

void memin_decode(FILE* imemin, Instruction* instructions, int* memory);

int check_input_files(Input_files* files);

int check_output_files(Output_files* files);

Instruction decode_instruction(char* line) ;

void simulation_loop(Instruction* instructions, int* memory, int* registers_array, unsigned int* clk, Input_files* input_files, Output_files* output_files);

void set_registers_imm1_imm2(Instruction* inst, int* registers_array);

////////////////////////////////////////////// Main Code //////////////////////////////////////////////
int main(int argc, char* argv[]) {

	int registers_array[REGISTER_NUM] = { 0 };	// Initialize all registers to 0

	unsigned int CLK = 0;
	unsigned int* clk = &CLK;
	printf("Initial value of clock: %u\n", *clk);
	
	unsigned int PC = 0;
	unsigned int* pc = &PC;


	// Initialize the input and output files to NULL to avoid garbage values

	Input_files input_files = { NULL, NULL, NULL, NULL };
	Output_files output_files = { NULL, NULL, NULL, NULL, NULL, NULL, NULL , NULL, NULL, NULL };

	// Allocate memory for the memory array

	int* memory = (int*)malloc(MAX_LINE_DEPTH * sizeof(memory));
	if (memory == NULL) {
		printf("Memory allocation failed\n");
		exit(1);
	}

	// have an array that stores all the instructions after translating them from the input files
	Instruction instructions[MAX_LINE_DEPTH];

	// Open the input files and store them in the input_files struct
	input_files.imemin = fopen(argv[1], "r");
	input_files.dmemin = fopen(argv[2], "r");
	input_files.diskin = fopen(argv[3], "r");
	input_files.irq2in = fopen(argv[4], "r");

	// check all files have been opened successfully
	if (check_input_files(&input_files) == 1) {
		return 1;
	}

	// Open the output files and store them in the output_files struct
	output_files.dmemout = fopen(argv[5], "w");
	output_files.regout = fopen(argv[6], "w");
	output_files.trace = fopen(argv[7], "w");
	output_files.hwregtrace = fopen(argv[8], "w");
	output_files.cycles = fopen(argv[9], "w");
	output_files.leds = fopen(argv[10], "w");
	output_files.diskout = fopen(argv[11], "w");
	output_files.display7seg = fopen(argv[12], "w");
	output_files.monitor_txt = fopen(argv[13], "w");
	output_files.monitor_yuv = fopen(argv[14], "w");

	// check all files have been opened successfully
	if (check_output_files(&output_files) == 1) {
		return 1;
	}

	// main code goes here- read the input files and store the instructions in the instructions array
	// then execute the instructions and write the output to the output files
	
	// bring the next instruction from address PC

	// decode the instruction according to the encoding
	memin_decode(input_files.imemin, instructions, memory);

	simulation_loop(instructions, memory, registers_array, clk, &input_files, &output_files);

	// end of execution of the instruction

	// print the values of the contents of the data memory to the dmemout file
	// print the values of the the values of the registers R3-R15 to the regout file
	// print the trace of the instruction to the trace file
	// print the values of the hardware registers to the hwregtrace file
	// print the number of cycles to the cycles file
	// print the values of the LEDs to the leds file
	// print the values of the display7seg to the display7seg file
	// print the values of the contents of the disk drive to diskout file
	// print the values of the monitor to the monitor_txt and monitor_yuv files

	// Free allocations and close all files

	free(memory);
	fclose(input_files.imemin);
	fclose(input_files.dmemin);
	fclose(input_files.diskin);
	fclose(input_files.irq2in);
	fclose(output_files.dmemout);
	fclose(output_files.regout);
	fclose(output_files.trace);
	fclose(output_files.hwregtrace);
	fclose(output_files.cycles);
	fclose(output_files.leds);
	fclose(output_files.diskout);
	fclose(output_files.display7seg);
	fclose(output_files.monitor_txt);
	fclose(output_files.monitor_yuv);
	printf("Info: SIMULATOR FINISHED\n");
	return 0;
}




/////////////////////////////////////// Function definitions /////////////////////////////////////////

void print_trace(FILE* trace, int* R, int pc, Instruction inst) {
	fprintf(trace, "%03X %02X%X%X%X%X%03X%03X ", pc, inst.opcode, inst.rd, inst.rs, inst.rt, inst.rm, inst.imm1, inst.imm2);
	fprintf(trace, "%08x %08x %08x %08x ", R[0], R[1], R[2], R[3]);
	fprintf(trace, "%08x %08x %08x %08x ", R[4], R[5], R[6], R[7]);
	fprintf(trace, "%08x %08x %08x %08x ", R[8], R[9], R[10], R[11]);
	fprintf(trace, "%08x %08x %08x %08x\n", R[12], R[13], R[14], R[15]);
}


int check_input_files(Input_files* files) {
	if (files->imemin == NULL || files->dmemin == NULL || files->diskin == NULL || files->irq2in == NULL) {
		printf("Error: One or more input files failed to open.\n");
		return 1;
	}
	else {
		return 0;
	}
}

int check_output_files(Output_files* files) {
	if (files->dmemout == NULL || files->regout == NULL || files->trace == NULL || files->hwregtrace == NULL || files->cycles == NULL || files->leds == NULL || files->diskout == NULL || files->display7seg == NULL || files->monitor_txt == NULL || files->monitor_yuv == NULL) {
		printf("Error: One or more output files failed to open.\n");
		return 1;
	}
	else {
		return 0;
	}
}

void memin_decode(FILE* imemin, Instruction* instructions, int* memory){
	char line[MAX_MEMIN_LINE_SIZE]; 
	int line_number = 0;
	int instruction_number = 0;
	while (fgets(line, MAX_MEMIN_LINE_SIZE, imemin) != NULL && line_number < MAX_LINE_DEPTH) {
		Instruction new_inst = decode_instruction(line);
		instructions[instruction_number] = new_inst;
		line_number++;
		instruction_number++;	
	}
}
	
Instruction decode_instruction(char* line) {
	Instruction inst;
	
	char opcode[3] = 	{ line[0] , line[1] , '\0' };
	char rd[2] 	   =	{ line[2],'\0' };
	char rs[2] 	   =	{ line[3],'\0' };
	char rt[2] 	   =	{ line[4],'\0' };
	char rm[2] 	   =	{ line[5],'\0' };
	char imm1[4]   = 	{ line[6] , line[7] , line[8] , '\0' };
	char imm2[4]   =	{ line[9] , line[10] , line[11] , '\0' };

	inst.opcode    = (int)strtoul(opcode, NULL, 16);
	inst.rd 	   = (int)strtoul(rd, NULL, 16);
	inst.rs 	   = (int)strtoul(rs, NULL, 16);
	inst.rt 	   = (int)strtoul(rt, NULL, 16);
	inst.rm 	   = (int)strtoul(rm, NULL, 16);
	inst.imm1 	   = (int)strtoul(imm1, NULL, 16);
	inst.imm2 	   = (int)strtoul(imm2, NULL, 16);

	return inst;
}

void simulation_loop(Instruction* instructions, int* memory, int* registers_array, unsigned int* clk, Input_files* input_files, Output_files* output_files){
	int next_pc = 0;
	int exit = 0;
	printf("Info: Simulation loop started\n");



	while (exit == 0){ //simulating
		registers_array[0] = 0; // always 0
		set_registers_imm1_imm2(instructions+ next_pc, registers_array);
		print_trace(output_files->trace, registers_array, next_pc, instructions[next_pc]);

		// excecuting the current instruction instruction and get the next PC value for the next itteration


		next_pc += 1;
		*clk += 1;
		if (next_pc == 10){
			printf("Info: Simulation loop finished\n");
			printf("cycles amount = %u\n", *clk);
			exit = 1;
		}
		// finish the calculations of pc to generate the correct trace file	
	}

}

void set_registers_imm1_imm2(Instruction* inst, int* registers_array){
	int full_imm1 = 0;
	int full_imm2 = 0;
	if (inst->imm1 > MAX_POSITIVE_IMM_VALUE)
	{
		full_imm1 = inst->imm1 & 0x00000FFF;
		full_imm1 = inst->imm1 | 0xFFFFF000;
	}
	else
	{
		full_imm1 = inst->imm1;
	}
	if (inst->imm2 > MAX_POSITIVE_IMM_VALUE)
	{
		full_imm2 = inst->imm2 & 0x00000FFF;
		full_imm2 = inst->imm2 | 0xFFFFF000;
	}
	else
	{
		full_imm2 = inst->imm2;
	}

	registers_array[1] = full_imm1;
	registers_array[2] = full_imm2;
}
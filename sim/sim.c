// Defines and includes
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE 
#define MAX_LINE_SIZE				12			// max size of each line in memin.txt as defined in the project
#define MAX_LINE_DEPTH				4096		// maximum line depth as defined in the project
#define MAX_LINE_DEPTH_DISKIN 		16384		// maximum line depth of the disk as defined in the project
#define REGISTER_NUM				16			// number of registers as defined in the project
#define REGISTER_SIZE				32			// bits size for each register as defined in the project

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
} Output_files;

typedef struct monitor_files {
	FILE* display7seg;
	FILE* monitor_txt;
	FILE* monitor_yuv;
} Monitor_files;


/////////////////////////////////////// Function declarations /////////////////////////////////////////

// trace
void print_trace(FILE* trace, int* R, int pc, Instruction inst);

void memin_decode(FILE* imemin, Instruction* instructions, int* memory);

int check_input_files(Input_files* files);

int check_output_files(Output_files* files);

int check_monitor_files(Monitor_files* files);



////////////////////////////////////////////// Main Code //////////////////////////////////////////////
int main(int argc, char* argv[]) {

	int R[REGISTER_NUM] = { 0 };	// Initialize all registers to 0
	int pc = 0;						// Initialize the program counter to 0
	unsigned int CLK = 0;
	unsigned int* clk = &CLK;


	// Initialize the input and output files to NULL to avoid garbage values

	Input_files input_files = { NULL, NULL, NULL, NULL };
	Output_files output_files = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	Monitor_files monitor_files = { NULL, NULL, NULL };

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

	// check all files have been opened successfully
	if (check_output_files(&output_files) == 1) {
		return 1;
	}


	// Open the monitor files and store them in the monitor_files struct
	monitor_files.display7seg = fopen(argv[12], "w");
	monitor_files.monitor_txt = fopen(argv[13], "w");
	monitor_files.monitor_yuv = fopen(argv[14], "w");

	// Check all files have been opened successfully
	if (check_monitor_files(&monitor_files) == 1) {
		return 1;
	}


	// main code goes here- read the input files and store the instructions in the instructions array
	// then execute the instructions and write the output to the output files
	
	// bring the next instruction from address PC

	// decode the instruction according to the encoding
	memin_decode(input_files.imemin, instructions, memory);
	// excecute the instruction

	// update PC to PC+1 (unless it was updated to another value for example by a jump command or an interrupt)

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
	fclose(monitor_files.display7seg);
	fclose(monitor_files.monitor_txt);
	fclose(monitor_files.monitor_yuv);
	printf("SIMULATOR FINISHED\n");
	return 0;
}




/////////////////////////////////////// Function definitions /////////////////////////////////////////

void print_trace(FILE* trace, int* R, int pc, Instruction inst) {
	fprintf(trace, "%03X %02X%X%X%X%X%03X%03X ", pc, inst.opcode, inst.rd, inst.rs, inst.rt, inst.rm, inst.imm1, inst.imm2);
	fprintf(trace, "%08X %08X %08X %08X ", R[0], R[1], R[2], R[3]);
	fprintf(trace, "%08X %08X %08X %08X ", R[4], R[5], R[6], R[7]);
	fprintf(trace, "%08X %08X %08X %08X ", R[8], R[9], R[10], R[11]);
	fprintf(trace, "%08X %08X %08X %08X\n", R[12], R[13], R[14], R[15]);
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
	if (files->dmemout == NULL || files->regout == NULL || files->trace == NULL || files->hwregtrace == NULL || files->cycles == NULL || files->leds == NULL || files->diskout == NULL) {
		printf("Error: One or more output files failed to open.\n");
		return 1;
	}
	else {
		return 0;
	}
}

int check_monitor_files(Monitor_files* files) {
	if (files->display7seg == NULL || files->monitor_txt == NULL|| files->monitor_yuv == NULL) {
		printf("Error: One or more monitor files failed to open.\n");
		return 1;
	}
	else {
		return 0;
	}
}


void memin_decode(FILE* imemin, Instruction* instructions, int* memory){
	char line[MAX_LINE_SIZE];
	int line_number = 0;
	while (fgets(line, MAX_LINE_SIZE, imemin) != NULL && line_number < MAX_LINE_DEPTH) {
		// decode the line and store the instruction in the instructions array
		// also store the instruction in the memory array
		// instructions[i] = decode(line); Store the instruction in the instructions array
		// memory[i] = decode(line); Store the instruction in the memory array
		line_number++;
	}
		
	
}
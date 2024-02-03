// Defines and includes
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE 
#define MAX_LINE_SIZE				500			// max size of each line as defined in the project
#define MAX_LABEL_LENGTH			50			// maximum label length as defined in the project
#define MAX_LINE_DEPTH			4096			// maximum line depth as defined in the project
#define MAX_DISK_LINE_NUM			16384		// max lines in disk as defined in the project
#define WORD_SIZE					32			// word size for each register as defined in the project
#define REGISTER_NUM				16			// number of registers as defined in the project

#include <stdio.h>


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
	FILE* monitor;
} Monitor_files;


/////////////////////////////////////// Function declarations /////////////////////////////////////////

// trace
void print_trace(FILE* trace, int* R, int pc, Instruction inst);

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
	Monitor_files monitor_files = { NULL, NULL };

	// Allocate memory for the memory array

	int* memory = (int*)malloc(MAX_LINE_DEPTH * sizeof(memory));
	if (memory == NULL) {
		printf("Memory allocation failed\n");
		exit(1);
	}

	// have an array that stores all the instructions after translating them from the input files
	Instruction instructions[MAX_LINE_DEPTH];

	// Open the input files and store them in the input_files struct
	input_files.imemin = fopen("imemin.txt", "r");
	input_files.dmemin = fopen("dmemin.txt", "r");
	input_files.diskin = fopen("diskin.txt", "r");
	input_files.irq2in = fopen("irq2in.txt", "r");

	// check all files have been opened successfully
	if (check_input_files(&input_files) == 1) {
		return 1;
	}

	// Open the output files and store them in the output_files struct
	output_files.dmemout = fopen("dmemout.txt", "w");
	output_files.regout = fopen("regout.txt", "w");
	output_files.trace = fopen("trace.txt", "w");
	output_files.hwregtrace = fopen("hwregtrace.txt", "w");
	output_files.cycles = fopen("cycles.txt", "w");
	output_files.leds = fopen("leds.txt", "w");
	output_files.diskout = fopen("diskout.txt", "w");

	// check all files have been opened successfully
	if (check_output_files(&output_files) == 1) {
		return 1;
	}


	// Open the monitor files and store them in the monitor_files struct
	monitor_files.display7seg = fopen("display7seg.txt", "w");
	monitor_files.monitor = fopen("monitor.txt", "w");

	// check all files have been opened successfully
	if (check_monitor_files(&monitor_files) == 1) {
		return 1;
	}

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
	if (files->display7seg == NULL || files->monitor == NULL) {
		printf("Error: One or more monitor files failed to open.\n");
		return 1;
	}
	else {
		return 0;
	}
}
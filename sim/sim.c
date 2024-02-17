// Defines and includes
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE 
#define MAX_MEMIN_LINE_SIZE			14			// max size of each line in memin.txt as defined in the project
#define MAX_LINE_DEPTH				4096		// maximum line depth as defined in the project
#define MAX_LINE_DEPTH_DISKIN 		16384		// maximum line depth of the disk as defined in the project
#define REGISTER_NUM				16			// number of registers as defined in the project
#define IO_REGISTER_NUM				23			// number of IO registers as defined in the project
#define REGISTER_SIZE				32			// bits size for each register as defined in the project
#define MAX_POSITIVE_IMM_VALUE		2047		// maximun positive value when using 3 hex digits
#define MAX_DISK_LINE_DEPTH			16384		// maximum line depth of the disk as defined in the project
#define MAX_DISK_LINE_SIZE			10			// max size of each line in disk.txt as defined in the project
#define MAX_IRQ2IN_LINE_SIZE		10			// max size of each line in irq2in.txt as defined in the project


#include <stdio.h>
#include <string.h>	
#include <stdlib.h>

/////////////////////////////////////////// [TODO] /////////////////////////////////////////
// inputs:
// 		How to handle the diskin file?
// 		How to hadnle the irq2in file?
// 
// functions:
// 		currently trace is correct up until first interrupt- after understanding how to handle the irq2in file, we can add the interrupt handling to the simulation loop and correct the trace file
//		 		
// outputs:
//		hwregtrace file not writing, maybe because there is no interrupt working currently
//		 		
//
//		only trace and hwregtrace are being written to, the rest of the files are not being written to
//
/////////////////////////////////////////// [TODOS] /////////////////////////////////////////


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

void print_trace(FILE* trace, int* R, int pc, Instruction inst);

void print_hwregtrace(FILE* hwregtrace, int is_read, int clk, int IO_reg_number, int DATA);

char* get_IORegister_name(int number);

void imemin_decode(FILE* imemin, Instruction* instructions);

void dmemin_decode(FILE* dmemin, int* memory);

void diskin_decode(FILE* diskin,int* disk);

void irq2in_decode(FILE* irq2in,int* irq2in_memory);

int check_input_files(Input_files* files);

int check_output_files(Output_files* files);

Instruction decode_instruction(char* line);

int power(int base, int exp);

void simulation_loop(Instruction* instructions, int* memory, int* registers_array, unsigned int* clk, Input_files* input_files, Output_files* output_files, int* IORegisters);

int simulate_current_instruction(Instruction inst, int* memory,int* registers_array,Input_files* input_files,Output_files* output_files,int curr_pc, unsigned int* clk, int* IORegisters);

void set_registers_imm1_imm2(Instruction* inst, int* registers_array);

int hex2dec(char hex_line[]);



////////////////////////////////////////////// Main Code //////////////////////////////////////////////
int main(int argc, char* argv[]) {

	int registers_array[REGISTER_NUM] = { 0 };	// Initialize all registers to 0
	int IO_registers_array[IO_REGISTER_NUM] = { 0 };	// Initialize all registers to 0

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
	
	int* disk = (int*)malloc(MAX_DISK_LINE_DEPTH * sizeof(disk));
	if (disk == NULL) {
		printf("Memory allocation failed\n");
		exit(1);
	}

	// TODO is there a max for irq2in.txt file? if so, change the param MAX_DISK_LINE_DEPTH to the correct value
	int* irq2in_memory = (int*)malloc(MAX_DISK_LINE_DEPTH * sizeof(irq2in_memory));
	if (irq2in_memory == NULL) {
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
	

	// decode the input files
	imemin_decode(input_files.imemin, instructions);
	
	dmemin_decode(input_files.dmemin, memory);

	// TODO test when ready
	// diskin_decode(input_files.diskin, disk);

	// TODO test when ready
	//irq2in_decode(input_files.irq2in, irq2in_memory);

	// start the simulation loop

	simulation_loop(instructions, memory, registers_array, clk, &input_files, &output_files, IO_registers_array);

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
	free(disk);
	free(irq2in_memory);
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

void imemin_decode(FILE* imemin, Instruction* instructions){
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

void diskin_decode(FILE* diskin,int* disk){
	// TODO: implement this function and test if the max line depth, max disk line size is correct
	char line[MAX_DISK_LINE_SIZE]; 
	int line_number = 0;
	while (fgets(line, MAX_DISK_LINE_SIZE, diskin) != NULL && line_number < MAX_DISK_LINE_DEPTH) {
		int line_len = strlen(line);
		line[line_len - 1] = '\0';
		int disk_dec = hex2dec(line);
		disk[line_number] = disk_dec;
		line_number++;
	}

}

void irq2in_decode(FILE* irq2in,int* irq2in_memory){
	char line[MAX_IRQ2IN_LINE_SIZE];
	int line_number = 0;
	while (fgets(line, MAX_IRQ2IN_LINE_SIZE, irq2in) != NULL && line_number < MAX_DISK_LINE_DEPTH) {
		int line_len = strlen(line);
		line[line_len - 1] = '\0';
		int irq2in_dec = atoi(line);
		irq2in_memory[line_number] = irq2in_dec;
		line_number++;
	}
}	


int power(int base, int exp) {
	if (exp == 0)
		return 1;
	return base * power(base, exp - 1);
}

int hex2dec(char hex_line[]) {
	int dec = strtoll(hex_line, NULL, 16);

	if (hex_line[0] >= '8' && hex_line[0] <= 'f') {		// Negetive number check
		dec -= power(2, REGISTER_SIZE);						 
	}

	return dec;
}

void dmemin_decode(FILE* imemin ,int* memory){
	char line[MAX_MEMIN_LINE_SIZE]; 
	int line_number = 0;
	while (fgets(line, MAX_MEMIN_LINE_SIZE, imemin) != NULL && line_number < MAX_LINE_DEPTH) {
		int line_len = strlen(line);
		line[line_len - 1] = '\0';
		int mem_dec = hex2dec(line);
		memory[line_number] = mem_dec;
		line_number++;
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

void simulation_loop(Instruction* instructions, int* memory, int* registers_array, unsigned int* clk, Input_files* input_files, Output_files* output_files, int* IORegisters){
	int next_pc = 0;
	int exit = 0;
	printf("Info: Simulation loop started\n");



	while (exit == 0){ //simulating
		registers_array[0] = 0; // always 0
		set_registers_imm1_imm2(instructions+ next_pc, registers_array);
		print_trace(output_files->trace, registers_array, next_pc, instructions[next_pc]);

		// executing the current instruction instruction and get the next PC value for the next iteration
		next_pc = simulate_current_instruction(instructions[next_pc], memory, registers_array, input_files, output_files, next_pc, clk, IORegisters);


		*clk += 1;
		if (*clk == 200){ 
			printf("Info: Simulation loop stopped");
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

int simulate_current_instruction(Instruction inst, int* memory,int* registers_array,Input_files* input_files,Output_files* output_files,int curr_pc, unsigned int* clk, int* IORegisters){
	int next_pc = 0;

	switch (inst.opcode)
	{
	case 0: {  //add
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			registers_array[inst.rd] = registers_array[inst.rs] + registers_array[inst.rt] + registers_array[inst.rm];
		next_pc = curr_pc + 1;
		break;
	}
	
	case 1: {  //sub
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			registers_array[inst.rd] = registers_array[inst.rs] - registers_array[inst.rt] - registers_array[inst.rm];
		next_pc = curr_pc + 1;
		break;
	}
	
	case 2: {  //mac
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			registers_array[inst.rd] = registers_array[inst.rs] * registers_array[inst.rt] + registers_array[inst.rm];
		next_pc = curr_pc + 1;
		break;
	}
	
	case 3: {  //and
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			registers_array[inst.rd] = registers_array[inst.rs] & registers_array[inst.rt] & registers_array[inst.rm];
		next_pc = curr_pc + 1;
		break;
	}
	
	case 4: {  //or
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			registers_array[inst.rd] = registers_array[inst.rs] | registers_array[inst.rt] | registers_array[inst.rm];
		next_pc = curr_pc + 1;
		break;
	}
	
	case 5: {  //xor
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			registers_array[inst.rd] = registers_array[inst.rs] ^ registers_array[inst.rt] ^ registers_array[inst.rm];
		next_pc = curr_pc + 1;
		break;
	}
	
	case 6: {  //sll
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			registers_array[inst.rd] = registers_array[inst.rs] << registers_array[inst.rt];
		next_pc = curr_pc + 1;
		break;
	}
	
	case 7: {  //sra
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			registers_array[inst.rd] = registers_array[inst.rs] >> registers_array[inst.rt];
		next_pc = curr_pc + 1;
		break;
	}

	case 8: {  //srl
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			// [TODO] TEST THIS 
			registers_array[inst.rd] = ((unsigned int)registers_array[inst.rs] >> registers_array[inst.rt]);
		next_pc = curr_pc + 1;
		break;
	}
	
	case 9: {  //beq
		if (registers_array[inst.rs] == registers_array[inst.rt])
			next_pc = registers_array[inst.rm] & 0xFFF;
		else
			next_pc = curr_pc + 1;
		break;
	}
	
	case 10: { //bne
		if (registers_array[inst.rs] != registers_array[inst.rt])
			next_pc = registers_array[inst.rm] & 0xFFF;
		else
			next_pc = curr_pc + 1;
		break;
	}
	
	case 11: { //blt
		if (registers_array[inst.rs] < registers_array[inst.rt])
			next_pc = registers_array[inst.rm] & 0xFFF;
		else
			next_pc = curr_pc + 1;
		break;
	}
	
	case 12: { //bgt
		if (registers_array[inst.rs] > registers_array[inst.rt])
			next_pc = registers_array[inst.rm] & 0xFFF;
		else
			next_pc = curr_pc + 1;
		break;
	}
	
	case 13: { //ble
		if (registers_array[inst.rs] <= registers_array[inst.rt])
			next_pc = registers_array[inst.rm] & 0xFFF;
		else
			next_pc = curr_pc + 1;
		break;
	}
	
	case 14: { //bge
		if (registers_array[inst.rs] >= registers_array[inst.rt])
			next_pc = registers_array[inst.rm] & 0xFFF;
		else
			next_pc = curr_pc + 1; 
		break;
	}
	
	case 15: { //jal
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			registers_array[inst.rd] = curr_pc + 1;
		next_pc = registers_array[inst.rm] & 0xFFF;
		break;
	}
	
	case 16: { //lw
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			registers_array[inst.rd] = memory[registers_array[inst.rs] + registers_array[inst.rt]] + registers_array[inst.rm];
		next_pc = curr_pc + 1;
		break;
	}
	
	case 17: { //sw
		memory[registers_array[inst.rs] + registers_array[inst.rt]] = registers_array[inst.rm] + registers_array[inst.rd];
		next_pc = curr_pc + 1;
		break;
	}
	
	case 18: { //reti
		next_pc = IORegisters[7]; 
		break;
	}
	
	case 19: { //in
		if (inst.rd < 3)
			printf("Info: Cannot change register $zero,$imm1,$imm2 \n");
		else
			registers_array[inst.rd] = IORegisters[registers_array[inst.rs] + registers_array[inst.rt]];
		print_hwregtrace(output_files->hwregtrace, 1, *clk, registers_array[inst.rs] + registers_array[inst.rt], IORegisters[registers_array[inst.rs]+ registers_array[inst.rt]]); 
		next_pc = curr_pc + 1;
		break;
	}
	
	case 20: { //out
		IORegisters[registers_array[inst.rs] + registers_array[inst.rt]] = registers_array[inst.rm]; 
		next_pc = curr_pc + 1;
		print_hwregtrace(output_files->hwregtrace, 0, *clk, registers_array[inst.rs] + registers_array[inst.rt], registers_array[inst.rm]);
		break;
	}
	
	case 21: { //halt
		next_pc = -1;
		break;
	}

	default:
		printf("Error: Invalid opcode. Jumping to next PC\n");
		next_pc = curr_pc + 1;
		break;

	}

	return next_pc;
}

void print_hwregtrace(FILE* hwregtrace, int is_read, int clk, int IO_reg_number, int DATA){
	char* IORegister_name = get_IORegister_name(IO_reg_number);
	if (is_read==1)
		fprintf(hwregtrace, "%d READ %s %08X\n",clk, IORegister_name, DATA);
	else
		fprintf(hwregtrace, "%d WRITE %s %08X\n",clk,IORegister_name, DATA);
}

char* get_IORegister_name(int number) {
	switch (number) {
		case 0: return "irq0enable";
		case 1: return "irq1enable";
		case 2: return "irq2enable";
		case 3: return "irq0status";
		case 4: return "irq1status";
		case 5: return "irq2status";
		case 6: return "irqhandler";
		case 7: return "irqreturn";
		case 8: return "clks";
		case 9: return "leds";
		case 10: return "display7seg";
		case 11: return "timerenable";
		case 12: return "timercurrent";
		case 13: return "timermax";
		case 14: return "diskcmd";
		case 15: return "disksector";
		case 16: return "diskbuffer";
		case 17: return "diskstatus";
		case 18: return "reserved";
		case 19: return "reserved";
		case 20: return "monitoraddr";
		case 21: return "monitordata";
		case 22: return "monitorcmd";
	}
}
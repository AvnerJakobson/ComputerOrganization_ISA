// Defines and includes
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE 
#define MAX_MEMIN_LINE_SIZE			14			// max size of each line in memin.txt as defined in the project
#define MAX_LINE_DEPTH				4096		// maximum line depth as defined in the project
#define REGISTER_NUM				16			// number of registers as defined in the project
#define IO_REGISTER_NUM				23			// number of IO registers as defined in the project
#define REGISTER_SIZE				32			// bits size for each register as defined in the project
#define MAX_POSITIVE_IMM_VALUE		2047		// maximun positive value when using 3 hex digits
#define MAX_DISK_LINE_DEPTH			65536		// maximum line depth of the disk as defined in the project
#define DISK_SECTOR_SIZE			512			// size of each sector in the disk as defined in the project
#define DISK_SECTOR_NUM				128			// size of each sector in the disk as defined in the project
#define PIXELS_IN_ROW_OR_COLUMN		256			// number of pixels in a row or column as defined in the project


#include <stdio.h>
#include <string.h>	
#include <stdlib.h>

/////////////////////////////////////////// [TODO] /////////////////////////////////////////
// Todos:
//		
//		Interruptions:
//		* When does the irq (0/1) status change back to zero?
//		* Make sure that the algorithm for interrupts is ok. If 2 interrupts happen at the same time, what should happen to the status of the interrupt not handled?
//		
//		Timer:
//		* Check the implementation of the timer
//
//		General Todos:
// 		* Test the disk implementation
//		* What happens if one of the input files are missing?
//
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

int get_next_irq2(FILE* irq2in, int curr_irq2);

int check_input_files(Input_files* files);

int check_output_files(Output_files* files);

Instruction decode_instruction(char* line);

int power(int base, int exp);

void simulation_loop(Instruction* instructions, int* memory, int* registers_array, unsigned int* clk, Input_files* input_files, Output_files* output_files, int* IORegisters, int* monitor_matrix, int* disk);

int simulate_current_instruction(Instruction inst, int* memory,int* registers_array,Input_files* input_files,Output_files* output_files,int curr_pc, unsigned int* clk, int* IORegisters, int* in_isr, int* monitor_matrix, int* disk, int* disk_process_completion_cycle);

void set_registers_imm1_imm2(Instruction* inst, int* registers_array);

int hex2dec(char hex_line[]);

void create_cycles_file(FILE* cycles, unsigned int* clk);

void create_regout_file(FILE* regout, int* registers_array);

void create_dmemout_file(FILE* dmemout, int* memory);

void print_leds(FILE* leds,unsigned int* clk, int leds_value);

void print_display7seg(FILE* display7seg,unsigned int* clk, int display7seg_value);

void increment_clks_IORegister(int* IORegisters);

void increment_timer_and_trigger_irq0(int* IORegisters);

void create_monitor_files(FILE* monitor_txt, FILE* monitor_yuv, int* monitor_matrix);

void create_diskout_file(FILE* diskout, int* disk);



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
	memset(memory, 0, MAX_LINE_DEPTH * sizeof(memory));	// init memory to zero
	
	int* disk = (int*)malloc(MAX_DISK_LINE_DEPTH * sizeof(disk));
	if (disk == NULL) {
		printf("Memory allocation failed\n");
		exit(1);
	}
	memset(disk, 0, MAX_DISK_LINE_DEPTH * sizeof(disk)); // init disk to zero

	int* monitor_matrix = (int*)malloc( PIXELS_IN_ROW_OR_COLUMN*PIXELS_IN_ROW_OR_COLUMN * sizeof(monitor_matrix));
	if (monitor_matrix == NULL) {
		printf("Memory allocation failed\n");
		exit(1);
	}
	memset(monitor_matrix, 0, PIXELS_IN_ROW_OR_COLUMN*PIXELS_IN_ROW_OR_COLUMN * sizeof(monitor_matrix)); // init monitor_matrix to zero



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
	output_files.dmemout     = fopen(argv[5], "w");
	output_files.regout      = fopen(argv[6], "w");
	output_files.trace       = fopen(argv[7], "w");
	output_files.hwregtrace  = fopen(argv[8], "w");
	output_files.cycles      = fopen(argv[9], "w");
	output_files.leds        = fopen(argv[10], "w");
	output_files.display7seg = fopen(argv[11], "w");
	output_files.diskout     = fopen(argv[12], "w");
	output_files.monitor_txt = fopen(argv[13], "w");
	output_files.monitor_yuv = fopen(argv[14], "wb");

	// check all files have been opened successfully
	if (check_output_files(&output_files) == 1) {
		return 1;
	}
	

	// decode the input files
	imemin_decode(input_files.imemin, instructions);
	
	dmemin_decode(input_files.dmemin, memory);

	diskin_decode(input_files.diskin, disk);


	// start the simulation loop
	simulation_loop(instructions, memory, registers_array, clk, &input_files, &output_files, IO_registers_array, monitor_matrix, disk);

	// end of execution of the instruction

	// print the values of the contents of the disk drive to diskout file
	create_diskout_file(output_files.diskout, disk);

	// print the values of the monitor to the monitor_txt and monitor_yuv files
	create_monitor_files(output_files.monitor_txt, output_files.monitor_yuv, monitor_matrix);
	
	// print the values of the contents of the data memory to the dmemout file
	create_dmemout_file(output_files.dmemout, memory);
	
	// print the values of the the values of the registers R3-R15 to the regout file
	create_regout_file(output_files.regout, registers_array);

	// print the number of cycles to the cycles file
	create_cycles_file(output_files.cycles, clk);
	
	// Free allocations and close all files

	free(memory);
	free(disk);
	free(monitor_matrix);
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

void print_leds(FILE* leds, unsigned int* clk, int leds_value) {
	fprintf(leds, "%d %08x\n", *clk, leds_value);
}

void print_display7seg(FILE* display7seg,unsigned int* clk, int display7seg_value){
	// Convert display7seg_value to individual digits
	int digit8 = (display7seg_value >> 28) & 0xF;
	int digit7 = (display7seg_value >> 24) & 0xF;
	int digit6 = (display7seg_value >> 20) & 0xF;
	int digit5 = (display7seg_value >> 16) & 0xF;
	int digit4 = (display7seg_value >> 12) & 0xF;
	int digit3 = (display7seg_value >> 8) & 0xF;
	int digit2 = (display7seg_value >> 4) & 0xF;
	int digit1 = display7seg_value & 0xF;

	// Print the digits to the display7seg file
	fprintf(display7seg, "%d %x%x%x%x%x%x%x%x\n", *clk, digit8, digit7, digit6, digit5, digit4, digit3, digit2, digit1);
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
	char line[MAX_MEMIN_LINE_SIZE]; 
	int line_number = 0;
	while (fgets(line, MAX_MEMIN_LINE_SIZE, diskin) != NULL && line_number < MAX_DISK_LINE_DEPTH) {
		int line_len = strlen(line);
		line[line_len - 1] = '\0';
		int disk_dec = hex2dec(line);
		disk[line_number] = disk_dec;
		line_number++;
	}

}

int get_next_irq2(FILE* irq2in, int curr_irq2) {
	char line[256]; // Buffer to hold the line.
	int next_irq2 = -1;

	if (fgets(line, sizeof(line), irq2in) == NULL) {
		return -1; // Return -1 if we've reached the end of the file or an error occurred.
	}
	next_irq2 = atoi(line); // Convert the line to an integer and return it if it's valid.
	if (next_irq2 < curr_irq2) {
		return -1;
	}
	return next_irq2; // Convert the line to an integer and return it.
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

void simulation_loop(Instruction* instructions, int* memory, int* registers_array, unsigned int* clk, Input_files* input_files, Output_files* output_files, int* IORegisters, int* monitor_matrix, int* disk){
	int next_pc = 0;
	int exit = 0;
	int next_irq2 = -1;
	int irq = 0;
	int IN_ISR = 0;
	int* in_isr = &IN_ISR;
	unsigned int DISK_PROCESS_COMPLETION_CYCLE = 0;
	unsigned int* disk_process_completion_cycle = &DISK_PROCESS_COMPLETION_CYCLE;

	printf("Info: Simulation loop started\n");
	
	
	// Check for inital value of next_irq2
	next_irq2 = get_next_irq2(input_files->irq2in, next_irq2);

	while (exit == 0){ //simulating


		// ISR Handling

		irq = (IORegisters[0] & IORegisters[3]) | (IORegisters[1] & IORegisters[4]) | (IORegisters[2] & IORegisters[5]);

		if (irq == 1 && *in_isr == 0) { // if an interrupt is triggered and we are not in an ISR
			*in_isr = 1;
			IORegisters[7] = next_pc;
			next_pc = IORegisters[6];
		}

		if (*clk == *disk_process_completion_cycle && *disk_process_completion_cycle != 0){ // irq1 is triggered when the disk process is completed
			IORegisters[17] = 0; // disk is not busy anymore
			IORegisters[14] = 0; // diskcmd marks the disk is not busy anymore
			*disk_process_completion_cycle = 0; // reset the disk process completion cycle
			IORegisters[4] = 1; // irq 1 is triggered to notify the disk process completion

		}

		if (*clk == next_irq2) {
			IORegisters[5] = 1; // irq 2 is triggered
			next_irq2 = get_next_irq2(input_files->irq2in, next_irq2);
		}



		registers_array[0] = 0; // always 0
		set_registers_imm1_imm2(instructions+ next_pc, registers_array);
				
		print_trace(output_files->trace, registers_array, next_pc, instructions[next_pc]);

		// executing the current instruction instruction and get the next PC value for the next iteration
		next_pc = simulate_current_instruction(instructions[next_pc], memory, registers_array, input_files, output_files, next_pc, clk, IORegisters, in_isr, monitor_matrix, disk, disk_process_completion_cycle);

		*clk += 1;

		increment_timer_and_trigger_irq0(IORegisters);

		increment_clks_IORegister(IORegisters);
		if (next_pc == -1) { // HALT - exiting the simulator
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

int simulate_current_instruction(Instruction inst, int* memory,int* registers_array,Input_files* input_files,Output_files* output_files,int curr_pc, unsigned int* clk, int* IORegisters, int* in_isr, int* monitor_matrix, int* disk, int* disk_process_completion_cycle){
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
		*in_isr = 0;
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
		
		if(registers_array[inst.rs] + registers_array[inst.rt] == 22) // Reading a command from the monitor
			IORegisters[22] = 0; // After reading, return 0

		break;
	}
	
	case 20: { //out
		if ((registers_array[inst.rs] + registers_array[inst.rt]) == 18 || (registers_array[inst.rs] + registers_array[inst.rt]) == 19){
			printf("Warning: Cannot change register 18-19 in IOregisters\n");
			break;
		}

		IORegisters[registers_array[inst.rs] + registers_array[inst.rt]] = registers_array[inst.rm];
		switch(registers_array[inst.rs] + registers_array[inst.rt])
		{
			case 9:{ //leds
				print_leds(output_files->leds, clk, registers_array[inst.rm]);
				break;
			}

			case 10:{ //display7seg
				// print the values of the display7seg to the display7seg file
				print_display7seg(output_files->display7seg, clk, registers_array[inst.rm]);
				break;
			}

			case 14:{ //diskcmd
				if (IORegisters[17] == 0){ // if the disk is not busy
					if (IORegisters[14] == 1){ // read
						IORegisters[17] = 1; // disk is now busy
						for (int i=0; i< DISK_SECTOR_SIZE; i++){
							memory[registers_array[16] + i] = disk[registers_array[15]*DISK_SECTOR_SIZE + i]; // read the sector from the disk to the memory in the specified address
						}
						*disk_process_completion_cycle = *clk + 1024;
					}
					else if (IORegisters[14] == 2){ // write
						IORegisters[17] = 1; // disk is now busy
						for (int i=0; i< DISK_SECTOR_SIZE; i++){
        					disk[registers_array[15]*DISK_SECTOR_SIZE + i] = memory[registers_array[16] + i]; // write the sector from the memory to the disk in the specified address
						}
 						*disk_process_completion_cycle = *clk + 1024;
					}
				}
				break;
			}

			case 22:{ //monitorcmd
				if (IORegisters[22] == 1)
					monitor_matrix[IORegisters[20]]= IORegisters[21];
						
				break;
			}

		}
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
		fprintf(hwregtrace, "%d READ %s %08x\n",clk, IORegister_name, DATA);
	else
		fprintf(hwregtrace, "%d WRITE %s %08x\n",clk,IORegister_name, DATA);
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

void create_cycles_file(FILE* cycles, unsigned int* clk){
	fprintf(cycles, "%d\n", *clk);
}

void create_regout_file(FILE* regout, int* registers_array){
	for (int i = 3; i < REGISTER_NUM; i++) {
		fprintf(regout, "%08X\n", registers_array[i]);
	}
}


void create_dmemout_file(FILE* dmemout, int* memory){
	int last_non_zero_line = 0;
	for (int i = 0; i < MAX_LINE_DEPTH; i++) {
		if (memory[i] != 0) {
			last_non_zero_line = i;
		}
	}

	if (last_non_zero_line != 0) {
		for (int i = 0; i < MAX_LINE_DEPTH; i++) {
			fprintf(dmemout, "%08X\n", memory[i]);
			if (i == last_non_zero_line) {
				break;
			}
		}
	}
}

void increment_clks_IORegister(int* IORegisters) {
	if (IORegisters[8] == 0xFFFFFFFF)
		IORegisters[8] = 0;
	else
		IORegisters[8] ++;
}

void increment_timer_and_trigger_irq0(int* IORegisters) {
	if (IORegisters[11]== 1)
	{
		if (IORegisters[12] >= IORegisters[13]){
			IORegisters[3] = 1;
			IORegisters[12] = 0;
		}
		else
			IORegisters[12] ++;
			IORegisters[3] = 1; // [TODO] is this correct? should the irq change back to 0 after making sure that the ISR is called? (maybe it was called about irq2 and not irq0)

	}
}

void create_monitor_files(FILE* monitor_txt, FILE* monitor_yuv, int* monitor_matrix){
	int last_non_zero_line = 0;
	for (int i = 0; i < PIXELS_IN_ROW_OR_COLUMN*PIXELS_IN_ROW_OR_COLUMN; i++) {
		if (monitor_matrix[i] != 0) {
			last_non_zero_line = i;
		}
	}

	if (last_non_zero_line != 0) {
		for (int i = 0; i < PIXELS_IN_ROW_OR_COLUMN*PIXELS_IN_ROW_OR_COLUMN; i++) {
			fprintf(monitor_txt, "%02X\n", monitor_matrix[i]); // Write up untill last non zero line

			if (i == last_non_zero_line) {
				break;
			}
		}
	}

	for (int i = 0; i < PIXELS_IN_ROW_OR_COLUMN*PIXELS_IN_ROW_OR_COLUMN; i++) {
		fprintf(monitor_yuv, "%c", monitor_matrix[i]); // Write all the lines
	}
	
}

void create_diskout_file(FILE* diskout, int* disk){
	int last_non_zero_line = 0;
	for (int i = 0; i < MAX_DISK_LINE_DEPTH; i++) {
		if (disk[i] != 0) {
			last_non_zero_line = i;
		}
	}

	if (last_non_zero_line != 0) {
		for (int i = 0; i < MAX_DISK_LINE_DEPTH; i++) {
			fprintf(diskout, "%08X\n", disk[i]);
			if (i == last_non_zero_line) {
				break;
			}
		}
	}
}
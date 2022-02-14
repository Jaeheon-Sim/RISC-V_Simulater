//2018113528 심재헌

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// memory
#define INST_MEM_SIZE 32*1024
#define DATA_MEM_SIZE 32*1024
unsigned long inst_mem[INST_MEM_SIZE];		 //instruction memory
unsigned long long data_mem[DATA_MEM_SIZE]; //data memory

// variable
unsigned long long fetch_data;		// fetch data
int type;							// 0: R, 1: I, 2: S 3: SB, 4: U 5: UJ
char opcode[8]= "";					// opcode
char rd[6] = "";					// rd
char funct3[4] = "";				// func3
char rs1[6] = "";					// rs1
char rs2[6] = "";					// rs2
char funct7[8]= "";					//funct7
char imm_u_result[13];				// imm_u string shift before
char beq_imm_result[21];			// beq_value string shift before
int rd1_index;						// rs1 value
int rd2_index;						// rs2 value
int rd_index;						// rd value
int data_index;						// memory index
int rd1_value;						// rs1 data value
int rd2_value;						// rs2 data value
int rd_value;						// rd data value
long long imm_value;				// imm value
long long arth_value;				// arth result value
int mem_value;						// memory data value
int beq_value;						// beq control 1 : beq 2 : bne


//clock cycles
long long cycles = 0;
// registers
long long int regs[32];

// program counter
unsigned long pc = 0;

//misc. function
int init(char* filename);
int take_address(char* binary);			// take a address value
long long take_imm(char* binary);		// take a immidiate value
void print_cycles();
void print_reg();
void print_pc();
void rType();
void iType();
void sType();
void sbType();
void ujType();

//fetch an instruction from a instruction memory
void fetch() {
	fetch_data= inst_mem[pc/4]; // fetch data
	pc+=4;
}

//decode the instruction and read data from register file
void decode() {
	char imm[13]="";		//immidiate string
	char imm_U[21]="";		// imm_u	string
	char beq_imm[14]="";	// beq_imm	string
    char translate[256];	// binary string
	char bin[256] = "";		// whole binary string
	int tr_cnt=0;			// cnt
	
	sprintf(translate,"%x", fetch_data);	//hex to string
	
	for(int i=0;translate[i] != '\0';i++){ // count
		tr_cnt++;
	}
	
	switch(tr_cnt){		// make whole string
		case 1:
			strcat(bin,"0000000000000000000000000000");
			break;	
		case 2:
			strcat(bin,"000000000000000000000000");
			break;
		case 3:
			strcat(bin,"00000000000000000000");
			break;
		case 4:
			strcat(bin,"0000000000000000");
			break;	
		case 5:
			strcat(bin,"000000000000");
			break;
		case 6:
			strcat(bin,"00000000");
			break;
		case 7:
			strcat(bin,"0000");
			break;
		default:
			break;
	}
	
	for(int i=0; translate[i] != '\0'; i++){ // translate 
		
		switch(translate[i]){
			case '0' :
				strcat(bin,"0000");	
				break;
			case '1' :
				strcat(bin,"0001");	
				break;
			case '2' :
				strcat(bin,"0010");	
				break;
			case '3' :
				strcat(bin,"0011");	
				break;
			case '4' :
				strcat(bin,"0100");	
				break;	
			case '5' :
				strcat(bin,"0101");	
				break;
			case '6' :
				strcat(bin,"0110");	
				break;
			case '7' :
				strcat(bin,"0111");	
				break;
			case '8' :
				strcat(bin,"1000");	
				break;
			case '9' :
				strcat(bin,"1001");	
				break;
			case 'a' :
			case 'A' :
				strcat(bin,"1010");	
				break;
			case 'b' :
			case 'B' :
				strcat(bin,"1011");	
				break;
			case 'c' :
			case 'C' :
				strcat(bin,"1100");	
				break;
			case 'd' :
			case 'D' :
				strcat(bin,"1101");	
				break;
			case 'e' :
			case 'E' :
				strcat(bin,"1110");	
				break;
			case 'f' :
			case 'F' :
				strcat(bin,"1111");	
				break;	
			default:
				printf("wrong input\n");
				exit(1);
		}
		
	}
	// take opcode
	strncpy(opcode,bin+25,7);
	
	// decode opcode
	if(strcmp(opcode,"0110011")==0){	//Rtype
		type =0;
		//decollate
		strncpy(funct7,bin,7);	
		strncpy(rs2,bin+7,5);
		strncpy(rs1,bin+12,5);
		strncpy(funct3,bin+17,3);
		strncpy(rd,bin+20,5);
		//calculate value
		rd1_index=take_address(rs1);
		rd2_index=take_address(rs2);
		rd_index=take_address(rd);
		rd1_value=regs[rd1_index];
		rd2_value=regs[rd2_index];
	}
	else if(strcmp(opcode,"0000011")==0 ||strcmp(opcode,"0010011")==0||strcmp(opcode,"1100111")==0){//Itype
		type=1;
		//decollate
		strncpy(rd,bin+20,5);
		strncpy(funct3,bin+17,3);
		strncpy(rs1,bin+12,5);		
		strncpy(imm,bin,12);
		//calculate value
		imm_value=take_imm(imm);
		rd_index=take_address(rd);
		rd1_index=take_address(rs1);
		rd_value=regs[rd_index];
		rd1_value=regs[rd1_index];
	}
	else if(strcmp(opcode,"0100011")==0){		//Stype
		type = 2;
		// decollate
		strncpy(rs1,bin+7,5);
		strncpy(rs2,bin+12,5);		
		strncpy(imm,bin,7);	
		strncat(imm,bin+20,5);	
		strncpy(funct3,bin+17,3);
		// calculate value
		imm_value=take_imm(imm);
		rd1_index=take_address(rs1);
		rd2_index=take_address(rs2);
		rd1_value=regs[rd1_index];
		rd2_value=regs[rd2_index];
	}
	else if(strcmp(opcode,"1100011")==0){		//SBtype
		type = 3;
		// decollate
		strncpy(beq_imm,bin,1);		//12
		strncat(beq_imm,bin+24,1);	//11
		strncat(beq_imm,bin+1,6);	//10~5
		strncat(beq_imm,bin+20,4);	//4
		strcpy(beq_imm_result,beq_imm);
		
		strncpy(rs2,bin+7,5);
		strncpy(rs1,bin+12,5);
		strncpy(funct3,bin+17,3);
		
		// calculate
		rd1_index=take_address(rs1);
		rd2_index=take_address(rs2);
		rd1_value=regs[rd1_index];
		rd2_value=regs[rd2_index];
		
	}else if(strcmp(opcode,"1101111")==0){		//Ujtype jal
		type = 5;
		//decollate
		strncpy(rd,bin+20,5);
		
		strncpy(imm_U,bin,1); // 20
		strncat(imm_U,bin+12,8);// 19~12
		strncat(imm_U,bin+11,1);//11
		strncat(imm_U,bin+1,10);//10~1
		strcpy(imm_u_result,imm_U);
		// calculate value
		rd_index=take_address(rd);		
		rd_value=regs[rd_index];
	}
}

int take_address(char* binary){	// calculate address
	int multiply = 1;
	long long add_value = 0;
	
	for(int i=4;i>=0;i--){
		add_value=(binary[i]-48)*multiply+add_value;
		multiply*=2;
	}
	return add_value;
}

long long take_imm(char* binary){// calculate imm_value
	int multiply=1;
	long long add_value=0;
	
	if(type==5){// ujtype
		int sign=0;
			if(binary[0]=='1'){ // if neg
				sign=1;
				for (int i = 0; i <=20;i++) {
					if (binary[i]=='0') {// do not operation
						binary[i] = '1';
					}
					else {
						binary[i] = '0';
					}
				}
			}
			for(int i=20;i>=0;i--){ // calculate imm_value
					add_value=(binary[i]-48)*multiply+add_value;
					multiply*=2;
			}
			if(sign==1){ // if neg do not operation
				add_value=~add_value;
			}
	}else{
		if(type==3){ // beq
			int sign=0;
			if(binary[0]=='1'){ // if neg
				sign=1;
				for (int i = 0; i <=12;i++) {
					if (binary[i]=='0') {
						binary[i] = '1';
					}
					else {
						binary[i] = '0';
					}
				}
			}
			for(int i=12;i>=0;i--){ // calculate imm_value
					add_value=(binary[i]-48)*multiply+add_value;
					multiply*=2;
			}
			if(sign==1){ // if neg
				add_value=~add_value; // do not operation
			}
		}else{ // I
			int sign=0;
			if(binary[0]=='1'){ // if neg
				sign=1;
				for (int i = 0; i <=11;i++) {
					if (binary[i]=='0') {
						binary[i] = '1';
					}
					else {
						binary[i] = '0';
					}
				}
			}
			for(int i=11;i>=0;i--){ // calculate imm_value
					add_value=(binary[i]-48)*multiply+add_value;
					multiply*=2;
			}
			if(sign==1){ // if neg do not operation
				add_value=~add_value;
			}
		}
	}
	return add_value;
}

//perform the appropriate operation 
void exe() {
	//add addi jal jalr ld sd beq
	if(type==0){ // R add
		rType();	
	}else if(type==1){ // I ld addi jalr
		iType();
	}else if(type==2){// S sd
		sType();
	}else if(type==3){// SB beq
		sbType();
	}else if(type==5){// UJ jal
		ujType();
	}
}

void rType(){ // add
	if(strcmp(funct3,"000")==0&&strcmp(funct7,"0000000")==0){ // add
		arth_value=rd1_value+rd2_value;
	}
}

void iType(){ // i type
	if(strcmp(funct3,"011")==0){ // ld
		data_index=imm_value+rd1_value;
	}
	else if(strcmp(opcode,"1100111")==0&&strcmp(funct3,"000")==0){// jalr
		
	}
	else if(strcmp(opcode,"0010011")==0&&strcmp(funct3,"000")==0){// addi
		arth_value=rd1_value+imm_value;
	}
}

void sType(){
	if(strcmp(funct3,"011")==0){ // sd
		data_index=imm_value+rd2_value;
	}
}

void sbType(){
	if(strcmp(funct3,"000")==0){// beq
		strcat(beq_imm_result,"0"); 		//shift
		imm_value=take_imm(beq_imm_result);
		if(rd1_value!=rd2_value){
			beq_value=0;
		}
		else{
			beq_value=1;
		}
	}
}

void ujType(){// jal
	strcat(imm_u_result,"0"); // shift
	imm_value=take_imm(imm_u_result);
}

//access the data memory
void mem() {
	if(type==1 && strcmp(funct3,"011")==0){ //ld
		mem_value=data_mem[data_index];
	}else if(type ==2){//sd
		data_mem[data_index]=rd1_value;		
	}else if(type==3){ 												// beq
		if(beq_value==1){
			pc=imm_value+pc-4;										// move progran counter
		}
	}
}

//write result of arithmetic operation or data read from the data memory if required
void wb() {
	
	if(type==0){													// add
		if(rd_index!=0){
			regs[rd_index]= arth_value;								//store arth_value
		}
	}else if(type ==1){
		if(strcmp(opcode,"0010011")==0&&strcmp(funct3,"000")==0){	// addi
			if(rd_index != 0){
				regs[rd_index]= arth_value;							//stroe arth_value
			}
		}else if(strcmp(funct3,"011")==0){ 							// ld
			if(rd_index != 0){
				regs[rd_index]=mem_value; 							// load memory data to register
			}
		}else if(strcmp(opcode,"1100111")==0&&strcmp(funct3,"000")==0){// jalr
			pc=regs[rd1_index];										// move progran counter
		}	
	}else if(type==5){ // jal
		
		if(rd_index!= 0){
			regs[rd_index]=pc;
		}
			pc+=imm_value-4;										// move progran counter
	}
}

int main(int ac, char* av[])
{
	if (ac < 3)
	{
		printf("./riscv_sim filename mode\n");
		return -1;
	}
	int k=0;
	char done = 0;
	if (init(av[1]) != 0)
		return -1;
	while (!done)
	{
		fetch();
		decode();
		exe();
		mem();
		wb();
		cycles++;    //increase clock cycle
	
		//if debug mode, print clock cycle, pc, reg 
		if (*av[2] == '0') {
			print_cycles();  //print clock cycles
			print_pc();		 //print pc
			print_reg();	 //print registers
		}
		// check the exit condition, do not delete!! 
		if (regs[9] == 10)  //if value in $t1 is 10, finish the simulation
			done = 1;
	}

	if (*av[2] == '1')
	{
		print_cycles();  //print clock cycles
		print_pc();		 //print pc
		print_reg();	 //print registers
	}

	
	return 0;
}


/* initialize all datapat elements
//fill the instruction and data memory
//reset the registers
*/
int init(char* filename)
{
	FILE* fp = fopen(filename, "r");
	int i;
	long inst;

	if (fp == NULL)
	{
		fprintf(stderr, "Error opening file.\n");
		return -1;
	}

	/* fill instruction memory */
	i = 0;
	while (fscanf(fp, "%lx", &inst) == 1)
	{
		inst_mem[i++] = inst;
	}


	/*reset the registers*/
	for (i = 0; i < 32; i++)
	{
		regs[i] = 0;
	}

	/*reset pc*/
	pc = 0;
	/*reset clock cycles*/
	cycles = 0;
	return 0;
}

void print_cycles()
{
	printf("---------------------------------------------------\n");

	printf("Clock cycles = %lld\n", cycles);
}

void print_pc()
{
	printf("PC	   = %ld\n\n", pc);
}

void print_reg()
{
	printf("x0   = %d\n", regs[0]);
	printf("x1   = %d\n", regs[1]);
	printf("x2   = %d\n", regs[2]);
	printf("x3   = %d\n", regs[3]);
	printf("x4   = %d\n", regs[4]);
	printf("x5   = %d\n", regs[5]);
	printf("x6   = %d\n", regs[6]);
	printf("x7   = %d\n", regs[7]);
	printf("x8   = %d\n", regs[8]);
	printf("x9   = %d\n", regs[9]);
	printf("x10  = %d\n", regs[10]);
	printf("x11  = %d\n", regs[11]);
	printf("x12  = %d\n", regs[12]);
	printf("x13  = %d\n", regs[13]);
	printf("x14  = %d\n", regs[14]);
	printf("x15  = %d\n", regs[15]);
	printf("x16  = %d\n", regs[16]);
	printf("x17  = %d\n", regs[17]);
	printf("x18  = %d\n", regs[18]);
	printf("x19  = %d\n", regs[19]);
	printf("x20  = %d\n", regs[20]);
	printf("x21  = %d\n", regs[21]);
	printf("x22  = %d\n", regs[22]);
	printf("x23  = %d\n", regs[23]);
	printf("x24  = %d\n", regs[24]);
	printf("x25  = %d\n", regs[25]);
	printf("x26  = %d\n", regs[26]);
	printf("x27  = %d\n", regs[27]);
	printf("x28  = %d\n", regs[28]);
	printf("x29  = %d\n", regs[29]);
	printf("x30  = %d\n", regs[30]);
	printf("x31  = %d\n", regs[31]);
	printf("\n");
}

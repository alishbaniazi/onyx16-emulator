#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
using namespace std;


//helper
int getlen(char* str) {
	int len = 0;
	while (str[len] != '\0') 
		len++;
	return len;
}


//================REGISTER BANK==================
class RegisterBank {
	unsigned short int r[8];
	unsigned short int PC;
	unsigned short int IR;
	unsigned char FLAGS;

public:
	//constructor
	RegisterBank() {
		for (int i = 0; i < 8; i++) {
			r[i] = 0x0000;
		}
		PC = 0x0000;
		IR = 0x0000;
		FLAGS = 0x00;
	}
	//read
	unsigned short readR(int ind) {
		return r[ind];
	}
	unsigned short readPC() {
		return PC;
	}
	unsigned short readIR() {
		return IR;
	}
	unsigned char readFLAGS() {
		return FLAGS;
	}


	//write
	void writeR(unsigned short int val, int ind) {
		r[ind] = val;
	}
	void writePC(unsigned short int val) {
		PC = val;
	}
	void writeIR(unsigned short int val) {
		IR = val;
	}
	void writeFLAGS(unsigned char val) {
		FLAGS= val;
	}

};


//=====================ARITHMETIC LOGIC UNIT=====================
class ALU {
public:
	unsigned short int operate(unsigned char opcode, unsigned short int op1, unsigned short int op2, unsigned char& flag) {
		unsigned short int res=0;

		if (opcode == 0x01) {
			 res = op1 + op2;
			
		}
		else if (opcode == 0x02) {
			res = op1 - op2;
			
		}
		else if (opcode == 0x03) {
			res = op1 * op2;
			
		}
		else if (opcode == 0x04) {
			if (op2 == 0) {
				res = 0x00;
			}
			else {
				res = op1 / op2;
			}
		}
		else if (opcode == 0x05) {
			res = op1 & op2;
			}
		else if (opcode == 0x06) {
			res = op1 | op2;
		}
		else if (opcode == 0x07) {
			res = op1 ^ op2;
		}
		else if (opcode == 0x08) {
			res = ~op1;
		}
		else if (opcode == 0x09) {
			res = op1 +1;
		}
		else if (opcode == 0x0B) {
			res = op1 -1;
		}
		else if (opcode == 0x0A) {
			if (op1 == op2) {
				flag |= 1;
				flag &= ~2;
				flag &= ~4;
			}
			else if (op1 < op2) {
				flag |= 2;
				flag &= ~1;
				flag &= ~4;
			}
			else if (op1 > op2) {
				flag &= ~1;
				flag &= ~2;
				flag |= 4;
			}
			return res;
		}

		if (res == 0x0000) {
			flag |= 1;
			flag &= ~0x02; 
			flag &= ~0x04;
		}
		else {
			flag &= ~1;
		}
		return res;
	}
};

//===================SYSTEM BUSES=====================
class bus {
public:
	unsigned short int address;
	unsigned char data;
	char control;          //r for read enable and w for write enable
	bus() {
		address = 0x0000;
		data = 0;
		control = '\0';
	}
};


//=========================MEMORY MODULE===========================
class MemoryModule {
	int capacity;
	unsigned char cell[3840];
public:
	MemoryModule() {
		capacity = 3840;
		for (int i = 0; i < capacity; i++) {
			cell[i] = 0x00;
		}
	}
	void loadRawBinary(unsigned short int address, unsigned short int word) {
		if (address + 1 < 3840) {
			cell[address] = (word >> 8) & 255;
			cell[address + 1] = word & 255;
		}
	}
	void memory_write(unsigned char val,unsigned short int address) {
		if (address < 0x00||address>=0x0F00) {
			cout << "[HARDWARE FAULT] Segmentation Fault: Write out of bounds " << endl;
		}
		else {
			cell[address] = val;
		}
	}
	unsigned char memory_read(unsigned short int address) {
		if (address < 0x00 || address >= 0x0F00) {
			cout << "[HARDWARE FAULT] Segmentation Fault: Read out of bounds " << endl;
			return 0xFF;
		}
		else {
			return cell[address];
		}
	}
};


class route {
public:
	// 0 memory
	// 1 keybboard char
	// 2 keyboard int
	// 3 graphics char
	// 4 graphics int
	// 5 fault zone
	// 6 invalid

	int decode(unsigned short int address, bool isWrite) {

		if (address <= 0x0EFF) {
			return 0;  
		}
		else if (address >= 0x0F00 && address <= 0x0FEF) {
			return 5;  
		}
		else if (address == 0x0FF0) {
			if (isWrite)
				return 6;
			else
				return 1;
		}
		else if (address == 0x0FF1) {
			if (isWrite)
				return 3;
			else
				return 6; 
		}
		else if (address == 0x0FF2) {
			if (isWrite)
				return 4;
			else
				return 6; 
		}
		else if (address == 0x0FF3) {
			if (isWrite)
				return 6;
			else
				return 2; 
		}
		else {
			return 6; 
		}
	}
};

class keyboard {
	//fifo
	char* buffer;
	int buff_size;
	int capacity;
public:
	keyboard() {
		capacity = 256;		
		buffer = new char[256];
		buff_size = 0;
	}
	void keyboard_write(char val) {
		
			//checking if space is available
			if (buff_size < capacity) {
				buffer[buff_size] = val;
				buff_size++;
			}
			else {
				char* temp = new char[capacity * 2];
				for (int i = 0; i < buff_size; i++) {
					temp[i] = buffer[i];
				}
				delete[] buffer;
				buffer = temp;
				temp = nullptr;
				buffer[buff_size] = val;
				buff_size++;
				capacity *= 2;
			}		
	}

	char keyboard_readchar() {
		// skip leading spaces first
		while (buff_size > 0 && buffer[0] == ' ') {
			for (int i = 0; i < buff_size - 1; i++)
				buffer[i] = buffer[i + 1];
			buff_size--;
		}
			if (buff_size != 0) {
				char ret = buffer[0];
				char* temp = new char[capacity];
				for (int i = 1; i < buff_size; i++) {
					temp[i - 1] = buffer[i];
				}
				delete[] buffer;
				buffer = temp;
				temp = nullptr;
				buff_size--;
				return ret;
			}
			else {
				cout << "[Hardware Interrupt] Awaiting Keyboard Input" << endl;
				// get input from user
				char input[256];
				cin >> input;
				for (int i = 0; input[i] != '\0'; i++) {
					keyboard_write(input[i]);
				}
				return keyboard_readchar();
			}
		
	}

	unsigned char keyboard_readInt() {
		//skipping till we find digit
		while (buff_size > 0 && (buffer[0] < '0' || buffer[0] > '9')) {
			for (int i = 0; i < buff_size - 1; i++)
				buffer[i] = buffer[i + 1];
			buff_size--;
		}

		//input if empty
		if (buff_size == 0) {
			cout << "[Hardware Interrupt] Awaiting Keyboard Input" << endl;
			char input[256];
			cin >> input;
			for (int i = 0; input[i] != '\0'; i++)
				keyboard_write(input[i]);
			keyboard_write(' ');
			return keyboard_readInt();
		}

		//finding digits
		unsigned char result = 0;
		while (buff_size > 0 && buffer[0] >= '0' && buffer[0] <= '9') {
			result = result * 10 + (buffer[0] - '0');
			for (int i = 0; i < buff_size - 1; i++)
				buffer[i] = buffer[i + 1];
			buff_size--;
		}
		return result;
	}

	~keyboard() {
		delete[] buffer;
	}
};

class phosphorusDisplay {
	char* display_buffer;                //accumulates char from graphicAdapter
	int buff_size;

public:

	phosphorusDisplay() {
		display_buffer = nullptr;
		buff_size = 0;
	}
	void populate(char* arr, int size) {

		char* temp = new char[size +buff_size];

		for (int i = 0; i < buff_size; i++) {
			temp[i] = display_buffer[i];
		}
		delete[] display_buffer;
		display_buffer = temp;
		temp = nullptr;

		for (int i = buff_size; i < buff_size+size; i++) {
			display_buffer[i] = arr[i-buff_size];
		}
		buff_size += size;
	}
	void render() {
		int ind = 0;
		int rows = 0;
		int col = 32;
		cout << "\033[32m";
		cout << "+--------------------------------+" << endl;
		cout << "|"<<"\033[0m"<<"   PHOSPHOR CRT DISPLAY RENDER " << "\033[32m" << " |" << endl;
		cout << "+--------------------------------+" << endl;
		
		while (ind < this->buff_size) {
		
			if (ind > 32 * 15) {
				cout << "+--------------------------------+" << endl;
				return;
			}
				
			cout << "|";
			for (int i = 0; i < 32; i++) {
				if (ind >= buff_size) {
					for (int j = 0; j < 32- i; j++)
						cout << " ";
					//cout << "|";
					//rows++;
					ind++;
					break;
				}
				if (display_buffer[ind] == '\n') {
					//fill rest of row with spaces then break to next row
					for (int j = i; j < 32; j++) cout << " ";
					ind++;
					break;
				}
				cout << display_buffer[ind++];
			
			}
				rows++;
			cout <<'|'<< endl;
			}
		while (rows++ < 16)
			cout << "|                                |" << endl;
		cout << "+--------------------------------+"<<"\033[0m" << endl;
	}

	~phosphorusDisplay() {
		delete[] display_buffer;
	}

};

class graphicAdapter {
	phosphorusDisplay* display;
public:
	graphicAdapter() {
		display = nullptr;
	}

	void setdisplay(phosphorusDisplay* p) {
		display = p;
	}

	void writeChar(unsigned char ch) {
		if (display == nullptr)
			return;
		char c = (char)ch;
		display->populate(&c,1);
	}
	void writeint(int val) {
		if (display == nullptr)
			return;
		if (val == 0) {
			char c = '0';
			display->populate(&c, 1);
			return;
		}
		int vallen = 0;
		int temp = val;
		while (temp != 0) {
			vallen++;
			temp /= 10;
		}
		char* ch=new char[vallen+1];
		temp = val;
		int i = 0;
		while (temp != 0&&i<vallen) {
			ch[vallen -1- i] = '0' + temp % 10;
			temp /= 10;
			i++;
		}
		ch[vallen] = '\0';

		display->populate(ch,getlen(ch));
		
		delete[] ch;
	}
};
class psu;
class Processor;
//====================MAIN BOARD====================
class mainBoard {
	Processor* processor;
	MemoryModule* RamSlot;
	graphicAdapter* ExpansionSlot;
	//power supply unit
	keyboard* Keyboard;
	phosphorusDisplay* display;
	psu* PSU;
	route decoder;
	bool powerOn;
	bus systemBus;
public:

	mainBoard() {
		RamSlot = nullptr;
		ExpansionSlot = nullptr;
		Keyboard = nullptr;
		powerOn = 1;
		PSU = nullptr;
	}

	void pulseClock();

	bus& getbus() {
		return systemBus;
	}
	MemoryModule* getmemory() {
		return RamSlot;
	}

	void addRAM(MemoryModule* m) {
		RamSlot = m;
	}
	void addKeyboard(keyboard* k) {
		Keyboard = k; 
	}
	void addgraphicAdapter(graphicAdapter* g) { 
		ExpansionSlot = g; 
	}
	void addDisplay(phosphorusDisplay* d) { 
		display = d; 
	}
	void killPower() {
		powerOn = false;
	}
	void poweron() {
		powerOn = 1;
	}
	void addPSU(psu* ps) {
		PSU = ps;
	}


};



//====================PSU======================

class psu {
public:
	float power_consumed;
	mainBoard* mboard;
	float wattageCapacity;

	psu(float total_capacity) {
		wattageCapacity = total_capacity;
		mboard = nullptr;
		power_consumed = 0;
	}
	void connectMainBoard(mainBoard* m) {
		mboard = m;
	}
	void is_overflow() {
		if (power_consumed > wattageCapacity) {
			cout << "[CRITICAL WARNING] Power Draw exceeded PSU Capacity" << endl;
			mboard->killPower();
		}
	}
	void add_power(float p) {
		power_consumed += p;
		is_overflow();

	}
	void powerReset() {
		power_consumed = 0;
	}
};

//==============pulseclock===============(mainboard)
void mainBoard::pulseClock() {
	if (!powerOn) {
		return;
	}

	if (PSU != 0) {

		PSU->powerReset();
		if (RamSlot != nullptr)
			PSU->add_power(0.1);      //idle powers
		if (ExpansionSlot != nullptr)
			PSU->add_power(2.0);
	}
	unsigned short int add = systemBus.address;
	unsigned char ctrl = systemBus.control;
	unsigned short int data = systemBus.data;
	char res = decoder.decode(add, systemBus.control == 'w');
	// 0 memory
// 1 keybboard char
// 2 keyboard int
// 3 graphics char
// 4 graphics int
// 5 fault zone
// 6 invalid
	bool null = false;
	if (res == 0) {
		if (RamSlot == nullptr) {

			return;
		}
		if (ctrl == 'r') {

			systemBus.data = RamSlot->memory_read(add);
		}

		else if (ctrl == 'w') {
			if (RamSlot == nullptr) {
				return;
			}
			RamSlot->memory_write(data, add);
		}
		if (PSU != nullptr)
			PSU->add_power(0.4);
	}


	else if (res == 1) {
		if (Keyboard == nullptr) {
			return;
		}
		systemBus.data = Keyboard->keyboard_readchar();
	}
	else if (res == 2) {
		if (Keyboard == nullptr) {
			return;
		}
		systemBus.data = Keyboard->keyboard_readInt();
	}
	else if (res == 3) {
		if (ExpansionSlot == nullptr) {
			return;
		}
		ExpansionSlot->writeChar(systemBus.data);
	}
	else if (res == 4) {
		if (ExpansionSlot == nullptr) {
			return;
		}
		ExpansionSlot->writeint(systemBus.data);
		if (PSU != nullptr)
			PSU->add_power(13);
	}
	else if (res == 5) {
		if (ctrl == 'r') {
			cout << "[HARDWARE FAULT] Segmentation Fault: Read out of bounds" << endl;
			systemBus.data = 0xFF;
		}
		else if (ctrl == 'w') {
			cout << "[HARDWARE FAULT] Segmentation Fault : Write out of bounds" << endl;
			return;
		}
	}
	else if (res == 6) {
		if (ctrl == 'r') {
			cout << "[MOTHERBOARD FAULT] Invalid Read Address" << endl;
			systemBus.data = 0x00;
		}
		else if (ctrl == 'w') {
			cout << "[MOTHERBOARD FAULT] Invalid Write Address" << endl;
			return;
		}
	}
	systemBus.control = '\0';
}


//====================THE PROCESSOR====================
struct singleVec {
	bool IS_VALID;
	bool USES_AEC;
	unsigned char AEC_OPERATION;
	bool IS_MEMORY_READ;
	bool IS_MEMORY_WRITE;
	bool IS_BRANCH;
	bool BRANCH_ON_ZERO;
	bool BRANCH_ON_NOT_ZERO;
	bool WRITES_TO_REGISTER;
	bool IS_4BIT_IMMEDIATE;
	bool IS_16BIT_IMMEDIATE;
};



class Processor {
	RegisterBank registers;
	ALU alu;
	mainBoard* mboard;
	unsigned char cache[16];
	unsigned short int baseAddr;
	bool valid;
	singleVec decodeMatrix[256];
	float temperature;


	void initDecodeMatrix() {
		//default
		for (int i = 0; i < 256; i++) {
			decodeMatrix[i] = { false,false,0x00,false,false,false,false,false,false,false,false };
		}

		//0x00 NOP valid but does nothing
		decodeMatrix[0x00].IS_VALID = true;

		//0x01 ADD
		decodeMatrix[0x01] = { true,true,0x01,false,false,false,false,false,true,false,false };

		//0x02 SUB
		decodeMatrix[0x02] = { true,true,0x02,false,false,false,false,false,true,false,false };

		//0x03 MUL
		decodeMatrix[0x03] = { true,true,0x03,false,false,false,false,false,true,false,false };

		//0x04 DIV
		decodeMatrix[0x04] = { true,true,0x04,false,false,false,false,false,true,false,false };

		//0x0A CMP
		decodeMatrix[0x0A] = { true,true,0x0A,false,false,false,false,false,false,false,false };

		//0x10 JMP — unconditional
		decodeMatrix[0x10] = { true,false,0x00,false,false,true,false,false,false,false,false };

		//0x11 JZ
		decodeMatrix[0x11] = { true,false,0x00,false,false,true,true,false,false,false,false };

		//0x12 JNZ
		decodeMatrix[0x12] = { true,false,0x00,false,false,true,false,true,false,false,false };

		//0x1A LDR_IMM format B
		decodeMatrix[0x1A] = { true,false,0x00,false,false,false,false,false,true,true,false };

		//0x1B LDR_IMM format D
		decodeMatrix[0x1B] = { true,false,0x00,false,false,false,false,false,true,false,true };

		//0x20 LDR
		decodeMatrix[0x20] = { true,false,0x00,true,false,false,false,false,true,false,false };

		//0x21 STR
		decodeMatrix[0x21] = { true,false,0x00,false,true,false,false,false,false,false,false };
	}

	



public:
	unsigned char fetchVal(unsigned short int add) {
		//cache hit
		if (valid && add >= baseAddr && add < baseAddr + 16) {
			return cache[add - baseAddr];
		}

		// cache miss
		unsigned short int alignedBase = (add/16)*16;
		for (int i = 0; i < 16; i++) {
			mboard->getbus().address = alignedBase + i;
			mboard->getbus().control = 'r';
			mboard->pulseClock();
			cache[i] = mboard->getbus().data;
		}
		baseAddr = alignedBase;
		valid = true;
		return cache[add - baseAddr];

	}

	//constructor
	Processor() {
		mboard = nullptr;
		temperature = 25.0f;
		valid = false;
		baseAddr = 0;
		for (int i = 0; i < 16; i++) cache[i] = 0;
		initDecodeMatrix();
	}
	void connectMboard(mainBoard* m) {
		mboard = m;
	}



	void FED_cycle() {
		//step1
		//halt check
		if (registers.readFLAGS() >> 7 & 1) {
			return;
		}

		//fetch
		unsigned short int a = fetchVal(registers.readPC());
		unsigned short int b = fetchVal(registers.readPC()+1);
		unsigned short int combined = (a<<8)|b;
		registers.writeIR(combined);
		if (combined == 0x0000) {
			registers.writeFLAGS(registers.readFLAGS() | 0x80);
			return;
		}


		//step2
		registers.writePC(registers.readPC() + 2);

		//step3
		//decode
		unsigned char opcode = (registers.readIR() >> 8) & 255;
		singleVec sv = decodeMatrix[opcode];
		unsigned char dest = (registers.readIR() >> 4) & 15;
		unsigned char src = registers.readIR() & 15;

		if (!sv.IS_VALID) {
			cout << "[HARDWARE FAULT] Invalid Instruction" << endl;
			return;
		}
		unsigned short int combined2=0;
		if (sv.IS_16BIT_IMMEDIATE) {
			unsigned short int a2 = fetchVal(registers.readPC());
			unsigned short int b2 = fetchVal(registers.readPC() + 1);
			combined2 = (a2 << 8) | b2;
			registers.writePC(registers.readPC() + 2);		
		}

		//step4
		//execute
		unsigned short int result = 0;
		unsigned short int A = registers.readR(dest);
		unsigned short int B = registers.readR(src);
		
		if (sv.USES_AEC) {
			unsigned char flags = registers.readFLAGS();
			result = alu.operate(sv.AEC_OPERATION, A, B, flags);
			registers.writeFLAGS(flags);
		}
		if (sv.IS_MEMORY_READ) {
			mboard->getbus().address = registers.readR(src);
			mboard->getbus().control = 'r';
			mboard->pulseClock();
			result = mboard->getbus().data;
		}
		if (sv.IS_MEMORY_WRITE) {
			unsigned short int writeAdd = registers.readR(src);
			mboard->getbus().address = writeAdd;
			mboard->getbus().data = registers.readR(dest);
			mboard->getbus().control = 'w';
			mboard->pulseClock();
			//invalidate cache if write hits cached block
			if (valid && writeAdd >= baseAddr && writeAdd < baseAddr + 16)
				valid = false;
		}

		if (sv.IS_BRANCH) {
			unsigned short int target = registers.readR(src);
			unsigned char zf = registers.readFLAGS() & 0x01;
			if (!sv.BRANCH_ON_ZERO && !sv.BRANCH_ON_NOT_ZERO)
				registers.writePC(target);
			else if (sv.BRANCH_ON_ZERO && zf == 1)
				registers.writePC(target);
			else if (sv.BRANCH_ON_NOT_ZERO && zf == 0)
				registers.writePC(target);
		}
		if (sv.IS_4BIT_IMMEDIATE)
			result = src; 

		if (sv.IS_16BIT_IMMEDIATE)
			result = combined2;


		//step5
		if (sv.WRITES_TO_REGISTER)
			registers.writeR(result, dest);

		//step6
		temperature += 0.05f;
		if (temperature > 90.0f) {
			registers.writeFLAGS(registers.readFLAGS() | 0x80);
			cout << "[CRITICAL ERROR] Thermal threshold exceeded" << endl;
		}

	}

	void run() {
		while (!(registers.readFLAGS() & 0x80)) {
			FED_cycle();
		}
	}

	void printState() {
		cout << "\033[33m"<<"========================================" << endl;
		cout << "        ONYX-16 SYSTEM STATE            " << endl;
		cout << "========================================" << endl;
		cout <<"\033[34m"<< "TEMPERATURE: " << temperature << " C" << endl;
		cout << "----------------------------------------" << endl;
		cout << "REGISTERS:" << endl;
		for (int i = 0; i < 8; i++) {
			cout << "  R" << i << " = " << registers.readR(i)
				<< " (0x" << hex << registers.readR(i) << dec << ")" << endl;
		}
		cout << "\033[0m";
	
	}
	
};

//========interpreter========

struct CompilationResult {
	std::vector<uint16_t> codeSegment;
	std::vector<uint16_t> dataSegment;
};

class Interpreter {
public:
	Interpreter() {}

	CompilationResult compile(const std::string& sourceCode);
	void loadProgramAndFlash(const std::string& filename, MemoryModule& ram);

private:
	std::map<std::string, uint16_t> labelMap;
	std::vector<uint16_t> parseLine(const std::string& line);
	uint8_t parseRegister(const std::string& regStr);

	std::vector<std::string> tokenize(const std::string& line);
};


//def interpreter


std::vector<std::string> Interpreter::tokenize(const std::string& line) {
	std::vector<std::string> tokens;
	std::stringstream ss(line);
	std::string token;

	while (ss >> token) {
		if (!token.empty() && token.back() == ',') {
			token.pop_back();
		}
		tokens.push_back(token);
	}
	return tokens;
}

uint8_t Interpreter::parseRegister(const std::string& regStr) {
	std::string cleanReg = regStr;
	if (cleanReg.front() == '[') cleanReg.erase(0, 1);
	if (cleanReg.back() == ']')  cleanReg.pop_back();

	if (cleanReg.size() >= 2 && cleanReg[0] == 'R') {
		int regNum = cleanReg[1] - '0';
		if (regNum >= 0 && regNum < 8) return static_cast<uint8_t>(regNum);
	}
	return 0;
}

std::vector<uint16_t> Interpreter::parseLine(const std::string& line) {
	size_t commentPos = line.find("//");
	std::string cleanLine = (commentPos != std::string::npos) ? line.substr(0, commentPos) : line;

	std::vector<std::string> tokens = tokenize(cleanLine);
	if (tokens.empty()) return { 0x0000 }; // NOP

	std::string mnemonic = tokens[0];
	std::vector<uint16_t> payload;

	if (mnemonic == "AARAM" || mnemonic == "NOP") {
		payload.push_back(0x0000);
	}
	else if (mnemonic == "JAMA" || mnemonic == "ADD") {
		uint8_t dest = parseRegister(tokens[1]);
		uint8_t src = parseRegister(tokens[2]);
		payload.push_back((0x01 << 8) | ((dest & 0x0F) << 4) | (src & 0x0F));
	}
	else if (mnemonic == "TAFREEK" || mnemonic == "SUB") {
		uint8_t dest = parseRegister(tokens[1]);
		uint8_t src = parseRegister(tokens[2]);
		payload.push_back((0x02 << 8) | ((dest & 0x0F) << 4) | (src & 0x0F));
	}
	else if (mnemonic == "ZARAB" || mnemonic == "MUL") {
		uint8_t dest = parseRegister(tokens[1]);
		uint8_t src = parseRegister(tokens[2]);
		payload.push_back((0x03 << 8) | ((dest & 0x0F) << 4) | (src & 0x0F));
	}
	else if (mnemonic == "TAQSEEM" || mnemonic == "DIV") {
		uint8_t dest = parseRegister(tokens[1]);
		uint8_t src = parseRegister(tokens[2]);
		payload.push_back((0x04 << 8) | ((dest & 0x0F) << 4) | (src & 0x0F));
	}
	else if (mnemonic == "MUWAZANA" || mnemonic == "CMP") {
		uint8_t dest = parseRegister(tokens[1]);
		uint8_t src = parseRegister(tokens[2]);
		payload.push_back((0x0A << 8) | ((dest & 0x0F) << 4) | (src & 0x0F));
	}
	else if (mnemonic == "CHHALANG" || mnemonic == "JMP") {
		uint8_t src = parseRegister(tokens[1]);
		payload.push_back((0x10 << 8) | (src & 0x0F));
	}
	else if (mnemonic == "AGAR_SIFAR" || mnemonic == "JZ") {
		uint8_t src = parseRegister(tokens[1]);
		payload.push_back((0x11 << 8) | (src & 0x0F));
	}
	else if (mnemonic == "AGAR_MAUJOOD" || mnemonic == "JNZ") {
		uint8_t src = parseRegister(tokens[1]);
		payload.push_back((0x12 << 8) | (src & 0x0F));
	}
	else if (mnemonic == "BHARO" || mnemonic == "DAALO" || mnemonic == "LDR_IMM") {
		uint8_t dest = parseRegister(tokens[1]);
		uint16_t immFull = 0;

		bool isNumber = isdigit(tokens[2][0]) || (tokens[2].size() > 1 && tokens[2][1] == 'x');
		if (isNumber) {
			try {
				immFull = std::stoi(tokens[2], nullptr, 0);
			}
			catch (...) {
				std::cout << "[COMPILER ERROR] Invalid immediate format: " << tokens[2] << std::endl;
			}
		}
		else {
			if (labelMap.find(tokens[2]) != labelMap.end()) {
				immFull = labelMap[tokens[2]];
			}
			else {
				std::cout << "[COMPILER ERROR] Unresolved Label: " << tokens[2] << std::endl;
			}
		}

		// If it's a Label, it was tracked as 4 bytes in Pass 1, so we must force Format D
		if (!isNumber || immFull > 0x0F) {
			// Requires 2-word 16-bit payload (Format D)
			payload.push_back((0x1B << 8) | ((dest & 0x0F) << 4)); // Header word
			payload.push_back(immFull);                            // Payload word
		}
		else {
			// Fits in 4 bits (Format B)
			payload.push_back((0x1A << 8) | ((dest & 0x0F) << 4) | (immFull & 0x0F));
		}
	}
	else if (mnemonic == "PARHO" || mnemonic == "LDR") {
		uint8_t dest = parseRegister(tokens[1]);
		uint8_t src = parseRegister(tokens[2]);
		payload.push_back((0x20 << 8) | ((dest & 0x0F) << 4) | (src & 0x0F));
	}
	else if (mnemonic == "RAKHO" || mnemonic == "STR") {
		uint8_t srcVal = parseRegister(tokens[1]);
		uint8_t ptrReg = parseRegister(tokens[2]);
		payload.push_back((0x21 << 8) | ((srcVal & 0x0F) << 4) | (ptrReg & 0x0F));
	}
	else {
		std::cout << "[COMPILER ERROR] Unrecognized Mnemonic: " << mnemonic << std::endl;
	}

	return payload;
}

CompilationResult Interpreter::compile(const std::string& sourceCode) {
	CompilationResult result;
	std::stringstream ss(sourceCode);
	std::string line;

	// storing pairs of {type, content}
	// type 0 = code, type 1 = data
	std::vector<std::pair<int, std::string>> processLines;

	labelMap.clear();
	uint16_t currentCodePC = 0;
	uint16_t currentDataPC = 0x0800;
	bool inDataSection = false;

	// Pass 1: PC Tracking and Label Mapping
	while (std::getline(ss, line)) {
		line.erase(0, line.find_first_not_of(" \t\r\n"));
		if (line.empty() || line.find("//") == 0) continue;

		size_t commentPos = line.find("//");
		std::string cleanLine = (commentPos != std::string::npos) ? line.substr(0, commentPos) : line;
		cleanLine.erase(cleanLine.find_last_not_of(" \t\r\n") + 1);

		if (cleanLine == ".MAWAAD" || cleanLine == ".DATA" || cleanLine == ".mawaad" || cleanLine == ".data") {
			inDataSection = true;
			continue;
		}
		if (cleanLine == ".HIDAYAT" || cleanLine == ".CODE" || cleanLine == ".hidayat" || cleanLine == ".code") {
			inDataSection = false;
			continue;
		}

		if (inDataSection) {
			size_t colonPos = cleanLine.find(':');
			if (colonPos != std::string::npos) {
				std::string labelName = cleanLine.substr(0, colonPos);
				labelName.erase(labelName.find_last_not_of(" \t") + 1);
				labelMap[labelName] = currentDataPC;

				std::string valueStr = cleanLine.substr(colonPos + 1);
				valueStr.erase(0, valueStr.find_first_not_of(" \t"));

				processLines.push_back({ 1, valueStr });

				if (!valueStr.empty() && valueStr.front() == '"') {
					// String literal
					int chars = 0;
					for (size_t i = 1; i < valueStr.size() && valueStr[i] != '"'; ++i) {
						if (valueStr[i] == '\\' && i + 1 < valueStr.size()) {
							i++; // escape sequence
						}
						chars++;
					}
					int words = (chars + 1) / 2;
					currentDataPC += words * 2;
				}
				else {
					// Integer array or single int
					int commas = std::count(valueStr.begin(), valueStr.end(), ',');
					int words = commas + 1;
					currentDataPC += words * 2;
				}
			}
		}
		else {
			if (cleanLine.back() == ':') {
				std::string labelName = cleanLine.substr(0, cleanLine.size() - 1);
				labelMap[labelName] = currentCodePC;
				continue; // Label takes 0 bytes
			}

			processLines.push_back({ 0, cleanLine });

			std::vector<std::string> tokens = tokenize(cleanLine);
			if (tokens.empty()) continue;

			// Calculate instruction byte size
			std::string mnemonic = tokens[0];
			if (mnemonic == "BHARO" || mnemonic == "DAALO" || mnemonic == "LDR_IMM") {
				if (tokens.size() > 2) {
					bool isNumber = isdigit(tokens[2][0]) || (tokens[2].size() > 1 && tokens[2][1] == 'x');
					if (isNumber) {
						try {
							uint16_t immFull = std::stoi(tokens[2], nullptr, 0);
							if (immFull <= 0x0F) currentCodePC += 2;
							else currentCodePC += 4; // Format D
						}
						catch (...) {
							currentCodePC += 4; // Fallback
						}
					}
					else {
						currentCodePC += 4; // Labels unconditionally 2 words
					}
				}
				else {
					currentCodePC += 2;
				}
			}
			else {
				currentCodePC += 2; // Fixed width defaults
			}
		}
	}

	// Pass 2: Instruction Generation
	for (const auto& item : processLines) {
		if (item.first == 1) { // DATA
			std::string valueStr = item.second;
			if (!valueStr.empty() && valueStr.front() == '"') {
				std::vector<uint8_t> bytes;
				for (size_t i = 1; i < valueStr.size() && valueStr[i] != '"'; ++i) {
					if (valueStr[i] == '\\' && i + 1 < valueStr.size()) {
						i++;
						if (valueStr[i] == '0') bytes.push_back(0);
						else if (valueStr[i] == 'n') bytes.push_back('\n');
						else bytes.push_back(valueStr[i]);
					}
					else {
						bytes.push_back(valueStr[i]);
					}
				}
				if (bytes.size() % 2 != 0) bytes.push_back(0); // Word alignment
				for (size_t i = 0; i < bytes.size(); i += 2) {
					result.dataSegment.push_back((bytes[i] << 8) | bytes[i + 1]);
				}
			}
			else {
				std::stringstream vss(valueStr);
				std::string token;
				while (std::getline(vss, token, ',')) {
					token.erase(0, token.find_first_not_of(" \t"));
					token.erase(token.find_last_not_of(" \t") + 1);
					try {
						uint16_t val = std::stoi(token, nullptr, 0);
						result.dataSegment.push_back(val);
					}
					catch (...) {
						result.dataSegment.push_back(0);
					}
				}
			}
		}
		else { // CODE
			std::vector<uint16_t> words = parseLine(item.second);
			for (uint16_t word : words) {
				result.codeSegment.push_back(word);
			}
		}
	}

	return result;
}

void Interpreter::loadProgramAndFlash(const std::string& filename, MemoryModule& ram) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cout << "[BIOS FAULT] Could not locate OS payload: " << filename << std::endl;
		return;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string targetOS = buffer.str();

	CompilationResult compiledOS = compile(targetOS);
	std::cout << "[BIOS] Compiled OS script into " << compiledOS.codeSegment.size() << " native 16-bit instruction words." << std::endl;

	std::cout << "[BOOTLOADER] Flashing Code Execution block to RAM Address 0x0000..." << std::endl;
	uint16_t codeAddress = 0x0000;
	for (uint16_t word : compiledOS.codeSegment) {
		ram.loadRawBinary(codeAddress, word);
		codeAddress += 2;
	}

	std::cout << "[BOOTLOADER] Flashing Data Variables (.MAWAAD) to RAM Address 0x0800..." << std::endl;
	uint16_t dataAddress = 0x0800;
	for (uint16_t word : compiledOS.dataSegment) {
		ram.loadRawBinary(dataAddress, word);
		dataAddress += 2;
	}

	std::cout << "[BOOTLOADER] Flash Complete." << std::endl;
	std::cout << "------------------------------------------------------------\n" << std::endl;
}

#include <iostream>
#include <bitset>
#include <iomanip>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <chrono>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <getopt.h>

using namespace std;

class VmClass {
public:
int registers[32] = {0};
int hi[1]={0};
int low[1]={0};
int vm_exec_slice_in_instructions;
string vm_binary;
string name;

}; 
//Load instruction
void li(int rs,int val,VmClass & obj){
    obj.registers[rs]=val;
}

//ADD instruction
void add(int rd,int rs, int rt,VmClass & obj) {
    obj.registers[rd] = obj.registers[rs] + obj.registers[rt];
}
//xor instruction
void xorop(int rd,int rs, int rt, VmClass & obj ){
    obj.registers[rd] = obj.registers[rs] ^ obj.registers[rt];
}
//SUB instruction
void sub( int rd,int rs, int rt, VmClass & obj ) {
    obj.registers[rd] = obj.registers[rs] - obj.registers[rt];
}

//ADDI instruction
void addi( int rt,int rs, int immediate, VmClass & obj ) {
    obj.registers[rt] = obj.registers[rs] + immediate;
}

//ADDU instruction
void addu(int rd,int rs, int rt, VmClass & obj ) {
    obj.registers[rd] = static_cast<unsigned int>(obj.registers[rs]) + static_cast<unsigned int>(obj.registers[rt]);
}

//SUBU instruction
void subu(int rd,int rs, int rt, VmClass & obj ) {
    obj.registers[rd] = static_cast<unsigned int>(obj.registers[rs]) - static_cast<unsigned int>(obj.registers[rt]);
}

//ADDIU instruction
void addiu(int rt,int rs, int immediate, VmClass & obj ) {
    obj.registers[rt] = static_cast<unsigned int>(obj.registers[rs]) + static_cast<unsigned int>(immediate);
}

//MUL instruction (without overflow)
void mul(int rd,int rs, int rt, VmClass & obj ) {
    int result = obj.registers[rs] * obj.registers[rt];
    obj.registers[rd] = result & 0xffffffff;
}

// MULT instruction
void mult(int rs, int rt, VmClass & obj ) {
    int32_t op1=obj.registers[rs];
    int32_t op2=obj.registers[rt];
    int64_t result = static_cast<int64_t>(op1) * static_cast<int64_t>(op2);
    obj.hi[0]= static_cast<int32_t>(result >> 32);
    obj.low[0]= static_cast<int32_t>(result & 0xffffffff);
    
}
// div instruction
void divi(int rs, int rt, VmClass & obj ) {
    if (obj.registers[rt] == 0) {
        std::cerr << "ERROR: Division by zero" << std::endl;
        exit(1);
    }
    obj.low[0]= obj.registers[rs] / obj.registers[rt]; 
    obj.hi[0]= obj.registers[rs] % obj.registers[rt]; 
}


// AND instruction
void AND(uint32_t rd, uint32_t rs, uint32_t rt, VmClass & obj ) {
    obj.registers[rd] = obj.registers[rs] & obj.registers[rt];
}

// OR instruction
void OR(uint32_t rd, uint32_t rs, uint32_t rt, VmClass & obj ) {
    obj.registers[rd] = obj.registers[rs] | obj.registers[rt];
}

// AND immediate instruction
void ANDI(uint32_t rt, uint32_t rs, uint32_t imm, VmClass & obj ) {
    obj.registers[rt] =obj.registers[rs] & imm;
}

// OR immediate instruction
void ORI(uint32_t rt, uint32_t rs, uint32_t imm, VmClass & obj ) {
    obj.registers[rt] =obj.registers[rs] | imm;
}

// Shift left logical instruction
void SLL(uint32_t rd, uint32_t rt,uint32_t imm, VmClass & obj ) {
    obj.registers[rd] =obj.registers[rt] << imm;
}

// Shift right logical instruction
void SRL(uint32_t rd, uint32_t rt,uint32_t imm, VmClass & obj ) {
    obj.registers[rd] =obj.registers[rt] >> imm;
}
//dumpstate instruction
void dumpstate(VmClass& obj){
    if(obj.name=="vm1"){
        cout<<"VM1"<<endl;
    }
    else{
        cout<<"VM2"<<endl;
    }
    
    for (int i = 0; i < 32; i++) {
        cout<<"R"+std::to_string(i)+"="<<obj.registers[i]<<endl;
        }
        cout << "hi=" << obj.hi[0] << endl;
        cout << "low=" << obj.low[0] << endl;
        cout << endl;
}

//Instruction Processing
int solve(string instructionsfileline, VmClass & obj ){
        std::stringstream ss(instructionsfileline);
        std::string opcode;
        std::getline(ss, opcode, ' ');
        if (opcode=="DUMP_PROCESSOR_STATE")
        { 
        dumpstate(obj);
        return 0;
               
        }
        if(opcode=="SNAPSHOT"){
            vector<string> tokens;
            size_t pos = 0, new_pos;
            while ((new_pos = instructionsfileline.find(' ', pos)) != string::npos) {
                tokens.push_back(instructionsfileline.substr(pos, new_pos - pos));
                pos = new_pos + 1;
            }
            tokens.push_back(instructionsfileline.substr(pos));
           ofstream outfile(tokens[1]);
            if (!outfile.is_open()) {
               cerr << "Error opening file " << tokens[1] << endl;
               }
               for (int i = 0; i < 32; i++) {
                outfile << "R" << i << "=" << obj.registers[i] << endl;
                }
            outfile << "hi=" << obj.hi[0] << endl;
            outfile << "low=" << obj.low[0] << endl;
            outfile.close();
            return 0;

        }
        
        std::string destRegister;
        std::getline(ss, destRegister, ',');
        std::size_t pos = destRegister.find("$");
        int destReg = std::stoi(destRegister.substr(pos + 1));
        
        std::string srcReg1;
        std::getline(ss, srcReg1, ',');
        std::string srcReg2;
        std::getline(ss, srcReg2);
        if(opcode=="li"){
            li(destReg,std::stoi(srcReg1),obj);

        }
        if(opcode=="add"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            pos = srcReg2.find("$");
            srcReg2 = srcReg2.substr(pos + 1);
            add(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
        }
        if(opcode=="addi"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            addi(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
        }
        if(opcode=="addu"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            pos = srcReg2.find("$");
            srcReg2 = srcReg2.substr(pos + 1);
            addu(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
        }
        if (opcode=="subu"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            pos = srcReg2.find("$");
            srcReg2 = srcReg2.substr(pos + 1);
            subu(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
        }
        if (opcode=="addiu"){ 
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            addiu(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
        }
        if (opcode=="mul"){ 
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            pos = srcReg2.find("$");
            srcReg2 = srcReg2.substr(pos + 1);
            mul(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
         }
         if (opcode=="mult"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            mult(destReg,std::stoi(srcReg1),obj);
         }
         if (opcode=="div"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            divi(destReg,std::stoi(srcReg1),obj);
         }
         if (opcode=="and"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            pos = srcReg2.find("$");
            srcReg2 = srcReg2.substr(pos + 1);
            AND(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
         }
         if (opcode=="or"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            if (srcReg2.find('$') != std::string::npos){
            pos = srcReg2.find("$");
            srcReg2 = srcReg2.substr(pos + 1);
            OR(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
            }
            else{
               ORI(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj); 
            }
         }
         if (opcode=="andi"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            ANDI(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
         }
         if (opcode=="sll"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            SLL(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
         }
         if (opcode=="srl"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            SRL(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
         }
         if (opcode=="sub"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            pos = srcReg2.find("$");
            srcReg2 = srcReg2.substr(pos + 1);
            sub(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
         }
         if (opcode=="xor"){
            pos = srcReg1.find("$");
            srcReg1 = srcReg1.substr(pos + 1);
            pos = srcReg2.find("$");
            srcReg2 = srcReg2.substr(pos + 1);
            xorop(destReg,std::stoi(srcReg1),std::stoi(srcReg2),obj);
         }
        
        return 0;
}
void load_Snapshot(string filename,VmClass & obj){
    ifstream infile(filename);

    if (!infile) {
        cerr << "Error: File not found." << endl;
    }
    string line;
    // Read each line of the file
    while (getline(infile, line)) {
        size_t pos;
        if ((pos = line.find("R")) != string::npos) {
            // Extract the register number and value
            int reg_num = stoi(line.substr(pos+1, line.find('=')-pos-1));
            int value = stoi(line.substr(line.find('=')+1));
            obj.registers[reg_num] = value;
        }
        else if (line.find("hi=") != string::npos) {
            obj.hi[0] = stoi(line.substr(line.find('=')+1));
        }
        else if (line.find("low=") != string::npos) {
            obj.low[0] = stoi(line.substr(line.find('=')+1));
        }
    }
}
    

int main(int argc, char* argv[]) {
    std::vector<std::string> assembly_Files;
    std::vector<std::string> snapshot_Files;
    
    int opt;
    int vm_num = 0;
    std::string vm_name, snapshot_name = "Nosnapshot";
    
    while ((opt = getopt(argc, argv, "v:s:")) != -1) {
        switch (opt) {
            case 'v':
                vm_num++;
                vm_name = optarg;
                assembly_Files.push_back(vm_name);
                if (vm_num > 1) {
                    snapshot_Files.push_back(snapshot_name);
                    snapshot_name = "Nosnapshot";
                }
                break;
            case 's':
                snapshot_name = optarg;
                break;
            default:
                std::cerr << "Unknown option: " << optopt << std::endl;
                
        }
    }
    snapshot_Files.push_back(snapshot_name);

    

   VmClass vm1;
   vm1.name="vm1";
   
   ifstream assemblyfile1("input_files_hw3/"+assembly_Files.at(0));
   if(snapshot_Files[0]!="Nosnapshot"){
    load_Snapshot(snapshot_Files[0],vm1);
   }
    if (!assemblyfile1.is_open()) {
        std::cerr << "Error: File open not Successful" << std::endl;
    }

    
    string assemblyfile1line;
    while (std::getline(assemblyfile1,assemblyfile1line)) {
        size_t idx = assemblyfile1line.find("=");
        if (assemblyfile1line.substr(0,idx) == "vm_exec_slice_in_instructions"){
            vm1.vm_exec_slice_in_instructions = std::stoi(assemblyfile1line.substr(idx+1));
        }else if (assemblyfile1line.substr(0,idx) == "vm_binary"){
            vm1.vm_binary = assemblyfile1line.substr(idx+1);
        }
    }

    ifstream assemblyfile2("input_files_hw3/"+assembly_Files.at(1)); 
    if (!assemblyfile2.is_open()) {
        std::cerr << "Error: File open not Successful" << std::endl;
    }

    VmClass vm2;
    vm2.name="vm2";

    string assemblyfile2line;
    if(snapshot_Files[1]!="Nosnapshot"){
    load_Snapshot(snapshot_Files[1],vm2);
   }
    
    while (std::getline(assemblyfile2,assemblyfile2line)) {
        size_t idx = assemblyfile2line.find("=");
        if (assemblyfile2line.substr(0,idx) == "vm_exec_slice_in_instructions"){
            vm2.vm_exec_slice_in_instructions = std::stoi(assemblyfile2line.substr(idx+1));
        }else if (assemblyfile2line.substr(0,idx) == "vm_binary"){
            vm2.vm_binary = assemblyfile2line.substr(idx+1);
        }
    }

    ifstream instructionsfile1("input_files_hw3/"+vm1.vm_binary);
     if (!instructionsfile1.is_open()) {
        std::cerr << "Error: File open not Successful" << std::endl;
    }
    ifstream instructionsfile2("input_files_hw3/"+vm2.vm_binary);
     if (!instructionsfile2.is_open()) {
        std::cerr << "Error: File open not Successful" << std::endl;
    }
    std::string line1;
    std::string line2;

    while (!instructionsfile1.eof() || !instructionsfile2.eof()) {
        // reading  x instructions from instructionsfile1
        for (int i = 0; i < vm1.vm_exec_slice_in_instructions && std::getline(instructionsfile1, line1); ++i) {
            // processing each instruction
            if(!line1.empty()){
                solve(line1,vm1);
            }
            
        }
        
        // read y instructions from instructionsfile2
        for (int j = 0; j < vm2.vm_exec_slice_in_instructions && std::getline(instructionsfile2, line2); ++j) {
            // processing each instruction
            if(!line2.empty()){
                solve(line2,vm2);
            }
        }
        
    }

    return 0;

}

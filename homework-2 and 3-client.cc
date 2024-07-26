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
#include <regex>
#include <unordered_map>
#include <winsock2.h>
#include <cstring>


#pragma comment(lib, "WS2_32.LIB")

using namespace std;

class VmClass {
public:
int registers[32] = {0};
int hi[1]={0};
int low[1]={0};
int vm_exec_slice_in_instructions;
string vm_binary;
string name;
int pc = 0;

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
    
    for (int i = 0; i < 32; i++) {
        cout<<"R"+std::to_string(i)+"="<<obj.registers[i]<<endl;
        }
        cout << "hi=" << obj.hi[0] << endl;
        cout << "low=" << obj.low[0] << endl;
        cout << endl;
}
void snapshot(string instructionsfileline, VmClass & obj){
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
}
//Instruction Processing
int solve(string instructionsfileline, VmClass & obj,string filename ){
        obj.pc+=1;
        std::stringstream ss(instructionsfileline);
        std::string opcode;
        std::getline(ss, opcode, ' ');
        if (opcode=="MIGRATE")
        { 
         cout<<"Before Migration"<<endl;
         dumpstate(obj);
        std::string command =instructionsfileline;
        std::regex ip_regex(R"(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})");
        std::smatch match;

    if (std::regex_search(command, match, ip_regex)) {
        std::string ip_address = match.str();
        std::cout << "Connecting to IP address: " << ip_address << std::endl;
         WSADATA wsaData;
         int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
         if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Create a socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Connect to a server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    result = connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result != 0) {
        std::cerr << "connect failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    const char* msg;
    
    // Store the current state of the registers and PC in a string
    std::stringstream ss;
    for (int i = 0; i < 32; i++) {
        ss << obj.registers[i] << " ";
    }
    ss << obj.pc << " ";
    ss<< filename<<" ";
    std::string temp = ss.str();
    msg = temp.c_str();
    result = send(sock, msg, strlen(msg), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }


    // Clean up
    closesocket(sock);
    WSACleanup();
    exit(0);

    return 0;

        
    } else {
        std::cout << "No IP address found." << std::endl;
    }
               
        }
        if (opcode=="DUMP_PROCESSOR_STATE")
        { 
        dumpstate(obj);
        return 0;
               
        }
        if(opcode=="SNAPSHOT"){
            snapshot(instructionsfileline,obj);
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

     ifstream instructionsfile1("input_files_hw3/"+vm1.vm_binary);
     if (!instructionsfile1.is_open()) {
        std::cerr << "Error: File open not Successful" << std::endl;
    }


    std::string line1;
    // reading  x instructions from instructionsfile1
        while(std::getline(instructionsfile1, line1)) {
            // processing each instruction
            if(!line1.empty()){
                solve(line1,vm1,vm1.vm_binary);
            }
            
        }
        

        
    

    return 0;

}

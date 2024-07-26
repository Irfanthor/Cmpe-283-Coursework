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
            //cout<<"2"<<endl;
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

int main() {
    // Initialize Winsock
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

    // Bind the socket to a port
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    result = bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result != 0) {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    result = listen(sock, 5);
    if (result != 0) {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Accept an incoming connection
    SOCKET clientSock = accept(sock, NULL, NULL);
    if (clientSock == INVALID_SOCKET) {
        std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Receive a message from the client
    char buffer[1024];
    result = recv(clientSock, buffer, sizeof(buffer), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSock);
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    VmClass vm;
    std::stringstream ss2(buffer);
    for (int i = 0; i < 32; i++) {
        ss2 >> vm.registers[i];
    }
    ss2 >>vm.pc;
    string filename;
    ss2>>filename;
    std::cout << "PC: " << vm.pc << std::endl;
    std::cout << "Registers: ";
    for (int i = 0; i < 32; i++) {
        std::cout << vm.registers[i] << " ";
    }
    std::cout << std::endl;
     ifstream instructionsfile1("input_files_hw3/"+filename);
     if (!instructionsfile1.is_open()) {
        std::cerr << "Error: File open not Successful" << std::endl;
    }


    std::string line;
    int num_lines_to_skip = vm.pc; // Change this to the number of lines you want to skip
    
    // Skip the first num_lines_to_skip lines in the file
    for (int i = 0; i < num_lines_to_skip; i++) {
        std::getline(instructionsfile1, line);
    }
    // reading  x instructions from instructionsfile1
        while(std::getline(instructionsfile1, line)) {
            // processing each instruction
            if(!line.empty()){
                solve(line,vm);
            }
            
        }
    buffer[result] = '\0';


    // Close the client socket
    closesocket(clientSock);

    // Close the server socket
    closesocket(sock);

    // Clean up
    WSACleanup();

    return 0;
}

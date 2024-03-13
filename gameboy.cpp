#include <iostream>
#include <fstream>
//#include <vector>
#include "Z80.h"
//include "screen.h"

// Global ROM array - Will be loaded from a file
unsigned char* rom = nullptr; // Use unsigned char* for binary data
int romSize = 0;

// Memory access functions
unsigned char memoryRead(int address) {
    if(address < romSize) {
        return rom[address];
    }
    return 0; // Return 0 for addresses beyond the ROM size
}

void memoryWrite(int address, unsigned char b) {
    // For this project, writing to ROM is not handled
}

int main() {
    // Load ROM from file
    std::ifstream romfile("testrom.gb", std::ios::in | std::ios::binary | std::ios::ate);
    if (!romfile.is_open()) {
        std::cerr << "Failed to open ROM file." << std::endl;
        return 1;
    }
    std::streampos size = romfile.tellg();
    rom = new unsigned char[size]; // Allocate memory for the ROM
    romSize = size;

    romfile.seekg(0, std::ios::beg);
    romfile.read(reinterpret_cast<char*>(rom), size); // Cast to char* for read function
    romfile.close();

    // Initialize the Z80 CPU with memory access functions
    Z80* z80 = new Z80(memoryRead, memoryWrite);

    // Reset the CPU
    z80->reset();

    // Execute instructions until the CPU halts
    while(!z80->halted) {
        z80->doInstruction();
        std::cout << "PC: " << z80->PC << ", A: " << (int)z80->A << ", B: " << (int)z80->B << std::endl;
    }

    // Output the final value in register A (expected: 21)
    std::cout << "Final A: " << (int)z80->A << std::endl;


    return 0;
}

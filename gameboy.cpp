#include <iostream>
#include <fstream>
#include <vector>
#include "Z80.h"
//#include "screen.h"
// Global ROM array - Initially hardcoded for testing
unsigned char rom[] = {0x06, 0x06, 0x3e, 0x00, 0x80, 0x05, 0xc2, 0x04, 0x00, 0x76};
int romSize = sizeof(rom) / sizeof(rom[0]);

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
    // Initialize the Z80 CPU with memory access functions
    Z80* z80 = new Z80(memoryRead, memoryWrite);

    // Reset the CPU and set the PC to the beginning of the ROM
    z80->reset();
    z80->PC = 0;

    // Execute instructions until the CPU halts
    while(!z80->halted) {
        z80->doInstruction();
        std::cout << "PC: " << z80->PC << ", A: " << (int)z80->A << ", B: " << (int)z80->B << std::endl;
    }

    // Output the final value in register A (expected: 21)
    std::cout << "Final A: " << (int)z80->A << std::endl;

   
    return 0;
}

#include <iostream>
#include <stdio.h>
#include <fstream>

#include "chip8.hpp"

unsigned char CHIP8::CHIP8_FONTSET[80] = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
unsigned short  CHIP8::CURRENT_OPCODE;
unsigned short  CHIP8::INDEX_REGISTER;
unsigned short  CHIP8::PROGRAM_COUNTER;
unsigned char   CHIP8::DELAY_TIMER;
unsigned char   CHIP8::SOUND_TIMER;
unsigned short  CHIP8::STACK_POINTER;
unsigned char   CHIP8::MEMORY[4096];
unsigned char   CHIP8::V[16];
unsigned char   CHIP8::PIXELS[64 * 32];
unsigned short  CHIP8::STACK[16];
unsigned char   CHIP8::KEYPAD[16];
bool            CHIP8::REDRAW_REQUIRED;

void CHIP8::EmulateCycle() {
    CURRENT_OPCODE = MEMORY[PROGRAM_COUNTER] << 8 | MEMORY[PROGRAM_COUNTER + 1];

    switch (CURRENT_OPCODE & 0xF000)
    {
        case 0x0000:
            switch (CURRENT_OPCODE & 0x000F)
            {
                case 0x0000:
                    // 0x00E0 Clear Screen
                    break;
                case 0x000E:
                    // 0x00EE Return from subroutine
                    break;
                default:
                printf("Unknown Opcode: 0x%04X\n", CURRENT_OPCODE);
            }
            break;
        
        case 0x1000:
            PROGRAM_COUNTER = CURRENT_OPCODE & 0x0FFF;
            break;
        
        case 0x2000:
            STACK[STACK_POINTER] = PROGRAM_COUNTER;
            STACK_POINTER++;
            PROGRAM_COUNTER = CURRENT_OPCODE & 0x0FFF;
            break;

        case 0x3000:
            if (V[(CURRENT_OPCODE & 0x0F00) >> 8] == (CURRENT_OPCODE & 0x00FF)) PROGRAM_COUNTER += 2;
            PROGRAM_COUNTER += 2;
            break;

        case 0x4000:
            if (V[(CURRENT_OPCODE & 0x0F00) >> 8] != (CURRENT_OPCODE & 0x00FF)) PROGRAM_COUNTER += 2;
            PROGRAM_COUNTER += 2;
            break;

        case 0x5000:
            if (V[(CURRENT_OPCODE & 0x0F00) >> 8] == V[(CURRENT_OPCODE & 0x00F0) >> 4]) PROGRAM_COUNTER += 2;
            PROGRAM_COUNTER += 2;
            break;

        case 0x6000:
            V[(CURRENT_OPCODE & 0x0F00) >> 8] = CURRENT_OPCODE & 0x0FF;
            PROGRAM_COUNTER += 2;
            break;
            
        default:
            printf("Unknown Opcode: 0x%04X\n", CURRENT_OPCODE);
    }

    if (PROGRAM_COUNTER >= 4096) PROGRAM_COUNTER = 0;
};

void CHIP8::Reset() {
    STACK_POINTER = 0;
    CURRENT_OPCODE = 0;
    INDEX_REGISTER = 0;
    PROGRAM_COUNTER = 0x200;
    STACK_POINTER = 0;
    DELAY_TIMER = 0;
    SOUND_TIMER = 0;
    REDRAW_REQUIRED = false;

    for (int i = 0; i < 4096; i++) {
        MEMORY[i] = 0x00;
    }

    for (int i = 0; i < 16; i++) {
        V[i] = 0x00;
    }

    for (int i = 0; i < 16; i++) {
        STACK[i] = 0;
    }

    for (int i = 0; i < 64 * 32; i++) {
        PIXELS[i] = 0;
    }

    for (int i = 0; i < 80; i++) {
        MEMORY[i] = CHIP8_FONTSET[i];
    }
}

bool CHIP8::LoadRom(char *file_path) {
    std::basic_ifstream<unsigned char> rom_file(file_path, std::ios::binary | std::ios::ate);

    if (!rom_file) {
        std::cout << "Could not open file " << file_path << std::endl;
        return false;
    }

    if (rom_file.is_open()) {
        std::cout << "Loading ROM" << std::endl;
        std::streampos size = rom_file.tellg();

        std::cout << "ROM byte size: " << int(size) << std::endl;
        unsigned char *rom_content = new unsigned char [size];

        rom_file.seekg(0, std::ios::beg);
        rom_file.read(rom_content, size);
        rom_file.close();

        for (int i = 0; i < int(size); i++) {
            MEMORY[i + 0x200] = rom_content[i];
        }
        return true;
    }
    return false;
}

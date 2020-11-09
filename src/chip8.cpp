#include <stdio.h>
#include <cstdlib>
#include <random>
#include <time.h>

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
    
    printf("Starting Emulation Cycle...\n");
    printf("Current PROGRAM_COUNTER: %d\n", PROGRAM_COUNTER);
    printf("Current OPCODE: 0x%04X\n", CURRENT_OPCODE);

    if (CURRENT_OPCODE == 0x0000) {
        PROGRAM_COUNTER += 2;
        return;
    }

    switch (CURRENT_OPCODE & 0xF000)
    {
        case 0x0000:
            switch (CURRENT_OPCODE & 0x00FF)
            {
                case 0x00E0: // Clears the screen
                    for (int i = 0; i < 64 * 32; i++) {
                        PIXELS[i] = 0;
                    }
                    PROGRAM_COUNTER += 2;
                    REDRAW_REQUIRED = true;
                    break;
                case 0x00EE: // Returns from a subroutine
                    STACK_POINTER--;
                    PROGRAM_COUNTER = STACK[STACK_POINTER];
                    PROGRAM_COUNTER += 2;
                    printf("PC from stack: 0x%04X\n", STACK[STACK_POINTER]);
                    break;
                default:
                    printf("Unknown Opcode: 0x%04X\n", CURRENT_OPCODE);
            }
            break;
        
        case 0x1000: // 0x1NNN Jumps to address NNN
            PROGRAM_COUNTER = CURRENT_OPCODE & 0x0FFF;
            break;
        
        case 0x2000: // 0x2NNN Calls subroutine at NNN
            STACK[STACK_POINTER] = PROGRAM_COUNTER;
            printf("PC put on stack: 0x%04X\n", PROGRAM_COUNTER);
            STACK_POINTER++;
            PROGRAM_COUNTER = CURRENT_OPCODE & 0x0FFF;
            break;

        case 0x3000: // 0x3XNN Skips the next instruction if VX equals NN
            if (V[(CURRENT_OPCODE & 0x0F00) >> 8] == (CURRENT_OPCODE & 0x00FF)) PROGRAM_COUNTER += 2;
            PROGRAM_COUNTER += 2;
            break;

        case 0x4000: // 0x4XNN Skips the next instruction if VX doesn't equal NN
            if (V[(CURRENT_OPCODE & 0x0F00) >> 8] != (CURRENT_OPCODE & 0x00FF)) PROGRAM_COUNTER += 2;
            PROGRAM_COUNTER += 2;
            break;

        case 0x5000: // 0x5XY0 Skips the next instruction if VX equals VY
            if (V[(CURRENT_OPCODE & 0x0F00) >> 8] == V[(CURRENT_OPCODE & 0x00F0) >> 4]) PROGRAM_COUNTER += 2;
            PROGRAM_COUNTER += 2;
            break;

        case 0x6000: // 0x6XNN Sets VX to NN
            V[(CURRENT_OPCODE & 0x0F00) >> 8] = CURRENT_OPCODE & 0x00FF;
            PROGRAM_COUNTER += 2;
            break;

        case 0x7000: // 7XNN Adds NN to VX
            V[(CURRENT_OPCODE & 0x0F00) >> 8] += CURRENT_OPCODE & 0x00FF;
            PROGRAM_COUNTER += 2;
            break;

        case 0x8000:
            switch (CURRENT_OPCODE & 0x000F) {
                case 0x0000: // 0x8XY0 Sets VX to the value of VY
                    V[(CURRENT_OPCODE & 0x0F00) >> 8] = V[(CURRENT_OPCODE & 0x00F0) >> 4];
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0001: // 0x8XY1 Sets VX to VX or VY
                    V[(CURRENT_OPCODE & 0x0F00) >> 8] |= V[(CURRENT_OPCODE & 0x00F0) >> 4];
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0002: // 0x8XY2 Sets VX to VX and VY
                    V[(CURRENT_OPCODE & 0x0F00) >> 8] &= V[(CURRENT_OPCODE & 0x00F0) >> 4];
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0003: // 0x8XY3 Sets VX to VX xor VY
                    V[(CURRENT_OPCODE & 0x0F00) >> 8] ^= V[(CURRENT_OPCODE & 0x00F0) >> 4];
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0004: // 0x8XY4 Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
                    V[(CURRENT_OPCODE & 0x0F00) >> 8] += V[(CURRENT_OPCODE & 0x00F0) >> 4];
                    if (V[(CURRENT_OPCODE & 0x00F0) >> 4] > (0xFF - V[(CURRENT_OPCODE & 0x0F00) >> 8])) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0005: // 0x8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
                    if (V[(CURRENT_OPCODE & 0x00F0) >> 4] > V[(CURRENT_OPCODE & 0x0F00) >> 8]) {
                        V[0xF] = 0;
                    }
                    else {
                        V[0xF] = 1;
                    }

                    V[(CURRENT_OPCODE & 0x0F00) >> 8] -= V[(CURRENT_OPCODE & 0x00F0) >> 4];
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0006: // 0x8XY6 Stores the least significant bit of VX in VF and then shifts VX to the right by 1
                    V[0xF] = V[(CURRENT_OPCODE & 0x0F00) >> 8] & 0x1;
                    V[(CURRENT_OPCODE & 0x0F00) >> 8] = V[(CURRENT_OPCODE & 0x0F00) >> 8] >> 0x0001;
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0007: // 0x8XY7 Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
                    if (V[(CURRENT_OPCODE & 0x0F00) >> 8] > V[(CURRENT_OPCODE & 0x00F0) >> 4]) {
                        V[0xF] = 0;
                    }
                    else {
                        V[0xF] = 1;
                    }
                    V[(CURRENT_OPCODE & 0x0F00) >> 8] = V[(CURRENT_OPCODE & 0x00F0) >> 4] - V[(CURRENT_OPCODE & 0x0F00) >> 8];
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x000E: // 0x8XYE Stores the most significant bit of VX in VF and then shifts VX to the left by 1
                    V[0xF] = V[(CURRENT_OPCODE & 0x0F00) >> 8] >> 7;
                    V[(CURRENT_OPCODE & 0x0F00) >> 8] <<= 1;
                    PROGRAM_COUNTER += 2;
                    break;

                default:
                    printf("Unknown Opcode: 0x%04X\n", CURRENT_OPCODE);
            }
            break;

        case 0x9000: // 0x9XY0 Skips the next instruction if VX doesn't equal VY
            if (V[(CURRENT_OPCODE & 0x0F00) >> 8] != V[(CURRENT_OPCODE & 0x00F0) >> 4]) PROGRAM_COUNTER += 2;
            PROGRAM_COUNTER += 2;
            break;

        case 0xA000: // 0xANNN Sets I to the address NNN
            INDEX_REGISTER = CURRENT_OPCODE & 0x0FFF;
            PROGRAM_COUNTER += 2;
            break;

        case 0xB000: // 0xBNNN Jumps to the address NNN plus V0
            PROGRAM_COUNTER = (CURRENT_OPCODE & 0x0FFF) + V[0];
            break;

        case 0xC000: // 0xCXNN Sets VX to the result of a bitwise and operation on a random number and NN
            V[(CURRENT_OPCODE & 0x0F00) >> 8] = ((rand() % (0xFF + 1)) & (CURRENT_OPCODE & 0x00FF));
            PROGRAM_COUNTER += 2;
            break;

        case 0xD000: // 0xDXYN Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels (See wikipedia entry for more info)
            {
                unsigned char x_coord = V[(CURRENT_OPCODE & 0x0F00) >> 8];
                unsigned char y_coord = V[(CURRENT_OPCODE & 0x00F0) >> 4];
                unsigned char height = CURRENT_OPCODE & 0x000F;

                V[0xF] = 0;
                for (int y = 0; y < height; y++) {
                    unsigned char pixel_row = MEMORY[INDEX_REGISTER + y];
                    for (int x = 0; x < 8; x++) {
                        if ((pixel_row & (0x80 >> x)) != 0) {
                            int pixel_position = x_coord + x + ((y_coord + y) * 64);
                            if (PIXELS[pixel_position] == 1) {
                                V[0xF] = 1;
                            }
                            PIXELS[pixel_position] ^= 1;
                        }
                    }
                }
            }
            
            REDRAW_REQUIRED = true;
            PROGRAM_COUNTER += 2;
            break;

        case 0xE000:
            switch (CURRENT_OPCODE & 0x00FF) {
                case 0x009E: // 0xEX9E Skips the next instruction if the key stored in VX is pressed
                    if (KEYPAD[V[(CURRENT_OPCODE & 0x0F00) >> 8]] != 0) PROGRAM_COUNTER += 2;
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x00A1: // 0xEXA1 Skips the next instruction if the key stored in VX isn't pressed
                    if (KEYPAD[V[(CURRENT_OPCODE & 0x0F00) >> 8]] == 0) PROGRAM_COUNTER += 2;
                    PROGRAM_COUNTER += 2;
                    break;

                default:
                    printf("Unknown Opcode: 0x%04X\n", CURRENT_OPCODE);
            }
            break;

        case 0xF000: // 
            switch (CURRENT_OPCODE & 0x00FF) {
                case 0x0007: // 0xFX07 Sets VX to the value of the delay timer
                    V[(CURRENT_OPCODE & 0x0F00) >> 8] = DELAY_TIMER;
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x000A: // 0xFX0A A key press is awaited, and then stored in VX
                    {
                        bool key_pressed = false;
                        for (int i = 0; i < 16; i++) {
                            if (KEYPAD[i] != 0) {
                                V[(CURRENT_OPCODE & 0x0F00) >> 8] = i;
                                key_pressed = true;
                            }
                        }

                        // If no key was pressed, do not increment PC
                        if (!key_pressed) return;
                    }

                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0015: // 0xFX15 Sets the delay timer to VX
                    DELAY_TIMER = V[(CURRENT_OPCODE & 0x0F00) >> 8];
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0018: // 0xFX18 Sets the sound timer to VX
                    SOUND_TIMER = V[(CURRENT_OPCODE & 0x0F00) >> 8];
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x001E: // 0xFX1E Adds VX to I. VF is not affected
                    INDEX_REGISTER += V[(CURRENT_OPCODE & 0x0F00) >> 8];
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0029: // 0xFX29 Sets I to the location of the sprite for the character in VX
                    INDEX_REGISTER = 0x5 * V[((CURRENT_OPCODE & 0x0F00) >> 8)];
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0033: // 0xFX33 (See wikipedia entry for more info)
                    MEMORY[INDEX_REGISTER]     =  V[(CURRENT_OPCODE & 0x0F00) >> 8] / 100;
                    MEMORY[INDEX_REGISTER + 1] = (V[(CURRENT_OPCODE & 0x0F00) >> 8] / 10) % 10;
                    MEMORY[INDEX_REGISTER + 2] = (V[(CURRENT_OPCODE & 0x0F00) >> 8] % 100) % 10;
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0055: // 0xFX55 Stores V0 to VX (including VX) in memory starting at address I
                    for (int i = 0; i <= ((CURRENT_OPCODE & 0x0F00) >> 8); i++) {
                        MEMORY[INDEX_REGISTER + i] = V[i];
                    }
                    PROGRAM_COUNTER += 2;
                    break;

                case 0x0065: // 0xFX65 Fills V0 to VX (including VX) with values from memory starting at address I
                    for (int i = 0; i <= ((CURRENT_OPCODE & 0x0F00) >> 8); i++) {
                        V[i] = MEMORY[INDEX_REGISTER + i];
                    }
                    PROGRAM_COUNTER += 2;
                    break;

                default:
                    printf("Unknown Opcode: 0x%04X\n", CURRENT_OPCODE);
            }
            break;
        default:
            printf("Unknown Opcode: 0x%04X\n", CURRENT_OPCODE);
    }
    
    if (SOUND_TIMER > 0) {
        SOUND_TIMER--;
        if (SOUND_TIMER == 0) {
            printf("\a");
            printf("BEEP!\n");
        }
    }
    if (DELAY_TIMER > 0) DELAY_TIMER--;
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

    srand(time(NULL));
}

bool CHIP8::LoadRom(char *file_path) {
    FILE* rom_file = fopen(file_path, "rb");
    if (rom_file == NULL) {
        printf("Failed to open ROM from path: %s\n", file_path);
        return false;
    }

    fseek(rom_file, 0, SEEK_END);
    long rom_size = ftell(rom_file);
    rewind(rom_file);

    unsigned char *rom_buffer = (unsigned char *) malloc(sizeof(unsigned char) * rom_size);
    if (rom_buffer == NULL) {
        printf("Failed to allocate memory for ROM!\n");
        return false;
    }

    long result = fread(rom_buffer, sizeof(unsigned char), (long)rom_size, rom_file);
    if (result != rom_size) {
        printf("Failed to read ROM\n");
        return false;
    }

    if ((4096 - 512) > rom_size) {
        for (int i = 0; i < rom_size; i++) {
            MEMORY[i + 512] = rom_buffer[i]; 
        }
    }
    else {
        printf("ROM too large to fit in memory\n");
        return false;
    }

    fclose(rom_file);
    free(rom_buffer);
    return true;
}

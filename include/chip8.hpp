#pragma once

namespace CHIP8 {
    extern unsigned char CHIP8_FONTSET[80];
    extern unsigned short CURRENT_OPCODE;
    extern unsigned short INDEX_REGISTER;
    extern unsigned short PROGRAM_COUNTER;
    extern unsigned char DELAY_TIMER;
    extern unsigned char SOUND_TIMER;
    extern unsigned short STACK_POINTER;

    // 4k memory
    extern unsigned char MEMORY[4096];
    // 15 one-bite registers and one carry flag
    extern unsigned char V[16];
    // 64x32 1D Array for the screen
    extern unsigned char PIXELS[64 * 32];
    // 32 byte stack
    extern unsigned short STACK[16];
    // State of the 16 Keys
    extern unsigned char KEYPAD[16];
    // True, if a redraw of the screen is required
    extern bool REDRAW_REQUIRED;

    void EmulateCycle();

    void Reset();

    bool LoadRom(char *file_path);
}

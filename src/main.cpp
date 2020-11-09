#include <iostream>
#include <fstream>

#include <Mantaray/Core/Window.hpp>
#include <Mantaray/Core/Logger.hpp>
#include <Mantaray/OpenGL/Drawables.hpp>
#include <Mantaray/OpenGL/ObjectLibrary.hpp>
#include <Mantaray/Core/InputManager.hpp>
#include <Mantaray/Core/KeyCodes.hpp>

#include "chip8.hpp"

using namespace MR;

unsigned int SCR_WIDTH = 512;
unsigned int SCR_HEIGHT = 256;
unsigned int SCR_RES_WIDTH = 64;
unsigned int SCR_RES_HEIGHT = 32;

void UpdateInput();

int main(int argc, char *argv[])
{
    Window* window = Window::CreateWindow(
        "Chip8", 
        Vector2u(SCR_WIDTH, SCR_HEIGHT), 
        Vector2u(SCR_WIDTH, SCR_HEIGHT), 
        Vector2f(SCR_RES_WIDTH, SCR_RES_HEIGHT)
    );
    Logger logger = Logger("Application");
    InputManager::AddKeyToWatch(MR_KEY_SPACE);

    VertexArray* default_vertex_array;
    ObjectLibrary::FindObject("DefaultVertexArray", default_vertex_array);
    Polygon pixel_polygon(default_vertex_array);

    CHIP8::Reset();

    if (argc == 2) {
        CHIP8::LoadRom(argv[1]);
    }

    float elapsed_time = 0.0f;
    float delta_time = 0.0f;
    bool halted = false;

    logger.Log("Starting Loop...");
    while (!window->getShouldClose())
    {
        delta_time = window->update();
        elapsed_time += delta_time;

        if (InputManager::GetKeyDown(MR_KEY_SPACE)) halted = !halted;

        if (!halted) {
            UpdateInput();
            CHIP8::EmulateCycle();
        }

        if (CHIP8::REDRAW_REQUIRED) {
            window->beginFrame();

            for (int y = 0; y < 32; y++) {
                for (int x = 0; x < 64; x++) {
                    if (CHIP8::PIXELS[x + y * 64] == 1) {
                        pixel_polygon.position = Vector2f(x, 32 - y - 1);
                        window->draw(pixel_polygon);
                    }
                }
            }
            CHIP8::REDRAW_REQUIRED = false;
            window->endFrame();
        }
    }

    logger.Log("Shutting down application...");
    delete window;
    return 0;
}

void UpdateInput() {
    CHIP8::KEYPAD[0] = InputManager::GetKey(MR_KEY_1) ? 1 : 0;
    CHIP8::KEYPAD[1] = InputManager::GetKey(MR_KEY_2) ? 1 : 0;
    CHIP8::KEYPAD[2] = InputManager::GetKey(MR_KEY_3) ? 1 : 0;
    CHIP8::KEYPAD[3] = InputManager::GetKey(MR_KEY_4) ? 1 : 0;
    
    CHIP8::KEYPAD[4] = InputManager::GetKey(MR_KEY_Q) ? 1 : 0;
    CHIP8::KEYPAD[5] = InputManager::GetKey(MR_KEY_W) ? 1 : 0;
    CHIP8::KEYPAD[6] = InputManager::GetKey(MR_KEY_E) ? 1 : 0;
    CHIP8::KEYPAD[7] = InputManager::GetKey(MR_KEY_R) ? 1 : 0;
    
    CHIP8::KEYPAD[8] = InputManager::GetKey(MR_KEY_A) ? 1 : 0;
    CHIP8::KEYPAD[9] = InputManager::GetKey(MR_KEY_S) ? 1 : 0;
    CHIP8::KEYPAD[10] = InputManager::GetKey(MR_KEY_D) ? 1 : 0;
    CHIP8::KEYPAD[11] = InputManager::GetKey(MR_KEY_F) ? 1 : 0;
    
    CHIP8::KEYPAD[12] = InputManager::GetKey(MR_KEY_Z) ? 1 : 0;
    CHIP8::KEYPAD[13] = InputManager::GetKey(MR_KEY_X) ? 1 : 0;
    CHIP8::KEYPAD[14] = InputManager::GetKey(MR_KEY_C) ? 1 : 0;
    CHIP8::KEYPAD[15] = InputManager::GetKey(MR_KEY_V) ? 1 : 0;
}

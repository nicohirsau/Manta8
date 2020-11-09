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

int main(int argc, char *argv[])
{
    Window* window = Window::CreateWindow(
        "Chip8", 
        Vector2u(SCR_WIDTH, SCR_HEIGHT), 
        Vector2u(SCR_WIDTH, SCR_HEIGHT), 
        Vector2f(SCR_RES_WIDTH, SCR_RES_HEIGHT)
    );
    Logger logger = Logger("Application");

    VertexArray* default_vertex_array;
    ObjectLibrary::FindObject("DefaultVertexArray", default_vertex_array);
    Polygon pixel_polygon(default_vertex_array);

    CHIP8::Reset();

    if (argc == 2) {
        CHIP8::LoadRom(argv[1]);
    }

    float elapsed_time = 0.0f;
    float delta_time = 0.0f;

    logger.Log("Starting Loop...");
    while (!window->getShouldClose())
    {
        delta_time = window->update();
        elapsed_time += delta_time;

        CHIP8::EmulateCycle();

        if (CHIP8::REDRAW_REQUIRED) {
            window->beginFrame();
            std::cout << "REDRAW" << std::endl;

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

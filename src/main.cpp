#include <iostream>

#include <Mantaray/Core/Window.hpp>
#include <Mantaray/Core/Logger.hpp>

using namespace MR;

unsigned int SCR_WIDTH = 512;
unsigned int SCR_HEIGHT = 256;
unsigned int SCR_RES_WIDTH = 64;
unsigned int SCR_RES_HEIGHT = 32;

int main()
{
    Window* window = Window::CreateWindow("OpenGL", Vector2u(SCR_WIDTH, SCR_HEIGHT), Vector2u(SCR_RES_WIDTH, SCR_RES_HEIGHT));
    Logger logger = Logger("Application");

    float elapsed_time = 0.0f;
    float delta_time = 0.0f;

    logger.Log("Starting Loop...");
    while (!window->getShouldClose())
    {
        delta_time = window->update();
        elapsed_time += delta_time;

        window->beginFrame();

        window->endFrame();
    }

    logger.Log("Shutting down application...");
    delete window;
    return 0;
}

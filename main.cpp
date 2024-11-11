#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include "WinUtil.h"
#include "Application.h"

//Screen dimension constants

int main(int argc, char* args[])
{
    Spar::Application app;
    app.Init();
    app.Run();
    return 0;
}
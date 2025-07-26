#include "headers/MyWindow.h"
#include "headers/Renderer.h"
#include <iostream>

using namespace std;

std::string textures[] = {
    "assest/bg1.png",
    "assest/paddle.png"
};

int main(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    MyWindow window(hInstance, 800, 600, L"Breakout Game");
    Renderer renderer;
    if (!window.Initialize()) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
        return -1;
    }

    window.Show(nCmdShow);
    if (!renderer.Initialize(window)) {
        MessageBoxW(nullptr, L"Failed to initialize renderer!", L"Error", MB_ICONERROR);
        return -1;
    }
    if (!renderer.LoadTexture("assets/paddle.png")) {
        MessageBoxW(nullptr, L"Failed to load bg1.png!", L"Error", MB_ICONERROR);
        return -1;
    }

    while (window.ProcessMessages()) {
        // Game update and render calls here

        renderer.Render();
        
    }

    return 0;
}
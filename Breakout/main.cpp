#include "headers/MyWindow.h"
#include "headers/Renderer.h"
#include "headers/InputManager.h"
#include <iostream>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;

// Assets to be preload
std::vector<std::string> textureList = {
    "assets/paddle.png",
    "assets/bg1.png",
};

int main(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    MyWindow window(hInstance, 800, 600, L"Breakout Game");
    Renderer renderer;
    InputManager inputManager;
    if (!window.Initialize()) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
        return -1;
    }

    window.Show(nCmdShow);
    if (!renderer.Initialize(window.GetHWND(), window.GetWidth(), window.GetHeight())) {
        MessageBoxW(nullptr, L"Failed to initialize renderer!", L"Error", MB_ICONERROR);
        return -1;
    }
    if (!renderer.LoadTexturesBatch(textureList)) {
        MessageBoxW(nullptr, L"Failed to load textures!", L"Error", MB_ICONERROR);
        return -1;
    }

    if (!inputManager.Initialize(hInstance, window.GetHWND())) {
        MessageBoxW(nullptr, L"Failed to initialize input manager!", L"Error", MB_ICONERROR);
        return -1;
    }

    while (window.ProcessMessages()) {
        // Game update and render calls here
        inputManager.Update();
        renderer.Render();
        
    }

    return 0;
}
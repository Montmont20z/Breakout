#include "MyWindow.cpp"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    MyWindow window(hInstance, 800, 600, L"Breakout Game");
    if (!window.Initialize()) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
        return -1;
    }

    window.Show(nCmdShow);
    while (window.ProcessMessages()) {
        // Game update and render calls here
        
    }

    return 0;
}
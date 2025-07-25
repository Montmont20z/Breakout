#pragma once
#include <Windows.h>
#include <string>

class MyWindow {
public:
	MyWindow(HINSTANCE hInstance, int width, int height, const std::wstring& title);
	~MyWindow();

	// Registers the window class and creates the window
	bool Initialize();
	// Shows the window
	void Show(int nCmdShow) const;   
	// Processes window messages; returns false when WM_QUIT is received; for Main Game Loop uses
	bool ProcessMessages() const;
	// Retrieves the handle to the created window
	HWND GetHWND() const;


private:
	// StaticWindowProc is to bridge between Win32 Callback and WindowProc
	static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_width;
	int m_height;
	std::wstring m_title;
	ATOM m_windowClass;


};
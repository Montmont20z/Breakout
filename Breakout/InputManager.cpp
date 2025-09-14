#include "headers/InputManager.h"
#include <iostream>

using namespace std;

InputManager::InputManager() : m_dInput(nullptr), m_dInputKeyboardDevice(nullptr), m_dInputMouseDevice(nullptr) {
    // Clean up previous buffer
    ZeroMemory(m_keysCurrent, sizeof(m_keysCurrent));
    ZeroMemory(m_keysPrev, sizeof(m_keysPrev));
}
InputManager::~InputManager() {
    //	Release keyboard device.
    if (m_dInputKeyboardDevice) {
        m_dInputKeyboardDevice->Unacquire();
        m_dInputKeyboardDevice->Release();
        m_dInputKeyboardDevice = NULL;
    }
    //	Release keyboard device.
    if (m_dInputMouseDevice) {
        m_dInputMouseDevice->Unacquire();
        m_dInputMouseDevice->Release();
        m_dInputMouseDevice = NULL;
    }
    //	Release DirectInput.
    if (m_dInput) {
        m_dInput->Release();
        m_dInput = NULL;
    }
}



bool InputManager::Initialize(HWND hWnd) {
    
    //	Create the Direct Input object.
    HRESULT hr = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_dInput, NULL);
    if (FAILED(hr)) {
        wchar_t msg[256];
        swprintf_s(msg, L"Error Creating Direct Input (HR: 0x%08X)", hr);
        MessageBox(nullptr, msg, L"Error", MB_ICONERROR | MB_OK);
        return false;
    }

    //	Create the keyboard device.
    hr = m_dInput->CreateDevice(GUID_SysKeyboard, &m_dInputKeyboardDevice, NULL);
    if (FAILED(hr)) {
        MessageBox(nullptr, L"Error Creating Keyboard Device", L"Error", MB_ICONERROR | MB_OK);
        return false;
    }
    //	Create the mouse device.
    hr = m_dInput->CreateDevice(GUID_SysMouse, &m_dInputMouseDevice, NULL);
    if (FAILED(hr)) {
        MessageBox(nullptr, L"Error Creating Mouse Device", L"Error", MB_ICONERROR | MB_OK);
        return false;
    }

    //	Set the input data format.
    m_dInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
    m_dInputMouseDevice->SetDataFormat(&c_dfDIMouse);
    //	Set the cooperative level.
    m_dInputKeyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    m_dInputMouseDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    //	Acquire the device.
    m_dInputKeyboardDevice->Acquire();
    m_dInputMouseDevice->Acquire();

    return true;
}

void InputManager::Update() {

    

    // copy current keys to prev keys
    memcpy(m_keysPrev, m_keysCurrent, sizeof(m_keysCurrent));
    ZeroMemory(m_keysCurrent, sizeof(m_keysCurrent)); // clean up current buffer
   
    if (!m_dInputKeyboardDevice) return;

    HRESULT hr = m_dInputKeyboardDevice->GetDeviceState(sizeof(m_keysCurrent), (LPVOID)m_keysCurrent); 
    if (FAILED(hr)) {
        // try to recover/reaquire device
        hr = m_dInputKeyboardDevice->Acquire();
        if (SUCCEEDED(hr)) {
            hr = m_dInputKeyboardDevice->GetDeviceState(sizeof(m_keysCurrent), (LPVOID)m_keysCurrent);
        }
        // if still failing, leave m_keysCurrent zeroed (no keys pressed)
        cout << "Failed to get keyboard device state" << endl;

        m_mousePrev = m_mouseCurrent;
        ZeroMemory(&m_mouseCurrent, sizeof(m_mouseCurrent));
        if (m_dInputMouseDevice) {
            HRESULT hr = m_dInputMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseCurrent);
            if (FAILED(hr)) {
                m_dInputMouseDevice->Acquire();
            }
            else {
                m_mouseX += m_mouseCurrent.lX;
                m_mouseY += m_mouseCurrent.lY;
                if (m_mouseX < 0) m_mouseX = 0;
                if (m_mouseY < 0) m_mouseY = 0;
                if (m_mouseX > 1000) m_mouseX = 1000; // clamp to screen
                if (m_mouseY > 600)  m_mouseY = 600;
            }
        }
    }
}

bool InputManager::IsKeyDown(unsigned char diKey) const {
    return (m_keysCurrent[diKey] & 0x80) != 0;
}

bool InputManager::IsKeyPressed(unsigned char diKey) const {
    return ((m_keysCurrent[diKey] & 0x80) != 0) && ((m_keysPrev[diKey] & 0x80) == 0);
}

bool InputManager::IsMouseDown(int button) const {
    if (button < 0 || button > 2) return false;
    return (m_mouseCurrent.rgbButtons[button] & 0x80) != 0;
}
bool InputManager::IsMousePressed(int button) const {
    if (button < 0 || button > 2) return false;
    return ((m_mouseCurrent.rgbButtons[button] & 0x80) != 0) &&
        ((m_mousePrev.rgbButtons[button] & 0x80) == 0);
}
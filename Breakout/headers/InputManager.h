#pragma once
#include <dinput.h>

class InputManager {
public:
	void Initialize();


private:
	// Input Variable
	//	Direct Input object.
	LPDIRECTINPUT8 m_dInput; 	
	//	Direct Input keyboard device.
	LPDIRECTINPUTDEVICE8  m_dInputKeyboardDevice; 
	//	Direct Input mouse device.
	LPDIRECTINPUTDEVICE8  m_dInputMouseDevice; 
	DIMOUSESTATE m_mouseState;




};
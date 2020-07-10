#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <array>
#include <dinput.h>
#include <Windows.h>

#include "KeyType.h"

class Keyboard
{
public:

	Keyboard(const HINSTANCE& hInstance, const HWND& hWnd);
	~Keyboard();

public:

	// キーボードの状態を更新
	void Update();

public:

	// キーボードのキーが押された瞬間か
	bool GetKeyDown(KeyType type) const;

	// キーボードのキーが押されているか
	bool GetKey(KeyType type) const;

	// キーボードのキーが離された瞬間か
	bool GetKeyUp(KeyType type) const;

private:

	LPDIRECTINPUTDEVICE8 m_inputDevice;
	LPDIRECTINPUT8 m_inputInterface;
	std::array<unsigned char, 256U> m_curKeys;
	std::array<unsigned char, 256U> m_preKeys;
};

#endif // !KEYBOARD_H_


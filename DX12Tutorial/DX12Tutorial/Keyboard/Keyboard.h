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

	// �L�[�{�[�h�̏�Ԃ��X�V
	void Update();

public:

	// �L�[�{�[�h�̃L�[�������ꂽ�u�Ԃ�
	bool GetKeyDown(KeyType type) const;

	// �L�[�{�[�h�̃L�[��������Ă��邩
	bool GetKey(KeyType type) const;

	// �L�[�{�[�h�̃L�[�������ꂽ�u�Ԃ�
	bool GetKeyUp(KeyType type) const;

private:

	LPDIRECTINPUTDEVICE8 m_inputDevice;
	LPDIRECTINPUT8 m_inputInterface;
	std::array<unsigned char, 256U> m_curKeys;
	std::array<unsigned char, 256U> m_preKeys;
};

#endif // !KEYBOARD_H_


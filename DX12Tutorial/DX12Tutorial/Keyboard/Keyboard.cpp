#include "Keyboard.h"

Keyboard::Keyboard(const HINSTANCE & hInstance, const HWND & hWnd)
{
	// インターフェースの作成
	DirectInput8Create(
		hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_inputInterface,
		nullptr);

	// デバイスの作成
	m_inputInterface->CreateDevice(
		GUID_SysKeyboard,
		&m_inputDevice,
		nullptr);

	// デバイスのフォーマットの設定
	m_inputDevice->SetDataFormat(&c_dfDIKeyboard);

	// 協調モードの設定
	m_inputDevice->SetCooperativeLevel(
		hWnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	// 制御開始
	m_inputDevice->Acquire();

	std::fill(m_curKeys.begin(), m_curKeys.end(), 0u);
	std::fill(m_preKeys.begin(), m_preKeys.end(), 0u);
}

Keyboard::~Keyboard()
{
	// 制御終了
	m_inputDevice->Unacquire();

	// デバイスの解放
	m_inputDevice->Release();

	// インターフェースの解放
	m_inputInterface->Release();
}

void Keyboard::Update()
{
	// 前回の状態を保持
	m_preKeys = m_curKeys;

	// 現在の状態を取得
	if (FAILED(m_inputDevice->GetDeviceState(256, m_curKeys.data())))
	{
		// 再び制御開始
		m_inputDevice->Acquire();

		// もう一度取得
		m_inputDevice->GetDeviceState(256, m_curKeys.data());
	}
}

bool Keyboard::GetKeyDown(KeyType type) const
{
	return (m_curKeys.at(static_cast<unsigned int>(type)) & 0x80) &
		~(m_preKeys.at(static_cast<unsigned int>(type)) & 0x80);
}

bool Keyboard::GetKey(KeyType type) const
{
	return m_curKeys.at(static_cast<unsigned int>(type)) & 0x80;
}

bool Keyboard::GetKeyUp(KeyType type) const
{
	return (m_preKeys.at(static_cast<unsigned int>(type)) & 0x80) &
		~(m_curKeys.at(static_cast<unsigned int>(type)) & 0x80);
}

#include "Keyboard.h"

Keyboard::Keyboard(const HINSTANCE & hInstance, const HWND & hWnd)
{
	// �C���^�[�t�F�[�X�̍쐬
	DirectInput8Create(
		hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_inputInterface,
		nullptr);

	// �f�o�C�X�̍쐬
	m_inputInterface->CreateDevice(
		GUID_SysKeyboard,
		&m_inputDevice,
		nullptr);

	// �f�o�C�X�̃t�H�[�}�b�g�̐ݒ�
	m_inputDevice->SetDataFormat(&c_dfDIKeyboard);

	// �������[�h�̐ݒ�
	m_inputDevice->SetCooperativeLevel(
		hWnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	// ����J�n
	m_inputDevice->Acquire();

	std::fill(m_curKeys.begin(), m_curKeys.end(), 0u);
	std::fill(m_preKeys.begin(), m_preKeys.end(), 0u);
}

Keyboard::~Keyboard()
{
	// ����I��
	m_inputDevice->Unacquire();

	// �f�o�C�X�̉��
	m_inputDevice->Release();

	// �C���^�[�t�F�[�X�̉��
	m_inputInterface->Release();
}

void Keyboard::Update()
{
	// �O��̏�Ԃ�ێ�
	m_preKeys = m_curKeys;

	// ���݂̏�Ԃ��擾
	if (FAILED(m_inputDevice->GetDeviceState(256, m_curKeys.data())))
	{
		// �Ăѐ���J�n
		m_inputDevice->Acquire();

		// ������x�擾
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

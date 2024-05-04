#include "..\Public\Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
{
}

HRESULT CInput_Device::Ready_Input_Device(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, nullptr)))
		return E_FAIL;

	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;

	//	DISCL_NONEXCLUSIVE : �������� ������ �ʰڴ�/ �ٸ� �����쿡�� �Է� ó���� �����ϵ��� �������
	//	Ű����� ���콺�� ������ DISCL_BACKGROUND, DISCL_NONEXCLUSIVE �� ����
	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	//	Ȱ���Ѵ�
	if (FAILED(m_pKeyboard->Acquire()))
		return E_FAIL;

	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInput_Device::SetUp_InputDeviceState()
{
	if (nullptr == m_pKeyboard ||
		nullptr == m_pMouse)
		return E_FAIL;

	//	ȣ���ϸ� �׻� �ʱ�ȭ�ϰ� �Է��ϴ� ���� �� ���� Ű�� �ε����� ������ ä����
	m_pKeyboard->GetDeviceState(256, m_byKeyState);

	/*
	typedef struct _DIMOUSESTATE {
	LONG    lX;					//	���콺�� �¿� ������
	LONG    lY;					//	���콺�� ���Ʒ� ������
	LONG    lZ;					//	���� ����°�
	BYTE    rgbButtons[4];
	} DIMOUSESTATE, *LPDIMOUSESTATE;
	*/
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);

	return S_OK;
}

void CInput_Device::Free()
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pInput);
}
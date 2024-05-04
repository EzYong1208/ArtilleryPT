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

	//	DISCL_NONEXCLUSIVE : 독점권을 가지지 않겠다/ 다른 윈도우에서 입력 처리가 가능하도록 만들어줌
	//	키보드와 마우스는 무조건 DISCL_BACKGROUND, DISCL_NONEXCLUSIVE 로 정의
	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	//	활성한다
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

	//	호출하면 항상 초기화하고 입력하는 순간 그 눌린 키의 인데스만 음수로 채워짐
	m_pKeyboard->GetDeviceState(256, m_byKeyState);

	/*
	typedef struct _DIMOUSESTATE {
	LONG    lX;					//	마우스의 좌우 움직임
	LONG    lY;					//	마우스의 위아래 움직임
	LONG    lZ;					//	휠을 땡기는거
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
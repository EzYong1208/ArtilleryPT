#include "..\Public\Key_Manager.h"

IMPLEMENT_SINGLETON(CKey_Manager)

CKey_Manager::CKey_Manager()
{
	ZeroMemory(&m_bState, sizeof(m_bState));
}

bool CKey_Manager::Key_Pressing(_int _key)
{
	if (GetAsyncKeyState(_key) & 0x8000)
		return true;

	return false;
}

bool CKey_Manager::Key_Down(_int _key)
{
	if ((GetAsyncKeyState(_key) & 0x8000) && (m_bState[_key] == false))
	{
		m_bState[_key] = true;
		return true;
	}

	if (((GetAsyncKeyState(_key) & 0x8000) == false) && m_bState[_key])
		m_bState[_key] = false;

	return false;
}

bool CKey_Manager::Key_Up(_int _key)
{
	if (((GetAsyncKeyState(_key) & 0x8000) == false) && m_bState[_key])
	{
		m_bState[_key] = false;
		return true;
	}

	if ((GetAsyncKeyState(_key) & 0x8000) && (m_bState[_key] == false))
		m_bState[_key] = true;

	return false;
}

void CKey_Manager::Free()
{
}
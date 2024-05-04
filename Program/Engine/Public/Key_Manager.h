#pragma once
#include "Base.h"

#define VK_MAX 0xFF

BEGIN(Engine)

class CKey_Manager final : public CBase
{
	DECLARE_SINGLETON(CKey_Manager)

private:
	CKey_Manager();
	virtual ~CKey_Manager() = default;

public:
	bool			Key_Pressing(_int _key);
	bool			Key_Down(_int _key);
	bool			Key_Up(_int _key);

public:
	virtual void	Free() override;

private:
	bool			m_bState[VK_MAX];
};

END
#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)

public:
	CLight_Manager() = default;
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC*				Get_LightDesc(_uint iIndex) const;

public:
	//	라이트를 만들어서 추가해야하니깐 장치받아오고, 몇번쨰 라이트를 겟할건지 _uint iIndex
	HRESULT							Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);

private:
	//	리스트 컨테이너로 빛 저장
	list<class CLight*>				m_Lights;
	typedef list<class CLight*>		LIGHTS;

public:
	virtual void					Free() override;
};

END
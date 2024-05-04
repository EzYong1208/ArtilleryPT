#pragma once

#include "Base.h"

//	Engine에 만든 Level 클래스를 부모 클래스로 만들어 Client에 Logo 등의 자식 클래스로 만들어 상속받아 사용
//	클라이언트에서 정의 할 레벨들의 부모가 되는 클래스

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase		//	abstract: 추상클래스화 했다(절대 독립적으로 동적할당 할 수 없음)
{
public:
	explicit CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel() = default;

public:
	virtual	HRESULT			NativeConstruct();
	virtual	_int			Tick(_double TimeDelta);
	virtual	_int			LateTick(_double TimeDelta);
	virtual	HRESULT			Render();

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

public:
	virtual	void			Free() override;
};

END
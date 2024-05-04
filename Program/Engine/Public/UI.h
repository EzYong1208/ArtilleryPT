#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI abstract : public CGameObject
{
protected:
	explicit CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT			NativeConstruct_Prototype();
	virtual HRESULT			NativeConstruct(void* pArg);
	virtual _int			Tick(_double TimeDelta);
	virtual _int			LateTick(_double TimeDelta);
	virtual HRESULT			Render();

public:
	virtual HRESULT			Save_Data(HANDLE hFile) = 0;
	virtual HRESULT			Load_Data(HANDLE hFile) = 0;

public:
	virtual CUI*			Clone(void* pArg) = 0;
	virtual void			Free() override;
};

END

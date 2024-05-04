#pragma once

#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT					NativeConstruct(const _tchar* pFontFilePath);
	HRESULT					Render(const _tchar* pText, _float2 vPosition, _fvector vColor);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	//	폰트를 그리기 위한 사각형 버퍼를 만들어줌
	SpriteBatch*			m_pBatch = nullptr;
	SpriteFont*				m_pFont = nullptr;

public:
	static CCustomFont*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFontFilePath);
	virtual void			Free() override;
};

END
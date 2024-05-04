#pragma once

#include "Base.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CComponent

class CRenderObject final : public CBase
{
private:
	CRenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CRenderObject(const CRenderObject& rhs);
	virtual ~CRenderObject() = default;

public:
	void SetTexture(string TextureKey);
	void SetTransform(_float4x4 Transform);
	void Render();

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	_float4x4	m_ProjMatrix;
	_float	m_fX, m_fY, m_fSizeX, m_fSizeY;

private:
	//	NativeConstruct() 에서 현재 객체에게 추가되어야 할 컴포넌트들을 복제(or 참조)하여 멤버변수에 보관하기 위한 함수
	HRESULT	SetUp_Components();
	//	자기의 멤버변수에 값을 채워줄 수 있는 함수
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	static CRenderObject* Create(string TextureKey, _float4x4 Transform);
	virtual void	Free() override;

};

END
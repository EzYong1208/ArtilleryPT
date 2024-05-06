#pragma once

#include "Base.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CComponent;
class CTransform;

class CRenderObject final : public CBase
{
//private:
//static Create말고 unique_ptr로 생성해 볼려고
public:
	CRenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CRenderObject(const CRenderObject& rhs) = delete;
	virtual ~CRenderObject() = default;

public:
	void SetTexture(string TextureKey);
	void SetTransform(_float4x4 Transform);
	HRESULT Render();

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform* m_pTransform = nullptr;

private:
	_float4x4	m_ProjMatrix;
	_float	m_fX, m_fY, m_fSizeX, m_fSizeY;

protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

private:
	HRESULT	NativeConstruct(string TextureKey, _float4x4 Transform);
	HRESULT	SetUp_Components();
	//	자기의 멤버변수에 값을 채워줄 수 있는 함수
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	static CRenderObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;

};

END
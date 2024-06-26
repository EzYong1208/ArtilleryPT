#pragma once

#include "Base.h"
#include "Renderer.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CComponent;
class CTransform;
class AGameObject;

class ENGINE_DLL CRenderObject final : public CBase
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
	void Add_RenderGroup(AGameObject* pRenderObject);
	void SetCoord_Size(_float fX, _float fY, _float fSizeX, _float fSizeY);
	void SetRenderType(CRenderer::RENDERGROUP eType);

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform* m_pTransform = nullptr;

private:
	_float4x4	m_ProjMatrix;
	_float	m_fX, m_fY, m_fSizeX, m_fSizeY;
	CRenderer::RENDERGROUP	m_RenderType = CRenderer::RENDER_END;

	map<const _tchar*, CComponent*> m_Components;
	typedef map<const _tchar*, CComponent*> COMPONENTS;

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
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
	//	NativeConstruct() ���� ���� ��ü���� �߰��Ǿ�� �� ������Ʈ���� ����(or ����)�Ͽ� ��������� �����ϱ� ���� �Լ�
	HRESULT	SetUp_Components();
	//	�ڱ��� ��������� ���� ä���� �� �ִ� �Լ�
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	static CRenderObject* Create(string TextureKey, _float4x4 Transform);
	virtual void	Free() override;

};

END
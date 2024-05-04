#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
public:
	typedef struct tagPassDesc
	{
		ID3DX11EffectPass*			pPass;
		ID3D11InputLayout*			pInputLayout;		//	해당패스에 대한 인풋 레이아웃도 같이 보관해줘야함
	} PASSDESC;

private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT				NativeConstruct_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT				NativeConstruct(void* pArg);

public:
	//	이 쉐이더로 그린다라고 해줘야함(버퍼를 그릴때마다)
	HRESULT						Apply(_uint iPassIndex);
	HRESULT						Set_InputLayout(_uint iPassIndex);
	HRESULT						Set_RawValue(const char* pValueName, const void* pData, _uint iLength);
	HRESULT						Set_Texture(const char* pValueName, ID3D11ShaderResourceView* pShaderResourceView);

private:
	ID3DX11Effect*				m_pEffect = nullptr;
	vector<PASSDESC*>			m_Passes;

public:
	static CShader*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent*			Clone(void* pArg = nullptr) override;
	virtual void				Free() override;

};

END
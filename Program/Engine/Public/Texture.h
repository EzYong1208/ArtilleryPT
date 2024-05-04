#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{

private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	//	텍스쳐클래스에서 쉐이더에 던져주기위한 함수, 몇번째 텍스쳐를 던질건지 _uint iTextureIndex
	HRESULT					SetUp_OnShader(class CShader* pShader, const char* pValueName, _uint iTextureIndex = 0);

public:
	virtual HRESULT			NativeConstruct_Prototype(const _tchar* pTexturePath, _uint iNumTextures);
	virtual HRESULT			NativeConstruct(void* pArg);

private:
	vector<ID3D11ShaderResourceView*>				m_Textures;
	typedef vector<ID3D11ShaderResourceView*>		TEXTURES;

public:
	static CTexture*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent*		Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END
#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_Textures(rhs.m_Textures)
{
	//	복제할때 얕은복사
	for (auto& pTexture : m_Textures)
		Safe_AddRef(pTexture);
}

HRESULT CTexture::SetUp_OnShader(CShader * pShader, const char* pValueName, _uint iTextureIndex)
{
	//	넣으려는 텍스처의 인덱스가 vector 사이즈보다 크면 E_FAIL
	if (nullptr == pShader ||
		iTextureIndex >= m_Textures.size())
		return E_FAIL;

	return pShader->Set_Texture(pValueName, m_Textures[iTextureIndex]);
}

HRESULT CTexture::NativeConstruct_Prototype(const _tchar * pTexturePath, _uint iNumTextures)
{
	//	서로 다른 쓰레드에서 호출될수 있는 함수
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	_tchar		szTextureFilePath[MAX_PATH] = TEXT("");
	_tchar		szExt[MAX_PATH] = TEXT("");

	m_Textures.reserve(iNumTextures);

	for (_uint i = 0; i < iNumTextures; ++i)
	{
		wsprintf(szTextureFilePath, pTexturePath, i);

		ID3D11ShaderResourceView*		pSRV = nullptr;

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		//	_wsplitpath_s(텍스처 경로,
		//				드라이브(안씀), 드라이브 경로가 필요없으면 nullptr
		//				드라이브경로 안줬으니깐 0
		//				폴더경로 안줌 nullptr
		//				아무튼 필요없음 0
		//				확장자를 받기위한 문자열 szExt
		//				문자열 길이)

		//---------------------------------------

		//	확장자를 비교만 해주면 된다
		if (!lstrcmp(szExt, TEXT(".dds")))
		{
			//	dds파일이면 CreateDDSTextureFromFile로 호출
			//	CreateDDSTextrueFromFile() << include 저거 추가했기때문에 사용 가능해짐
			if (FAILED(CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV)))
				return E_FAIL;
		}

		//	TK는 텍스쳐 콜라이더 렌더링, 폰트도 가능하지만 tga 파일 로드가 불가함
		else if (!lstrcmp(szExt, TEXT(".tga")))
			return E_FAIL;

		else
		{
			if (FAILED(CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV)))
				return E_FAIL;
		}

		m_Textures.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::NativeConstruct(void * pArg)
{
	return S_OK;
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CTexture*	pInstance = new CTexture(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSGBOX("Failed to Creating CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*	pInstance = new CTexture(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);

	m_Textures.clear();
}


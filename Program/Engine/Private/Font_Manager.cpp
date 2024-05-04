#include "..\Public\Font_Manager.h"
#include "CustomFont.h"

IMPLEMENT_SINGLETON(CFont_Manager)

CFont_Manager::CFont_Manager()
{
}

HRESULT CFont_Manager::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr != Find_CustomFont(pFontTag))
		return E_FAIL;

	CCustomFont*		pCustomFont = CCustomFont::Create(pDevice, pDeviceContext, pFontFilePath);
	if (nullptr == pCustomFont)
		return E_FAIL;

	m_Fonts.emplace(pFontTag, pCustomFont);

	return S_OK;
}

HRESULT CFont_Manager::Render_Font(ID3D11DeviceContext* pDeviceContext, const _tchar* pFontTag, const _tchar* pText, _float2 vPosition, _fvector vColor)
{
	CCustomFont*		pFont = Find_CustomFont(pFontTag);
	if (nullptr == pFont)
		return E_FAIL;

	pDeviceContext->GSSetShader(nullptr, nullptr, 0);

	return pFont->Render(pText, vPosition, vColor);
}

CCustomFont * CFont_Manager::Find_CustomFont(const _tchar * pFontTag)
{
	auto	iter = find_if(m_Fonts.begin(), m_Fonts.end(), CTagFinder(pFontTag));

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();
}

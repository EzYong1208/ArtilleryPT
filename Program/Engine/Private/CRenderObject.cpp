#include "CRenderObject.h"
#include "GameInstance.h"

void CRenderObject::SetTexture(string TagName)
{
}

void CRenderObject::SetTransform(_float4x4 Transform)
{
}

void CRenderObject::Render()
{
}

HRESULT CRenderObject::SetUp_Components()
{
	//	For.Com_Renderer
	if (FAILED(Add_Component(
		0,
		TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	//	For.Com_Shader
	if (FAILED(Add_Component(
		0,
		TEXT("Prototype_Component_Shader_VTXTEX"),
		TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	//	For.Com_VIBuffer
	if (FAILED(Add_Component(
		0,
		TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderObject::Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CComponent* pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);

	if (nullptr == pComponent)
		return E_FAIL;

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CRenderObject* CRenderObject::Create(string TextureKey, _float4x4 Transform)
{
	GameInstance clone ∫Œ≈π

		seting

	return nullptr;
}

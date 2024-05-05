#include "RenderObject.h"
#include "GameInstance.h"

CRenderObject::CRenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

void CRenderObject::SetTexture(string TagName)
{
	//Prototype_Component_Texture_Sky ../Bin/Resources/Textures/SkyBox/Sky004.dds
	//	For.Com_Texture
	//if (FAILED(__super::Add_Component(
	//	m_iCurrentLevel,
	//	TEXT("Prototype_Component_Texture_UI_Coin"),
	//	TEXT("Com_Texture"),
	//	(CComponent**)&m_pTextureCom)))
	//	return E_FAIL;
}

void CRenderObject::SetTransform(_float4x4 Transform)
{
	m_pTransform = CTransform::Create(m_pDevice, m_pDeviceContext);

	//m_pTransform->Set_State(CTransform::STATE_POSITION, Transform);
	m_pTransform->Set_WorldMatrix(Transform);
}

void CRenderObject::Render()
{
}

HRESULT CRenderObject::NativeConstruct(string TextureKey, _float4x4 Transform)
{
	SetUp_Components();

	SetTexture(TextureKey);
	SetTransform(Transform);

	return S_OK;
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

	//	For.Com_Transform
	if (FAILED(Add_Component(
		0,
		TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"),
		(CComponent**)&m_pTransform)))
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

CRenderObject* CRenderObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, string TextureKey, _float4x4 Transform)
{
	//GameInstance clone ºÎÅ¹

	//	setting
	CRenderObject* pInstance = new CRenderObject(pDevice, pDeviceContext);

	pInstance->NativeConstruct(TextureKey, Transform);

	return pInstance;
}

void CRenderObject::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransform);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}

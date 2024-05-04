#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_Passes(rhs.m_Passes)
{
	//	�н��� ������ŭ, ��ǲ���̾ƿ� ������ŭ ���۷��� ī��Ʈ ����
	//	��ǲ���̾ƿ��� ���۷��� ī��Ʈ�� ���� ������ �־ �����϶��� Shader Free���� �����Ǵ°ɷ� ��(���� ���縸 ���ѵΰ�)
	Safe_AddRef(m_pEffect);
}

HRESULT CShader::NativeConstruct_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;

#endif
	ID3DBlob*			pShaderBinaryCodes = nullptr;
	ID3DBlob*			pErrorMessage = nullptr;

	//	��ũ��Ŀ ���� �н���
	//	���͸� �־ �н��� ����  �н��� ������ ���� 0��° �н��� apply �Ұž� ��� �Ұ�

	//	���̴� ������ �������� �ؼ� �޸𸮿� �ø��� �۾�
	if (FAILED(D3DCompileFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, "fx_5_0", iFlag, 0, &pShaderBinaryCodes, &pErrorMessage)))
		return E_FAIL;

	//	D3DCompileFromFile(���̴� ���� �н�, 
	//						���̴� ��ũ�� ���ǵ� ��ü�� �ִ���,,����(nullptr), 
	//						D3D_COMPILE_STANDARD_FILE_INCLUDE << ���̴� ���� �ȿ��� �ٸ� ������ include �Ҽ��� ����, 11������ �� �ʼ������� �ʿ���,, �� ����α� ���� include �� ���̴������� �Բ� �������� �� �ְԲ� �������,
	//						������ �Լ�(���� ��Ʈ�� ����Ʈ�� �������� ������ nullptr),
	//						� ������ �ش��ϴ� ���̴� �����̳�("fx_5_0)" << (((Ŭ���̾�Ʈ - Bin - ShaderFiles-Shader_VtxTex.hlsl / Ŭ���̾�Ʈ - HLSL �����Ϸ� . ���̴� ���� / fx, ���̴� �� Shader Model 5.0(/ 5_0))))
	//						Flag �ϳ� ��Ƽ� ifdef _DEBUG, ����׸���϶��� iFlag �� ����(���̴� ����װ� ��������) ���̴� ����ȭ�� ������ ������
	//						�ι��� �÷��״� ���� �ʿ����(0)
	//						�޸� ��� ���� ���������� ä��� �ִ� ������ �ƴ� / �޸� ������ ����ٰŰ�, 2�� ������ �־ �����(pShaderBinaryCodes) ����ٰ�
	//						pErrorMessage << ���ڿ��� ����, ���� ����� ��(�ؿ� ������� ��))

	//-----------------------------------------

	//	�ܺο��� ������ ���̴� ������ �̸� �����ؼ� �޸𸮻� �÷��ΰ� �Ǹ� �� ����Ʈ �İ�ü�� ����� �ְ� ��
	if (FAILED(D3DX11CreateEffectFromMemory(pShaderBinaryCodes->GetBufferPointer(), pShaderBinaryCodes->GetBufferSize(), 0, m_pDevice, &m_pEffect)))
		return E_FAIL;

	//	D3DX11CreateEffectFromMemory(���� �ּҸ� �����ְ�, 
	//								�ּ��� ũ�⸦ �����ְ�,
	//								FXFlag >> �ǹ̾���(0),
	//								����̽���ü(m_pDevice),
	//								�Ҵ��� �༮�� ������ ����Ʈ ��ü(m_pEffect))
	//	�׷��� �Ǹ� ����Ʈ ��ü�� ������ ��(m_pEffect)

	//-----------------------------------------

	//	Shader Ŭ�������� Pass ��ü���� ��������
	//	GetTechniqueByIndex()�� pTechnique �� ���ü��� �ִ�
	ID3DX11EffectTechnique*			pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	//	D3DX11_TECHNIQUE_DESC ����ü ���� Pass ������ �������� �ʹ�, �� �н� ������ �ε��� �־ �� ����ŭ ���Բ�
	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));

	pTechnique->GetDesc(&TechniqueDesc);

	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		//	�� for�� �ȿ� ���ö����� ����ü�� ����������
		PASSDESC*		pPassDesc = new PASSDESC;
		ZeroMemory(pPassDesc, sizeof(PASSDESC));

		pPassDesc->pPass = pTechnique->GetPassByIndex(i);		//	��ũ��Ŀ�� �޾ƿͼ�
		if (nullptr == pPassDesc->pPass)
			return E_FAIL;

		D3DX11_PASS_DESC		PassDesc;
		ZeroMemory(&PassDesc, sizeof(D3DX11_PASS_DESC));

		pPassDesc->pPass->GetDesc(&PassDesc);										//	pPass���� ���� ���Թް�

		//	CreateInputLayout() <<< ������ �ʿ���
		//	�� �н��� ������ŭ ������ٰ�
		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pPassDesc->pInputLayout)))
			return E_FAIL;

		m_Passes.push_back(pPassDesc);
	}
	return S_OK;
}

HRESULT CShader::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CShader::Apply(_uint iPassIndex)
{
	if (iPassIndex >= m_Passes.size())
		return E_FAIL;

	return m_Passes[iPassIndex]->pPass->Apply(0, m_pDeviceContext);
}

HRESULT CShader::Set_InputLayout(_uint iPassIndex)
{
	if (iPassIndex >= m_Passes.size())
		return E_FAIL;

	m_pDeviceContext->IASetInputLayout(m_Passes[iPassIndex]->pInputLayout);

	return S_OK;
}

HRESULT CShader::Set_RawValue(const char* pValueName, const void* pData, _uint iLength)
{
	//	���������� ���� ������(�̸�����)
	ID3DX11EffectVariable*		pValue = m_pEffect->GetVariableByName(pValueName);
	if (nullptr == pValue)
		return E_FAIL;

	return pValue->SetRawValue(pData, 0, iLength);
	
	//	SetRawValue(��������� �����͸� �޾ƿͼ� ������, ��𼭺��ʹ�����(0 ����), ������(iLength)
}

HRESULT CShader::Set_Texture(const char * pValueName, ID3D11ShaderResourceView * pShaderResourceView)
{
	ID3DX11EffectVariable*		pValue = m_pEffect->GetVariableByName(pValueName);
	if (nullptr == pValue)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*	pShaderResourceVariable = pValue->AsShaderResource();
	if (nullptr == pShaderResourceVariable)
		return E_FAIL;

	return pShaderResourceVariable->SetResource(pShaderResourceView);
}

CShader * CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader*	pInstance = new CShader(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSGBOX("Failed to Creating CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CShader::Clone(void * pArg)
{
	CShader*	pInstance = new CShader(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		for (auto& pPassDesc : m_Passes)
		{
			Safe_Release(pPassDesc->pPass);
			Safe_Release(pPassDesc->pInputLayout);

			Safe_Delete(pPassDesc);
		}

		m_Passes.clear();
	}

	Safe_Release(m_pEffect);
}

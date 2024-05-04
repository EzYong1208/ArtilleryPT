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
	//	패스의 갯수만큼, 인풋레이아웃 갯수만큼 레퍼런스 카운트 증가
	//	인풋레이아웃이 레퍼런스 카운트에 대한 문제가 있어서 원본일때만 Shader Free에서 삭제되는걸로 함(얕은 복사만 시켜두고)
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

	//	테크니커 안의 패스들
	//	벡터를 넣어서 패스를 만들어서  패스를 여러개 만들어서 0번째 패스를 apply 할거야 라고 할거

	//	쉐이더 파일을 컴파일을 해서 메모리에 올리는 작업
	if (FAILED(D3DCompileFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, "fx_5_0", iFlag, 0, &pShaderBinaryCodes, &pErrorMessage)))
		return E_FAIL;

	//	D3DCompileFromFile(쉐이더 파일 패스, 
	//						쉐이더 매크로 정의된 객체가 있느냐,,없음(nullptr), 
	//						D3D_COMPILE_STANDARD_FILE_INCLUDE << 쉐이더 파일 안에서 다른 파일을 include 할수가 있음, 11에서는 좀 필수적으로 필요함,, 그 묶어두기 위한 include 한 쉐이더파일을 함께 컴파일할 수 있게끔 만들어줌,
	//						진입점 함수(따로 엔트리 포인트를 지정하지 않을거 nullptr),
	//						어떤 버전에 해당하는 쉐이더 파일이냐("fx_5_0)" << (((클라이언트 - Bin - ShaderFiles-Shader_VtxTex.hlsl / 클라이언트 - HLSL 컴파일러 . 쉐이더 형식 / fx, 쉐이더 모델 Shader Model 5.0(/ 5_0))))
	//						Flag 하나 잡아서 ifdef _DEBUG, 디버그모드일때는 iFlag 로 들어갈꺼(쉐이더 디버그가 가능해짐) 쉐이더 최적화를 무조건 꺼야함
	//						두번쨰 플래그는 따로 필요없음(0)
	//						메모리 덩어리 내가 인위적으로 채울수 있는 영역이 아님 / 메모리 영역에 담아줄거고, 2중 포인터 넣어서 여기다(pShaderBinaryCodes) 담아줄거
	//						pErrorMessage << 문자열을 담음, 쓸모가 없기는 함(밑에 출력으로 뜸))

	//-----------------------------------------

	//	외부에서 가져온 쉐이더 파일을 미리 번역해서 메모리상에 올려두게 되면 그 이펙트 컴객체를 만들어 주게 됨
	if (FAILED(D3DX11CreateEffectFromMemory(pShaderBinaryCodes->GetBufferPointer(), pShaderBinaryCodes->GetBufferSize(), 0, m_pDevice, &m_pEffect)))
		return E_FAIL;

	//	D3DX11CreateEffectFromMemory(실제 주소를 던져주고, 
	//								주소의 크기를 던져주고,
	//								FXFlag >> 의미없다(0),
	//								디바이스객체(m_pDevice),
	//								할당한 녀석을 저장할 이펙트 객체(m_pEffect))
	//	그렇게 되면 이펙트 객체가 생성이 됨(m_pEffect)

	//-----------------------------------------

	//	Shader 클래스에서 Pass 객체들을 만들어야함
	//	GetTechniqueByIndex()로 pTechnique 를 얻어올수가 있다
	ID3DX11EffectTechnique*			pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	//	D3DX11_TECHNIQUE_DESC 구조체 내의 Pass 갯수를 가져오고 싶다, 그 패스 갯수를 인덱스 넣어서 그 수만큼 돌게끔
	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));

	pTechnique->GetDesc(&TechniqueDesc);

	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		//	이 for문 안에 들어올때마다 구조체를 만들어줘야함
		PASSDESC*		pPassDesc = new PASSDESC;
		ZeroMemory(pPassDesc, sizeof(PASSDESC));

		pPassDesc->pPass = pTechnique->GetPassByIndex(i);		//	테크니커를 받아와서
		if (nullptr == pPassDesc->pPass)
			return E_FAIL;

		D3DX11_PASS_DESC		PassDesc;
		ZeroMemory(&PassDesc, sizeof(D3DX11_PASS_DESC));

		pPassDesc->pPass->GetDesc(&PassDesc);										//	pPass에다 값을 대입받고

		//	CreateInputLayout() <<< 무조건 필요함
		//	각 패스의 갯수만큼 만들어줄거
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
	//	전역변수의 값을 가져옴(이름으로)
	ID3DX11EffectVariable*		pValue = m_pEffect->GetVariableByName(pValueName);
	if (nullptr == pValue)
		return E_FAIL;

	return pValue->SetRawValue(pData, 0, iLength);
	
	//	SetRawValue(던지고싶은 데이터를 받아와서 던진다, 어디서부터던지냐(0 부터), 사이즈(iLength)
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

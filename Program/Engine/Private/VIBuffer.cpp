#include "..\Public\VIBuffer.h"
#include "Shader.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_VBDesc(rhs.m_VBDesc)
	, m_VBSubResourceData(rhs.m_VBSubResourceData)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
	, m_IBDesc(rhs.m_IBDesc)
	, m_IBSubResourceData(rhs.m_IBSubResourceData)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_iNumIndicesPerPrimitive(rhs.m_iNumIndicesPerPrimitive)
	, m_eIndexFormat(rhs.m_eIndexFormat)
	, m_eTopology(rhs.m_eTopology)
	, m_pVerticesPos(rhs.m_pVerticesPos)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Render(CShader* pShader, _uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*	pVertexBuffers[] = {
		m_pVB
	};

	_uint			iStrides[] = {
		m_VBDesc.StructureByteStride
	};

	_uint			iOffsets[] = {
		0,
	};

	//	여러개의 버텍스버퍼를 세팅할 수 있다
	//	장치는 여러개의 버텍스 버퍼를 바인딩 할 수 있는데 몇번째 슬롯부터 채울껀지(당연히 0부터)
	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);		//	트라이앵글 리스트(TRIANGLELIST) 채워주던거

	if (FAILED(pShader->Set_InputLayout(iPassIndex)))
		return E_FAIL;

	//	Apply()는 그리기 직전에 해라
	//	Apply 이후에 쉐이더 전역변수 값을 던지면 제대로 commit이 안됨
	if (FAILED(pShader->Apply(iPassIndex)))
		return E_FAIL;

	m_pDeviceContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	return m_pDevice->CreateBuffer(&m_VBDesc, &m_VBSubResourceData, &m_pVB);
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	return m_pDevice->CreateBuffer(&m_IBDesc, &m_IBSubResourceData, &m_pIB);
}

void CVIBuffer::Free()
{
	__super::Free();

	//	위치벡터는 안지우고 원본 지울때 삭제하려니깐 CVIBuffer Free()에서 isCloned가 flase일떄 m_pVerticesPos 를 지운다
	if (false == m_isCloned)
		Safe_Delete_Array(m_pVerticesPos);

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
}

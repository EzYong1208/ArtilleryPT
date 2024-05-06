#include "..\Public\VIBuffer_Terrain.h"
#include "Transform.h"
#include "Frustum.h"
#include "QuadTree.h"
#include "PipeLine.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_pIndices(rhs.m_pIndices)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	Safe_AddRef(m_pQuadTree);
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(const _tchar* pHeightMap)
{

#pragma region VERTEX_BUFFER

	//	파일 입출력을 통해서 정점의 위치를 세팅
	HANDLE		hFile = CreateFile(pHeightMap, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong					dwByte = 0;
	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	_ulong*		pPixel = new _ulong[m_iNumVerticesX * m_iNumVerticesZ];
	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iNumVertexBuffers = 1;

	m_VBDesc.ByteWidth = sizeof(VTXNORTEX) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXNORTEX);

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3((float)j, (float)(pPixel[iIndex] & 0x000000ff) / 10.f, (float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}

	//	직접 값을 채워주고 대입
	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	//	값 채우고나면 동적할당한거 필요가 없어지니 삭제
	Safe_Delete_Array(pPixel);
	CloseHandle(hFile);

#pragma endregion


#pragma region INDEX_BUFFER
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	//m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.Usage = D3D11_USAGE_DYNAMIC;			//	인덱스 버퍼만 동적으로
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	FACEINDICES32*	pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	//	미리 할당해 두기
	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFace = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[1];
			pIndices[iNumFace]._2 = iIndices[2];

			//	iNumFace 증가시키기 전에 3개 정점의 정보를 가져와서
			//	정점마다 끼고 있는 면이 서로 다르다(어떤 점이 면을 여러개를 가지고 있을수도 있다)
			//	평균을 구해서 노멀벡터를 구하는게 맞지만 방향벡터이기 떄문에 길이가 필요하지 않다

			_vector		vSour, vDest, vNormal;
			//	2번째 정점에서 1번쨰 정점을 빼서 vSour 구함
			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._0]);
			//	마지막 정점에서 2번쨰 정점을 빼서 vDest 구하고
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]);
			//	vSour와 vDest를 외적에서 vNormal을 구함
			//	XMVector3Cross를 통해서 외적한다음에 XMVector3Normalize로 노멀라이즈해서 vNormal
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			//	꼭 노멀라이즈해서 저장해주는 작업을 해야함
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNormal));
			++iNumFace;

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[2];
			pIndices[iNumFace]._2 = iIndices[3];

			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._0]);
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNormal));
			++iNumFace;
		}
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX, m_iNumVerticesZ);
	if (nullptr == m_pQuadTree)
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct(void * pArg)
{
	return S_OK;
}

void CVIBuffer_Terrain::Culling(CTransform * pTransform)
{
	CFrustum*		pFrustum = GET_INSTANCE(CFrustum);
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	//	로컬상의 평면을 구하는 작업
	if (FAILED(pFrustum->Transform_ToLocalSpace(pTransform)))
		return;

	_uint		iNumFaces = 0;

	//	XMVector3TransformCoord(pPipeLine->Get_CamPosition()) : 카메라 위치를 월드행렬의 역행렬을 곱해서 로컬위치로 바꿈
	//	쿼드트리를 통해서 지형컬링
	m_pQuadTree->Culling(pFrustum, m_pVerticesPos, m_pIndices, &iNumFaces, XMVector3TransformCoord(pPipeLine->Get_CamPosition(), pTransform->Get_WorldMatrixInverse()));
	
	//for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	//{
	//	for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
	//	{
	//		_uint		iIndex = i * m_iNumVerticesX + j;

	//		_uint		iIndices[4] = {
	//			iIndex + m_iNumVerticesX,
	//			iIndex + m_iNumVerticesX + 1,
	//			iIndex + 1,
	//			iIndex
	//		};

	//		_bool		isIn[4] = {
	//			//	m_pVerticesPos(정점의 위치)의 iIndices의 0번째께 절두체 안에 있는지 확인해야함
	//			//	판단하고자 하는 점의 위치(XMLoadFloat3(&m_pVerticesPos[iIndices[0]])), fRange(디폴트 0)
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[0]])),
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[1]])),
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[2]])),
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[3]]))
	//		};

	//		//	오른쪽 위 삼각형(3개 중의 하나가 들어와 있으면)
	//		//	우선은 체크할려고 &&로 해둠, 원래는 ||
	//		if (true == isIn[0] &&
	//			true == isIn[1] &&
	//			true == isIn[2])
	//		{
	//			//	인덱스를 다시 쌓아주는 작업을 함
	//			//	루프마다 무조건 추가하는게 아니라서 iNumFaces 하나 지역변수 추가해서 m_pIndices[iNumFaces] 다 채우고 ++iNumFaces
	//			m_pIndices[iNumFaces]._0 = iIndices[0];
	//			m_pIndices[iNumFaces]._1 = iIndices[1];
	//			m_pIndices[iNumFaces]._2 = iIndices[2];
	//			++iNumFaces;
	//		}

	//		//	왼쪽 하단 삼각형
	//		if (true == isIn[0] &&
	//			true == isIn[2] &&
	//			true == isIn[3])
	//		{
	//			m_pIndices[iNumFaces]._0 = iIndices[0];
	//			m_pIndices[iNumFaces]._1 = iIndices[2];
	//			m_pIndices[iNumFaces]._2 = iIndices[3];
	//			++iNumFaces;
	//		}
	//	}
	//}

	RELEASE_INSTANCE(CPipeLine);
	RELEASE_INSTANCE(CFrustum);

	//	기존 인덱스 버퍼 교체
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//	어떤 특정 버퍼를 Lock 한다(Map) 이 함수는 모든 메모리 영역에 대해서 Map 한다
	//	Map(누구를 락 할건지(m_pVB), 추가적으로 할당하는 녀석들이 있을수 있음(혹시 여러개를 공유할수 있음, 버텍스버퍼는 상관없어서 0),
	//	MapType를 어떤 방식으로 Map UnMap 할거냐(*밑에 주석참고), MapFlag(필요없음, 0),
	//	SubResource(구조체의 주소를 주면 구조체 안에 있는 pData 보이드 포인터, 버텍스버퍼의 공간의 주소를 줌))
	m_pDeviceContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	//	SubResource.pData 에 완전히 새로운 정보를 받아야함, 그래서 memcpy로 복사받음
	memcpy(SubResource.pData, m_pIndices, sizeof(FACEINDICES32) * iNumFaces);

	m_pDeviceContext->Unmap(m_pIB, 0);

	m_iNumPrimitive = iNumFaces;

	/*
	typedef
	enum D3D11_MAP
	{
	D3D11_MAP_READ	= 1,
	D3D11_MAP_WRITE	= 2,				//	거의 사용되지 않음(퍼포먼스 구림)
	D3D11_MAP_READ_WRITE = 3,
	D3D11_MAP_WRITE_DISCARD	= 4,		//	맵을 걸때 기존에 값은 다 날라간 정보들을 삭제함
	D3D11_MAP_WRITE_NO_OVERWRITE = 5	//	기존값을 그대로 얻어옴(살아있다, 특정 값만 교체하고 싶을때)
	} 	D3D11_MAP;
	*/
	//	어지간하면 D3D11_MAP_WRITE_DISCARD가 좋은데 기존값을 유지해야겟다 하면 D3D11_MAP_WRITE_NO_OVERWRITE
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pHeightMap)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pHeightMap)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pQuadTree);

	if (false == m_isCloned)
		Safe_Delete_Array(m_pIndices);
}

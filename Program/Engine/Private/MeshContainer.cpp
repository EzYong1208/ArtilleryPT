#include "..\Public\MeshContainer.h"
#include "HierarchyNode.h"

CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs)
{

}

void CMeshContainer::Set_Check(_bool bChoose)
{
	m_bChoose = bChoose;
}

HRESULT CMeshContainer::NativeConstruct_Prototype(aiMesh * pAIMesh)
{
	m_pAIMesh = pAIMesh;
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumBones = pAIMesh->mNumBones;

	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::Ready_VertexIndexBuffer(CModel::MODELTYPE eMeshtype, _fmatrix TransformMatrix)
{
#pragma region VERTICES
	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertices = m_pAIMesh->mNumVertices;
	m_iNumVertexBuffers = 1;

	HRESULT			hr = 0;

	if (CModel::TYPE_NONANIM == eMeshtype)
		hr = Ready_NonAnimMeshContainer(m_pAIMesh, TransformMatrix);
	else
		hr = Ready_AnimMeshContainer(m_pAIMesh);

#pragma endregion


#pragma region INDICES
	m_iNumPrimitive = m_pAIMesh->mNumFaces;		//	그려야할 면의 주소
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._0 = m_pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = m_pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = m_pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CMeshContainer::Add_HierarchyNode(_uint iNodeIndex)
{
	m_HierarchyNodeIndices.push_back(iNodeIndex);

	return S_OK;
}

HRESULT CMeshContainer::SetUp_Matrices(_float4x4 * pBoneMatrices, const vector<CHierarchyNode*>* pNodes, _fmatrix TransformMatrix)
{
	//	메쉬컨테이너 들어올때마다 SetUp_Matrices()로 행렬의 주소를 전달(BoneMatrices)
	_uint	iIndex = 0;

	//	같은 이름의 하이어라키노드가 없다면 pBoneMatrices 걍 찾음
	if (0 == m_iNumBones)
	{
		auto	iter = find_if(pNodes->begin(), pNodes->end(), [&](CHierarchyNode* pHierarchyNode)
		{
			return !strcmp(m_szName, pHierarchyNode->Get_Name());
		});

		if (iter == pNodes->end())
			XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());

		//	아니면(찾았다면) 밑에 for문을 돌지 않음, 그래서 밑의 작업을 위에서 똑같이 해줘야함
		else
		{
			//	//	하이어라키노드에 들어가있었던 OffsetMatrix, CombinedTransformationMatrix를 가져옴
			_matrix		OffsetMatrix = (*iter)->Get_OffsetMatrix();
			_matrix		CombinedTransformationMatrix = (*iter)->Get_CombinedTransformationMatrix();

			_matrix		BoneMatrix = OffsetMatrix * CombinedTransformationMatrix * TransformMatrix;

			//	최종 그리기 위한 행렬을 만듬
			//	어차피 쉐이더에 던질때 또 해야해서 바로 XMMatrixTranspose(BoneMatrix)
			XMStoreFloat4x4(&pBoneMatrices[iIndex++], XMMatrixTranspose(BoneMatrix));
		}
	}

	for (auto& iNodeIndex : m_HierarchyNodeIndices)
	{
		//	하이어라키노드에 들어가있었던 OffsetMatrix, CombinedTransformationMatrix를 가져옴
		_matrix		OffsetMatrix = (*pNodes)[iNodeIndex]->Get_OffsetMatrix();
		_matrix		CombinedTransformationMatrix = (*pNodes)[iNodeIndex]->Get_CombinedTransformationMatrix();

		_matrix		BoneMatrix = OffsetMatrix * CombinedTransformationMatrix * TransformMatrix;

		//	최종 그리기 위한 행렬을 만듬
		//	어차피 쉐이더에 던질때 또 해야해서 바로 XMMatrixTranspose(BoneMatrix)
		XMStoreFloat4x4(&pBoneMatrices[iIndex++], XMMatrixTranspose(BoneMatrix));
	}

	return S_OK;
}

HRESULT CMeshContainer::Ready_NonAnimMeshContainer(aiMesh* pAIMesh, _fmatrix TransformMatrix)
{
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = sizeof(VTXMODEL) * m_iNumVertices;
	m_VBDesc.StructureByteStride = sizeof(VTXMODEL);
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	VTXMODEL*	pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		//	정점에 포지션 벡터랑 TransformMatrix를 곱하자(XMVector3TransformCoord)
		//	이렇게 나온 결과값을 저장 XMStoreFloat3를 통해 pVertices[i].vPosition에 저장
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), TransformMatrix));

		//	노말도 똑같이 XMVector3TransformNormal을 통해 TransformMatrix를 곱해서 저장
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), TransformMatrix));

		//	모주건 0번쨰 좌표를 읽고 있음(이게 문제가 있을수도 있음)
		//	통 맵을 깔았는데 타일 같은게 이상하게 나온다던가 하는 경우가 있음
		//	만약에 UV좌표가 0번째가 아닌 1번째부터 값이 있으면 그걸 다 앞으로 끌어오면 왠만한 문제는 해결된다고 함
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	//	정점정보를 그대로 들고 있는건 무거우니깐 삭제
	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Ready_AnimMeshContainer(aiMesh* pAIMesh)
{
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = sizeof(VTXANIMMODEL) * m_iNumVertices;
	m_VBDesc.StructureByteStride = sizeof(VTXANIMMODEL);
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	VTXANIMMODEL*	pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	if (FAILED(Ready_SkinnedInfo(pVertices)))
		return E_FAIL;

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	//	정점정보를 그대로 들고 있는건 무거우니깐 삭제
	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Ready_SkinnedInfo(VTXANIMMODEL * pVertices)
{
	if (nullptr == m_pAIMesh)
		return E_FAIL;

	m_iNumBones = m_pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*			pAIBone = m_pAIMesh->mBones[i];

		//	본 안에 웨이트가 있고 그 웨이트의 갯수만큼 루프를 돔
		//	mNumWeights : 이 뼈가 영향을 주고 있는 정점들의 갯수
		for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
		{

			//	블렌드웨이트가 만약에 0이라면 값이 안 채워진거, 웨이트가 0인 경우는 존재할 수 없음
			//	정점은 최대 4개 뼈대의 영향을 받음
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}
			//	4개 중 하나만 채우는거래 죄다 비어있다고 뼈 정보 다 가져가라는 게 아니라서
			//	else if 로 조건잡음
			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	return S_OK;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, aiMesh * pAIMesh)
{
	CMeshContainer*	pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pAIMesh)))
	{
		MSGBOX("Failed to Created CMeshContainer");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CComponent * CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*	pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMeshContainer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeshContainer::Free()
{
	__super::Free();

	m_HierarchyNodeIndices.clear();
}

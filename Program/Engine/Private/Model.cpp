#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Channel.h"
#include "Shader.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)
	, m_pMeshContainers(rhs.m_pMeshContainers)
	, m_iNumMeshContainers(rhs.m_iNumMeshContainers)
	, m_eModelType(rhs.m_eModelType)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iCurrentAnim(rhs.m_iCurrentAnim)
	, m_TransformMatrix(rhs.m_TransformMatrix)
{
	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(pMaterial->pTexture[i]);
	}

	if (nullptr != m_pMeshContainers)
	{
		for (_uint i = 0; i < m_iNumMaterials; ++i)
		{
			for (auto& pMeshContainer : m_pMeshContainers[i])
				Safe_AddRef(pMeshContainer);
		}
	}
}

CModel::BONEMATRIX_PTR CModel::Get_BoneMatrixPtr(const char * pBoneName)
{
	BONEMATRIX_PTR		BoneMatrixPtr;
	ZeroMemory(&BoneMatrixPtr, sizeof(BONEMATRIX_PTR));

	CHierarchyNode*		pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return BONEMATRIX_PTR();

	BoneMatrixPtr.pOffsetMatrix = pNode->Get_OffsetMatrixPtr();
	BoneMatrixPtr.pCombinedMatrix = pNode->Get_CombinedTransformationMatrixPtr();

	return BoneMatrixPtr;
}

_bool CModel::Get_LimitAnim()
{
	m_bLimitAnim = m_Animations[m_iCurrentAnim]->Get_LimitAnim();

	return m_bLimitAnim;
}

void CModel::Set_LimitAnimFalse()
{
	m_Animations[m_iCurrentAnim]->Set_LimitAnimFalse();
}

_uint CModel::Get_CurrentKeyFrame()
{
	return m_Animations[m_iCurrentAnim]->Get_CurrentKeyFrame();
}

HRESULT CModel::NativeConstruct_Prototype(MODELTYPE eModelType, const char* pModelFilePath, const char* pModelFileName, _fmatrix TransformMatrix)
{
	char	szFullPath[MAX_PATH] = "";

	//	������ ��� ����ް� �� �ڿ� �����̸����� ����
	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint		iFlag = 0;

	//	aiProcess_PreTransformVertices : �� ���°� �ٲ��� �ʴ´�(�ִϸ��̼��� ���� �ֵ�)
	//	aiProcess_ConvertToLeftHanded : ������ dx �޼���ǥ��� �ҷ���(fbxsdk�� �� ����� ��� assimp�� �ε�)
	//	aiProcess_Triangulate : �ﰢ���� �������� �ʴ� �ֵ��� �ٽ� �ﰢ��ȭ ������
	//	aiProcess_CalcTangentSpace : ��ָ���, ź��Ʈ ���ñ⸦ ������ ����ؼ� ��������

	if (TYPE_NONANIM == eModelType)
		iFlag = aiProcess_PreTransformVertices | /*aiProcess_GlobalScale | */ aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	//	assimp importer �ȿ� ���ǵǾ��ִ� ��ü, m_Importer �� ���ؼ� ReadFile()�� �޽������� �ҷ��ü��� �ִ�
	m_pScene = m_Importer.ReadFile(szFullPath, iFlag);

	if (nullptr == m_pScene)
		return E_FAIL;

	m_eModelType = eModelType;
	XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);

	//	�޽�(��)�� ���׸��� ������ �����Ѵ�
	//	���׸��� : ��ǻ��, ����ŧ��, ���, �̹̽��� ���� ����
	//	�ؽ��Ŀ� ��Ƽ� �ε���
	//	�ؽ��� �ε�
	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	//	�ִϸ��̼��� �ִ� ���� ���
	//	�ִϸ��̼��� ���� ���� ���� ����(���̾��Ű ���) �ʿ����
	if (TYPE_ANIM == m_eModelType)
	{
		//	���̾��Ű��� �غ� �۾��� ��
		//	��Ʈ ���κ��� �Ļ��ǰ� �ִ� �ڽ� ������ ���� ���鼭 ����
		if (FAILED(Ready_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
			return E_FAIL;

		//	����
		sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
		{
			return pSour->Get_Depth() < pDest->Get_Depth();
		});
	}

	//	�޽��� �д� ���
	if (FAILED(Ready_MeshContainers()))
		return E_FAIL;

	//	���ؽ� ���ۿ� �Ҵ�, �ε��� ���ۿ� �Ҵ��ϴ� �۾��� ��
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		for (auto& pMeshContainer : m_pMeshContainers[i])
			pMeshContainer->Ready_VertexIndexBuffer(m_eModelType, TransformMatrix);
	}
	
	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	//	�ִϸ��̼��� ����Ǵ� �ֵ� �ѿ����� ��������
	//	HierarchyNode���� �޽õ��� �ʱ���¸� �������ֱ� ���� ��ĵ� �ִ�!
	//	�̹� �ε��ؼ� �����Ҷ����� �� ����� �����Ǳ� ������ �ִϸ��̼��� ���� �ֵ��� �ε��Ҷ��� �ʿ䰡 ����(aiProcess_PreTransformVertices ���� ����)
	if (FAILED(Ready_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	//	������ �� ���������� sort�� ����
	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	//	�纻��ü�� �������� �޾�����ؼ� �� �ٽ� �޽��� ��ȸ�ϴ� �۾��� �ϰ� ������ ��Ʈ������ �����ϴ� �۾��� ��
	if (FAILED(Ready_OffsetMatrices()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	if (FAILED(Link_ChannelToNode()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Render(class CShader* pShader, _uint iPassIndex, _uint iMaterialIndex, const char* pBoneValueName)
{
	if (iMaterialIndex >= m_iNumMaterials ||
		nullptr == m_pMeshContainers)
		return E_FAIL;

	_float4x4		BoneMatrices[128];

	for (auto& pMeshContainer : m_pMeshContainers[iMaterialIndex])
	{
		//	�޽����� ��ȸ�ϸ鼭 �׸�
		if (TYPE_ANIM == m_eModelType)
		{
			ZeroMemory(&BoneMatrices, sizeof(_float4x4) * 128);

			pMeshContainer->SetUp_Matrices(BoneMatrices, &m_HierarchyNodes, XMLoadFloat4x4(&m_TransformMatrix));
			
			//	���� ����� ������ �۾�
			pShader->Set_RawValue(pBoneValueName, BoneMatrices, sizeof(_float4x4) * 128);
		}

		if (pMeshContainer->Get_Check())
		{
			if (nullptr != pMeshContainer)
				pMeshContainer->Render(pShader, iPassIndex);
		}

		if (m_bAllOn)
		{
			if (nullptr != pMeshContainer)
				pMeshContainer->Render(pShader, iPassIndex);
		}
	}

	return S_OK;
}

HRESULT CModel::Bind_OnShader(CShader * pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char * pValueName)
{
	//	���� �ؽ��ĸ� ȣ���Ҽ��� �����ϱ� ����ó��
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	//	���׸����ε����� ����ü�� �ִ� �ؽ����� �ؽ���Ŭ���� �ȿ� SetUp_OnShader�� ���̴�, �̸� ������ �ȴ�
	CTexture*		pTexture = m_Materials[iMaterialIndex]->pTexture[eTextureType];
	if (nullptr == pTexture)
		return E_FAIL;

	return pTexture->SetUp_OnShader(pShader, pValueName);
}

HRESULT CModel::SetUp_AnimIndex(_uint iAnimIndex, _bool isInfinity, _uint iNextIndex)
{
	//	�ε��� ����, �̰� ������ map���� �ؼ� �±׷� ������ �ǰ�
	if (iAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	//if (iNextIndex >= m_iNumAnimations)
	//	return E_FAIL;

	if (m_iCurrentAnim != iAnimIndex)
		m_Animations[m_iCurrentAnim]->Set_PlayTimeAcc();

	m_iCurrentAnim = iAnimIndex;

	m_bisInfinity = isInfinity;
	m_iNextAnimIndex = iNextIndex;

	return S_OK;
}

HRESULT CModel::Update_CombinedTransformationMatrices(_double TimeDelta)
{
	if (m_bisInfinity)
	{
		//	��� ������ ���� �ð��� �´� ���¸� ä�ο� �����Ŵ
		//	������ Ʈ��������Ʈ������ ���ŵ�
		m_Animations[m_iCurrentAnim]->Update_TransformMatrices(TimeDelta);

		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			//	�θ�κ��� �ڽı��� ��� ���¸� ������Ű�� �������ش�
			pHierarchyNode->Update_CombinedTransformationMatrix();
		}
	}
	else
	{
		//	��� ������ ���� �ð��� �´� ���¸� ä�ο� �����Ŵ
		//	������ Ʈ��������Ʈ������ ���ŵ�
		m_Animations[m_iCurrentAnim]->Update_TransformMatricesOneTime(TimeDelta);

		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			//	�θ�κ��� �ڽı��� ��� ���¸� ������Ű�� �������ش�
			pHierarchyNode->Update_CombinedTransformationMatrix();
		}

		if (m_Animations[m_iCurrentAnim]->Get_isFinished())
		{
			if (THEREISNONEXT != m_iNextAnimIndex)
			{
				m_bisInfinity = true;
				//m_bLimitAnim = true;
				SetUp_AnimIndex(m_iNextAnimIndex, m_bisInfinity);
			}
			else if (THEREISNONEXT == m_iNextAnimIndex)
				m_bisInfinity = true;
		}
	}

	return S_OK;
}

HRESULT CModel::Object_Active(const char* pMeshName, _bool bChoose)
{
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		for (auto& pMeshContainer : m_pMeshContainers[i])
		{
			char	szName[MAX_PATH] = "";
			strcpy_s(szName, pMeshContainer->Get_Name());

			if (0 == strcmp(pMeshName, szName))
			{
				if (true == bChoose)
				{
					pMeshContainer->Set_Check(bChoose);
				}
				else
				{
					pMeshContainer->Set_Check(bChoose);
				}

				break;
			}
		}
	}

	return S_OK;
}

void CModel::Set_AllOn()
{
	m_bAllOn = true;
}

void CModel::Set_AllOff()
{
	m_bAllOn = false;
}

HRESULT CModel::Ready_MeshContainers()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	//	iNumMeshContainers �Ű�ü�� mNumMeshes ����
	m_iNumMeshContainers = m_pScene->mNumMeshes;	//	mNumMeshes : ��� �޽��� �����Ǿ��ִ��� Ȯ���� �� ����

	//	�迭 �̸� Ȯ��
	m_pMeshContainers = new MESHCONTAINERS[m_iNumMeshContainers];

	for (_uint i = 0; i < m_iNumMeshContainers; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, m_pScene->mMeshes[i]);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		//	�ִϸ��̼� ���°Ÿ� NumBone�� 0���״ϱ� for���� ���� �ʴ´�
		for (_uint j = 0; j < m_pScene->mMeshes[i]->mNumBones; ++j)
		{
			//	�޽��ȿ� ���ǵǾ� �ִ� ��������(�޽��� ������ �ִ� ��) ĳġ�ؼ� ���̾��Ű ��带 ã��
			//	ã�� �ֵ��� �� �޽������̳ʿ� ��Ƶ�
			_uint		iIndex = 0;
			CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(m_pScene->mMeshes[i]->mBones[j]->mName.data, &iIndex);

			if (nullptr == pHierarchyNode)
				return E_FAIL;

			pMeshContainer->Add_HierarchyNode(iIndex);
		}

		m_pMeshContainers[m_pScene->mMeshes[i]->mMaterialIndex].push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char * pModelFilePath)
{
	if (nullptr == m_pScene)
		return E_FAIL;

	//	�޽��� ���׸��� ���� X, ��(Scene)�� ���׸��� ����
	m_iNumMaterials = m_pScene->mNumMaterials;

	//	���׸��� ������ŭ �ε�
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		//	���׸��� ���ö����� �����Ҵ�
		MESHMATERIALDESC*		pMeshMaterialDesc = new MESHMATERIALDESC;
		ZeroMemory(pMeshMaterialDesc, sizeof(MESHMATERIALDESC));

		for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString	TexturePath;

			//	j�� ���鼭 aiTexutreType���� ĳ����
			//	j : ��ǻ�� �����ð���, ����ŧ���� �����ð��� ���� ���ڰ����� �����°�
			if (FAILED(m_pScene->mMaterials[i]->GetTexture(aiTextureType(j), 0, &TexturePath)))
				continue;

			char		szFullPath[MAX_PATH] = "";
			char		szFileName[MAX_PATH] = "";
			char		szExt[MAX_PATH] = "";		//	Ȯ����

			//	�ؽ��� �̸��� �����÷���
			//	_splitpath_s(� ���ڿ���, ����̺�null, 0, ���丮null, 0, ĳ���ͳ���(szFileName), ����, Ȯ����(szExt), ����)
			_splitpath_s(TexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szTextureFullPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szTextureFullPath, (int)MAX_PATH);

			pMeshMaterialDesc->pTexture[j] = CTexture::Create(m_pDevice, m_pDeviceContext, szTextureFullPath);
			if (nullptr == pMeshMaterialDesc->pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(pMeshMaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth)
{
	if (nullptr == pNode)
		return E_FAIL;

	_float4x4		TransformationMatrix;

	//	Ư�� ���� ������ �ִ� Ű������, ������, ����� ������ TransformationMatrix�� ����
	//	(pNode->mTransformation)�� aifloat4x4 Ÿ���̶� memcpy�� ����
	//	�� TransformationMatrix�� ������ �ٲ� �� ����
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_float4x4));

	//	�� ��忡 ���� ������ �����
	//	���� �̸��� ����
	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode->mName.data, TransformationMatrix, iDepth, pParent);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		//	�ڽ� ����
		//	�ڽ� ��带 �ְ�, �θ�(pHierarchyNode)�� ���� �����Ű�� +1
		Ready_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth + 1);
	}

	/*
	�� ���, �θ� ����, 0
	�� ���(��Ʈ��� pNode) Create�ؿ� ����鼭 depth 0
	�� ��Ʈ����� �ڽ��� ��� ���, 0��° �ڽĳְ�, �θ�ְ�, depth�� 1�����ؼ� 1,
	�׸��� �ٽ� Ready �Լ� ȣ��, �ڽĳְ�, �θ�(��) �ְ� depth �־ �ٽôٽ� ����� �ں�����
	*/

	return S_OK;
}

HRESULT CModel::Ready_OffsetMatrices()
{
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		for (auto& pMeshContainer : m_pMeshContainers[i])
		{
			_uint iNumBones = pMeshContainer->Get_NumBones();

			aiMesh*		pAIMesh = pMeshContainer->Get_AIMesh();

			for (_uint j = 0; j < iNumBones; ++j)
			{
				aiBone*		pBone = pAIMesh->mBones[j];

				CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pBone->mName.data);

				if (nullptr == pHierarchyNode)
					return E_FAIL;

				//	aiŸ�����״ϱ� memcpy
				_float4x4		OffsetMatrix;
				memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_float4x4));

				pHierarchyNode->Set_OffsetMatrix(OffsetMatrix);
			}
		}
	}

	return S_OK;
}

HRESULT CModel::Ready_Animation()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_iNumAnimations = m_pScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation*	pAIAnimation = m_pScene->mAnimations[i];

		CAnimation*		pAnimation = CAnimation::Create(pAIAnimation->mName.data, pAIAnimation->mDuration, pAIAnimation->mTicksPerSecond);
		if (nullptr == pAnimation)
			return E_FAIL;

		//	Channel : �� �ִϸ��̼ǿ��� �����Ǿ�� �� ����, �ִϸ��̼��� ǥ���ϱ� ���� ����
		pAnimation->Reserve(pAIAnimation->mNumChannels);

		//	ä�� ������ŭ ��ȸ
		for (_uint j = 0; j < pAIAnimation->mNumChannels; ++j)
		{
			aiNodeAnim*	pAIChannel = pAIAnimation->mChannels[j];

			//	mNodeName : ä���� �̸�
			CChannel*	pChannel = CChannel::Create(pAIChannel->mNodeName.data);
			if (nullptr == pChannel)
				return E_FAIL;

			////	KeyFrame : Ư�� �ִϸ��̼��� ǥ���ϱ� ���� ���۵�
			////	iNumKeyFrames : 

			//	��ü Ű�������� ����� �˾ƾ��ϴµ� mNumScalingKeys, mNumRotationKeys, mNumPositionKeys ������ �� �ٸ� �� �ִ�
			//	�׷��� iNumKeyFrames, �� 3�� �߿� ���� ū(max) �������� �����(�ظ��ؼ� ������ Ű�������� ������ ����,,,�츮 �ڵ�� �װ� �������� ģ��)
			_uint		iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, pAIChannel->mNumPositionKeys);

			pChannel->Reserve(iNumKeyFrames);

			_float3		vScale;
			_float4		vRotation;
			_float3		vPosition;
			_double		Time;

			for (_uint k = 0; k < iNumKeyFrames; ++k)
			{
				//	����ü�� ���� ������ �����ؼ� pChannel �� �߰��ϰ� �� ä���� �ִϸ��̼ǿ� �߰��ϰ� �� �ִϸ��̼��� �� Ŭ���� �ȿ� �����̳ʿ� �߰���
				//	���� Ű���� ������ �����ϱ� �� ���� ���� �޾ƿ�����ϴ� �۾��� �ؾ���
				//	�׷��� k for�� �������� ���������� �߰���

				KEYFRAME*			pKeyFrame = new KEYFRAME;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				//	float4���ε� w x y z ������ memcpy �״�� �ϸ� ��
				//	Time�� ���ǹ� ���ö����� ä����
				if (pAIChannel->mNumScalingKeys > k)
				{
					memcpy(&vScale, &pAIChannel->mScalingKeys[k].mValue, sizeof(_float3));
					Time = pAIChannel->mScalingKeys[k].mTime;
				}

				if (pAIChannel->mNumRotationKeys > k)
				{
					//	Rotation�� �̻��� ���ʹϾ� ���ñ�� ���� ���� ä����
					vRotation.x = pAIChannel->mRotationKeys[k].mValue.x;
					vRotation.y = pAIChannel->mRotationKeys[k].mValue.y;
					vRotation.z = pAIChannel->mRotationKeys[k].mValue.z;
					vRotation.w = pAIChannel->mRotationKeys[k].mValue.w;
					Time = pAIChannel->mRotationKeys[k].mTime;
				}

				if (pAIChannel->mNumPositionKeys > k)
				{
					memcpy(&vPosition, &pAIChannel->mPositionKeys[k].mValue, sizeof(_float3));
					Time = pAIChannel->mPositionKeys[k].mTime;
				}

				//	�� ä����
				pKeyFrame->vScale = vScale;
				pKeyFrame->vRotation = vRotation;
				pKeyFrame->vPosition = vPosition;
				pKeyFrame->Time = Time;

				pChannel->Add_KeyFrame(pKeyFrame);
			}

			pAnimation->Add_Channel(pChannel);
		}

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Link_ChannelToNode()
{
	for (auto& pAnimation : m_Animations)
	{
		const vector<CChannel*>* pChannels = pAnimation->Get_Channels();

		for (auto& pChannel : *pChannels)
		{
			//	ä�ο��ٰ� ��带 ����ִ� �۾�
			//	�̸��� ���� �ָ� ã�Ƽ� ����
			CHierarchyNode* pNode = Find_HierarchyNode(pChannel->Get_Name());
			if (nullptr == pNode)
				return E_FAIL;

			pChannel->Set_HierarchyNodePtr(pNode);
		}
	}

	return S_OK;
}

CHierarchyNode * CModel::Find_HierarchyNode(const char * pName, _uint* pOut)
{
	_uint		iIndex = 0;
	CHierarchyNode*	pNode = nullptr;

	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		if (!strcmp(pHierarchyNode->Get_Name(), pName))
		{
			pNode = pHierarchyNode;
			//	pOut�� �ƴ� �ֵ��� ������ ������ ���� ���� ��쿡�� ����
			if (nullptr != pOut)
				*pOut = iIndex;

			break;
		}

		iIndex++;
	}
	return pNode;
}

CModel * CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODELTYPE eModelType, const char* pModelFilePath, const char* pModelFileName, _fmatrix TransformMatrix)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eModelType, pModelFilePath, pModelFileName, TransformMatrix)))
	{
		MSGBOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	//	m_Animations ����---------------------------
	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	//	m_Materials ����---------------------------
	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			if(nullptr!= pMaterial->pTexture[i])
				Safe_Release(pMaterial->pTexture[i]);
		}

		if (false == m_isCloned)
			Safe_Delete(pMaterial);
	}

	m_Materials.clear();

	//	m_pMeshContainers ����----------------------
	if (nullptr != m_pMeshContainers)
	{
		for (_uint i = 0; i < m_iNumMaterials; ++i)
		{
			for (auto& pMeshContainer : m_pMeshContainers[i])
				Safe_Release(pMeshContainer);

			if (false == m_isCloned)
				m_pMeshContainers[i].clear();
		}
	}

	if (false == m_isCloned)
	{
		m_Importer.FreeScene();
		Safe_Delete_Array(m_pMeshContainers);
	}

	//	m_HierarchyNodes ����----------------------
	for (auto& pNode : m_HierarchyNodes)
		Safe_Release(pNode);

	m_HierarchyNodes.clear();
}

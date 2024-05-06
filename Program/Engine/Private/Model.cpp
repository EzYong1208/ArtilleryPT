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

	//	파일의 경로 복사받고 그 뒤에 파일이름까지 붙임
	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint		iFlag = 0;

	//	aiProcess_PreTransformVertices : 뼈 상태가 바뀌지 않는다(애니메이션이 없는 애들)
	//	aiProcess_ConvertToLeftHanded : 무조건 dx 왼손좌표계로 불러옴(fbxsdk는 이 기능이 없어서 assimp로 로드)
	//	aiProcess_Triangulate : 삼각형을 구성하지 않는 애들을 다시 삼각형화 시켜줌
	//	aiProcess_CalcTangentSpace : 노멀매핑, 탄젠트 뭐시기를 스스로 계산해서 저장해줌

	if (TYPE_NONANIM == eModelType)
		iFlag = aiProcess_PreTransformVertices | /*aiProcess_GlobalScale | */ aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	//	assimp importer 안에 정의되어있는 객체, m_Importer 를 통해서 ReadFile()로 메쉬파일을 불러올수가 있다
	m_pScene = m_Importer.ReadFile(szFullPath, iFlag);

	if (nullptr == m_pScene)
		return E_FAIL;

	m_eModelType = eModelType;
	XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);

	//	메쉬(모델)의 머테리얼 정보를 세팅한다
	//	머테리얼 : 디퓨즈, 스펙큘러, 노멀, 이미시즈 등의 정보
	//	텍스쳐에 담아서 로드함
	//	텍스쳐 로드
	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	//	애니메이션이 있는 모델인 경우
	//	애니메이션이 없는 모델인 경우는 뼈가(하이어라키 노드) 필요없다
	if (TYPE_ANIM == m_eModelType)
	{
		//	하이어라키노드 준비 작업을 함
		//	루트 노드로부터 파생되고 있는 자식 노드들을 따라 가면서 생성
		if (FAILED(Ready_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
			return E_FAIL;

		//	정렬
		sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
		{
			return pSour->Get_Depth() < pDest->Get_Depth();
		});
	}

	//	메쉬를 읽는 기능
	if (FAILED(Ready_MeshContainers()))
		return E_FAIL;

	//	버텍스 버퍼에 할당, 인덱스 버퍼에 할당하는 작업을 함
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		for (auto& pMeshContainer : m_pMeshContainers[i])
			pMeshContainer->Ready_VertexIndexBuffer(m_eModelType, TransformMatrix);
	}
	
	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	//	애니메이션이 재생되는 애들 한에서만 넣을거임
	//	HierarchyNode에는 메시들의 초기상태를 정의해주기 위한 행렬도 있다!
	//	이미 로드해서 생성할때부터 위 기능이 제공되기 때문에 애니메이션이 없는 애들을 로드할때는 필요가 없다(aiProcess_PreTransformVertices 에서 해줌)
	if (FAILED(Ready_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	//	노드들을 다 생성했으면 sort로 정렬
	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	//	사본객체에 오프셋을 달아줘야해서 또 다시 메쉬를 순회하는 작업을 하고 오프셋 매트릭스를 세팅하는 작업을 함
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
		//	메쉬들을 순회하면서 그림
		if (TYPE_ANIM == m_eModelType)
		{
			ZeroMemory(&BoneMatrices, sizeof(_float4x4) * 128);

			pMeshContainer->SetUp_Matrices(BoneMatrices, &m_HierarchyNodes, XMLoadFloat4x4(&m_TransformMatrix));
			
			//	뼈의 행렬을 던지는 작업
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
	//	없는 텍스쳐를 호출할수도 있으니깐 예외처리
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	//	머테리얼인덱스의 구조체에 있는 텍스쳐의 텍스쳐클래스 안에 SetUp_OnShader에 쉐이더, 이름 던지면 된다
	CTexture*		pTexture = m_Materials[iMaterialIndex]->pTexture[eTextureType];
	if (nullptr == pTexture)
		return E_FAIL;

	return pTexture->SetUp_OnShader(pShader, pValueName);
}

HRESULT CModel::SetUp_AnimIndex(_uint iAnimIndex, _bool isInfinity, _uint iNextIndex)
{
	//	인덱스 던져, 이게 싫으면 map으로 해서 태그로 던져도 되고
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
		//	모든 뼈들의 현재 시간에 맞는 상태를 채널에 저장시킴
		//	노드들의 트랜스폼매트릭스가 갱신됨
		m_Animations[m_iCurrentAnim]->Update_TransformMatrices(TimeDelta);

		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			//	부모로부터 자식까지 행렬 상태를 누적시키며 세팅해준다
			pHierarchyNode->Update_CombinedTransformationMatrix();
		}
	}
	else
	{
		//	모든 뼈들의 현재 시간에 맞는 상태를 채널에 저장시킴
		//	노드들의 트랜스폼매트릭스가 갱신됨
		m_Animations[m_iCurrentAnim]->Update_TransformMatricesOneTime(TimeDelta);

		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			//	부모로부터 자식까지 행렬 상태를 누적시키며 세팅해준다
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

	//	iNumMeshContainers 신객체에 mNumMeshes 저장
	m_iNumMeshContainers = m_pScene->mNumMeshes;	//	mNumMeshes : 몇개의 메쉬로 구성되어있는지 확인할 수 있음

	//	배열 미리 확보
	m_pMeshContainers = new MESHCONTAINERS[m_iNumMeshContainers];

	for (_uint i = 0; i < m_iNumMeshContainers; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, m_pScene->mMeshes[i]);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		//	애니메이션 없는거면 NumBone이 0일테니깐 for문을 돌지 않는다
		for (_uint j = 0; j < m_pScene->mMeshes[i]->mNumBones; ++j)
		{
			//	메쉬안에 정의되어 있는 정보들을(메쉬에 영향을 주는 뼈) 캐치해서 하이어라키 노드를 찾고
			//	찾은 애들을 다 메쉬컨테이너에 모아둠
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

	//	메쉬의 머테리얼 갯수 X, 신(Scene)의 머테리얼 갯수
	m_iNumMaterials = m_pScene->mNumMaterials;

	//	머테리얼 갯수만큼 로드
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		//	머테리얼 들어올때마다 동적할당
		MESHMATERIALDESC*		pMeshMaterialDesc = new MESHMATERIALDESC;
		ZeroMemory(pMeshMaterialDesc, sizeof(MESHMATERIALDESC));

		for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString	TexturePath;

			//	j번 돌면서 aiTexutreType으로 캐스팅
			//	j : 디퓨즈 가져올건지, 스펙큘러를 가져올건지 내가 인자값으로 던지는거
			if (FAILED(m_pScene->mMaterials[i]->GetTexture(aiTextureType(j), 0, &TexturePath)))
				continue;

			char		szFullPath[MAX_PATH] = "";
			char		szFileName[MAX_PATH] = "";
			char		szExt[MAX_PATH] = "";		//	확장자

			//	텍스쳐 이름만 가져올려고
			//	_splitpath_s(어떤 문자열을, 드라이브null, 0, 디렉토리null, 0, 캐릭터네임(szFileName), 길이, 확장자(szExt), 길이)
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

	//	특정 뼈가 가지고 있는 키프레임, 스케일, 등등의 정보를 TransformationMatrix에 보관
	//	(pNode->mTransformation)이 aifloat4x4 타입이라 memcpy로 복사
	//	이 TransformationMatrix는 언제든 바뀔 수 있음
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_float4x4));

	//	내 노드에 대한 정보를 만들고
	//	뼈와 이름을 저장
	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode->mName.data, TransformationMatrix, iDepth, pParent);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		//	자식 세팅
		//	자식 노드를 주고, 부모(pHierarchyNode)를 던져 저장시키면 +1
		Ready_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth + 1);
	}

	/*
	내 노드, 부모 없어, 0
	내 노드(루트노드 pNode) Create해요 만들면서 depth 0
	이 루트노드의 자식이 몇개냐 계산, 0번째 자식넣고, 부모넣고, depth는 1증가해서 1,
	그리고 다시 Ready 함수 호출, 자식넣고, 부모(나) 넣고 depth 넣어서 다시다시 만들고 자빠지고
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

				//	ai타입일테니깐 memcpy
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

		//	Channel : 이 애니메이션에서 변형되어야 할 뼈들, 애니메이션을 표현하기 위한 뼈들
		pAnimation->Reserve(pAIAnimation->mNumChannels);

		//	채널 갯수만큼 순회
		for (_uint j = 0; j < pAIAnimation->mNumChannels; ++j)
		{
			aiNodeAnim*	pAIChannel = pAIAnimation->mChannels[j];

			//	mNodeName : 채널의 이름
			CChannel*	pChannel = CChannel::Create(pAIChannel->mNodeName.data);
			if (nullptr == pChannel)
				return E_FAIL;

			////	KeyFrame : 특정 애니메이션을 표현하기 위한 동작들
			////	iNumKeyFrames : 

			//	전체 키프레임이 몇개인지 알아야하는데 mNumScalingKeys, mNumRotationKeys, mNumPositionKeys 갯수가 다 다를 수 있다
			//	그래서 iNumKeyFrames, 위 3개 중에 가장 큰(max) 프레임을 갖고옴(왠만해선 포지션 키프레임의 갯수와 같다,,,우리 코드는 그걸 가정으로 친다)
			_uint		iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, pAIChannel->mNumPositionKeys);

			pChannel->Reserve(iNumKeyFrames);

			_float3		vScale;
			_float4		vRotation;
			_float3		vPosition;
			_double		Time;

			for (_uint k = 0; k < iNumKeyFrames; ++k)
			{
				//	구조체에 대한 정보를 생성해서 pChannel 에 추가하고 이 채널을 애니메이션에 추가하고 이 애니메이션을 모델 클래스 안에 컨테이너에 추가함
				//	없는 키값이 있을수 있으니깐 그 전의 값을 받아와줘야하는 작업을 해야함
				//	그래서 k for문 돌기전에 지역변수들 추가함

				KEYFRAME*			pKeyFrame = new KEYFRAME;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				//	float4개인데 w x y z 순서라서 memcpy 그대로 하면 됨
				//	Time은 조건문 들어올때마다 채워줌
				if (pAIChannel->mNumScalingKeys > k)
				{
					memcpy(&vScale, &pAIChannel->mScalingKeys[k].mValue, sizeof(_float3));
					Time = pAIChannel->mScalingKeys[k].mTime;
				}

				if (pAIChannel->mNumRotationKeys > k)
				{
					//	Rotation는 이상한 퀀터니엄 뭐시기라서 직접 값을 채워줌
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

				//	값 채워줌
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
			//	채널에다가 노드를 집어넣는 작업
			//	이름이 같은 애를 찾아서 넣음
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
			//	pOut이 아닌 애들이 있을수 있으니 값이 있을 경우에만 대입
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

	//	m_Animations 정리---------------------------
	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	//	m_Materials 정리---------------------------
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

	//	m_pMeshContainers 정리----------------------
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

	//	m_HierarchyNodes 정리----------------------
	for (auto& pNode : m_HierarchyNodes)
		Safe_Release(pNode);

	m_HierarchyNodes.clear();
}

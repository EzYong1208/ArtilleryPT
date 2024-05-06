#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::NativeConstruct(const char* pName, _double Duration, _double TickPerSecond)
{
	strcpy_s(m_szName, pName);

	m_Duration = Duration;
	m_TickPerSecond = TickPerSecond;

	return S_OK;
}

HRESULT CAnimation::Update_TransformMatrices(_double TimeDelta)
{
	//	현재 내 애니메이션의 재생 위치
	//	현재 내 애니메이션의 재생 위치(재생 시간이라 한다면 그냥 += TimeDelta, 위치라 m_TickPersec까지 곱함)
	m_PlayTimeAcc += m_TickPerSecond * TimeDelta * 2;

	//	누적시킨 시간이 총 재생 계산값(m_Duration)을 넘어가면 다시 m_PlayTimeAcc을 0으로 돌리고 Finished 는 참
	if (m_PlayTimeAcc >= m_Duration)
	{
		m_PlayTimeAcc = 0.0;
		m_isFinished = true;
	}
	else
		m_isFinished = false;

	_vector			vScale, vRotation, vPosition;

	//	현재 내 애니메이션 상태에서 재생된 시간에 따른 모든 뼈의 상태를 갱신함
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		//	애니메이션 재생이 끝나면 다시 루프
		if (true == m_isFinished)
			m_Channels[i]->Set_CurrentKeyFrame(0);

		//	현재 내 애니메이션 상태에서 재생된 시간에 따른 모든 뼈의 상태를 갱신함
		//	모든 뼈를 다 순회하면서 각각의 뼈들이 시간값에 따른 상태값을 표현한 키 프레임들을 가져옴
		const vector<KEYFRAME*>*	pKeyFrames = m_Channels[i]->Get_KeyFrames();
		if (nullptr == pKeyFrames)
			return E_FAIL;

		_uint iNumKeyFrame = _uint(pKeyFrames->size());

		_uint iCurrentKeyFrameIndex = m_Channels[i]->Get_CurrentKeyFrame();
		m_iCurrentKeyFrame = m_Channels[i]->Get_CurrentKeyFrame();

		//	마지막 키프레임을 넘어가면
		if (m_PlayTimeAcc >= (*pKeyFrames)[iNumKeyFrame - 1]->Time)
		{
			vScale = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrame - 1]->vScale);
			vRotation = XMLoadFloat4(&(*pKeyFrames)[iNumKeyFrame - 1]->vRotation);
			vPosition = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrame - 1]->vPosition);
		}
		//	특정 키프레임 두개 사이에 존재할때?
		else
		{
			//	딱 다음 키프레임으로 넘어갔니? 라고 조건으로 물어봐줘야한다
			/*
			TimeDelta가 큰값이 들어오면 m_PlayTimeAcc 값이 너무 커져서 모델이 막 이상해짐
			너무 큰값이 들어와서 키프레임을 막 2개씩 건너뛰고 그럼, 근데 우린 1개만 건너뛰게 해가지고 키프레임이 막 스킵되는 문제가 발생함
			Ratio가 1보다 큰값이 나와버림(1프레임 차로 나눠두게 짯기때문에)
			if말구 while로 바꿨다, 그러면 막 메쉬?모델이 막 튀는 현상이 사라짐
			*/
			while (m_PlayTimeAcc >= (*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time)
			{
				//	우선 나의 iCurrentKeyFrameIndex를 증가시키고 채널한테도 Set_CurrentKeyFrame으로 넣어줌
				m_Channels[i]->Set_CurrentKeyFrame(++iCurrentKeyFrameIndex);
			}

			_double		Ratio = (m_PlayTimeAcc - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time)
				/ ((*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time);

			//	내 키프레임에 대한 스케일값(vSourScale), 그 다음 키프레임에 대한 스케일값(vDestScale)
			_vector		vSourScale, vDestScale;
			_vector		vSourRotation, vDestRotation;
			_vector		vSourPosition, vDestPosition;

			vSourScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vScale);
			vSourRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vRotation);
			vSourPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vPosition);

			vDestScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vScale);
			vDestRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vRotation);
			vDestPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vPosition);

			//	XMVectorLerp(vSour 랑 vDest 사이(를 1로보고)의 퍼센테지)Ratio, float으로 받아야해서 캐스팅
			vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
			//	Rotation은 XMQuaternionSlerp,,, 쿼터니언 뭐시기를 선형보관하기 위한 함수
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));
		}

		vPosition = XMVectorSetW(vPosition, 1.f);

		//	XMMatrixAffineTransformation : 크자이공부 해주는 함수
		//	TransformationMatrix : 뼈들의 상태행렬
		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		//	우선은 채널에 보관해주고 이걸 하이어라키노드에 전달	
		m_Channels[i]->Set_TransformationMatrix(TransformationMatrix);
	}

	return S_OK;
}

HRESULT CAnimation::Update_TransformMatricesOneTime(_double TimeDelta)
{
	//	현재 내 애니메이션의 재생 위치
	//	현재 내 애니메이션의 재생 위치(재생 시간이라 한다면 그냥 += TimeDelta, 위치라 m_TickPersec까지 곱함)
	m_PlayTimeAcc += m_TickPerSecond * TimeDelta * 2;

	//	누적시킨 시간이 총 재생 계산값(m_Duration)을 넘어가면 다시 m_PlayTimeAcc을 0으로 돌리고 Finished 는 참
	if (m_PlayTimeAcc >= m_Duration)
	{
		m_PlayTimeAcc = 0.0;
		m_bLimitAnim = true;
		m_isFinished = true;

		//	현재 내 애니메이션 상태에서 재생된 시간에 따른 모든 뼈의 상태를 갱신함
		for (_uint i = 0; i < m_iNumChannels; ++i)
		{
			m_Channels[i]->Set_CurrentKeyFrame(0);
		}

		return S_OK;
	}
	else
	{
		//m_bLimitAnim = false;
		m_isFinished = false;
	}

	_vector			vScale, vRotation, vPosition;

	//	현재 내 애니메이션 상태에서 재생된 시간에 따른 모든 뼈의 상태를 갱신함
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		//	애니메이션 재생이 끝나면 다시 루프
		if (true == m_isFinished)
		{
			m_Channels[i]->Set_CurrentKeyFrame(0);
		}

		if (false == m_isFinished)
		{
			//	현재 내 애니메이션 상태에서 재생된 시간에 따른 모든 뼈의 상태를 갱신함
			//	모든 뼈를 다 순회하면서 각각의 뼈들이 시간값에 따른 상태값을 표현한 키 프레임들을 가져옴
			const vector<KEYFRAME*>*	pKeyFrames = m_Channels[i]->Get_KeyFrames();
			if (nullptr == pKeyFrames)
				return E_FAIL;

			_uint iNumKeyFrame = _uint(pKeyFrames->size());

			_uint iCurrentKeyFrameIndex = m_Channels[i]->Get_CurrentKeyFrame();
			m_iCurrentKeyFrame = m_Channels[i]->Get_CurrentKeyFrame();

			//	마지막 키프레임을 넘어가면
			if (m_PlayTimeAcc >= (*pKeyFrames)[iNumKeyFrame - 1]->Time)
			{
				vScale = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrame - 1]->vScale);
				vRotation = XMLoadFloat4(&(*pKeyFrames)[iNumKeyFrame - 1]->vRotation);
				vPosition = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrame - 1]->vPosition);
			}
			//	특정 키프레임 두개 사이에 존재할때?
			else
			{
				//	딱 다음 키프레임으로 넘어갔니? 라고 조건으로 물어봐줘야한다
				/*
				TimeDelta가 큰값이 들어오면 m_PlayTimeAcc 값이 너무 커져서 모델이 막 이상해짐
				너무 큰값이 들어와서 키프레임을 막 2개씩 건너뛰고 그럼, 근데 우린 1개만 건너뛰게 해가지고 키프레임이 막 스킵되는 문제가 발생함
				Ratio가 1보다 큰값이 나와버림(1프레임 차로 나눠두게 짯기때문에)
				if말구 while로 바꿨다, 그러면 막 메쉬?모델이 막 튀는 현상이 사라짐
				*/
				while (m_PlayTimeAcc >= (*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time)
				{
					//	우선 나의 iCurrentKeyFrameIndex를 증가시키고 채널한테도 Set_CurrentKeyFrame으로 넣어줌
					m_Channels[i]->Set_CurrentKeyFrame(++iCurrentKeyFrameIndex);
				}

				_double		Ratio = (m_PlayTimeAcc - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time)
					/ ((*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time);

				//	내 키프레임에 대한 스케일값(vSourScale), 그 다음 키프레임에 대한 스케일값(vDestScale)
				_vector		vSourScale, vDestScale;
				_vector		vSourRotation, vDestRotation;
				_vector		vSourPosition, vDestPosition;

				vSourScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vScale);
				vSourRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vRotation);
				vSourPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vPosition);

				vDestScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vScale);
				vDestRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vRotation);
				vDestPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vPosition);

				//	XMVectorLerp(vSour 랑 vDest 사이(를 1로보고)의 퍼센테지)Ratio, float으로 받아야해서 캐스팅
				vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
				//	Rotation은 XMQuaternionSlerp,,, 쿼터니언 뭐시기를 선형보관하기 위한 함수
				vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
				vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));
			}

			vPosition = XMVectorSetW(vPosition, 1.f);

			//	XMMatrixAffineTransformation : 크자이공부 해주는 함수
			//	TransformationMatrix : 뼈들의 상태행렬
			_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

			//	우선은 채널에 보관해주고 이걸 하이어라키노드에 전달	
			m_Channels[i]->Set_TransformationMatrix(TransformationMatrix);
		}
	}

	return S_OK;
}

void CAnimation::Set_PlayTimeAcc()
{
	m_PlayTimeAcc = 0.0;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Set_CurrentKeyFrame(0);
	}
}

void CAnimation::Set_LimitAnimFalse()
{
	m_bLimitAnim = false;
}

CAnimation * CAnimation::Create(const char* pName, _double Duration, _double TickPerSecond)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct(pName, Duration, TickPerSecond)))
	{
		MSGBOX("Failed to Created CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}


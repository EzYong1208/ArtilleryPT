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
	//	���� �� �ִϸ��̼��� ��� ��ġ
	//	���� �� �ִϸ��̼��� ��� ��ġ(��� �ð��̶� �Ѵٸ� �׳� += TimeDelta, ��ġ�� m_TickPersec���� ����)
	m_PlayTimeAcc += m_TickPerSecond * TimeDelta * 2;

	//	������Ų �ð��� �� ��� ��갪(m_Duration)�� �Ѿ�� �ٽ� m_PlayTimeAcc�� 0���� ������ Finished �� ��
	if (m_PlayTimeAcc >= m_Duration)
	{
		m_PlayTimeAcc = 0.0;
		m_isFinished = true;
	}
	else
		m_isFinished = false;

	_vector			vScale, vRotation, vPosition;

	//	���� �� �ִϸ��̼� ���¿��� ����� �ð��� ���� ��� ���� ���¸� ������
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		//	�ִϸ��̼� ����� ������ �ٽ� ����
		if (true == m_isFinished)
			m_Channels[i]->Set_CurrentKeyFrame(0);

		//	���� �� �ִϸ��̼� ���¿��� ����� �ð��� ���� ��� ���� ���¸� ������
		//	��� ���� �� ��ȸ�ϸ鼭 ������ ������ �ð����� ���� ���°��� ǥ���� Ű �����ӵ��� ������
		const vector<KEYFRAME*>*	pKeyFrames = m_Channels[i]->Get_KeyFrames();
		if (nullptr == pKeyFrames)
			return E_FAIL;

		_uint iNumKeyFrame = _uint(pKeyFrames->size());

		_uint iCurrentKeyFrameIndex = m_Channels[i]->Get_CurrentKeyFrame();
		m_iCurrentKeyFrame = m_Channels[i]->Get_CurrentKeyFrame();

		//	������ Ű�������� �Ѿ��
		if (m_PlayTimeAcc >= (*pKeyFrames)[iNumKeyFrame - 1]->Time)
		{
			vScale = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrame - 1]->vScale);
			vRotation = XMLoadFloat4(&(*pKeyFrames)[iNumKeyFrame - 1]->vRotation);
			vPosition = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrame - 1]->vPosition);
		}
		//	Ư�� Ű������ �ΰ� ���̿� �����Ҷ�?
		else
		{
			//	�� ���� Ű���������� �Ѿ��? ��� �������� ���������Ѵ�
			/*
			TimeDelta�� ū���� ������ m_PlayTimeAcc ���� �ʹ� Ŀ���� ���� �� �̻�����
			�ʹ� ū���� ���ͼ� Ű�������� �� 2���� �ǳʶٰ� �׷�, �ٵ� �츰 1���� �ǳʶٰ� �ذ����� Ű�������� �� ��ŵ�Ǵ� ������ �߻���
			Ratio�� 1���� ū���� ���͹���(1������ ���� �����ΰ� ­�⶧����)
			if���� while�� �ٲ��, �׷��� �� �޽�?���� �� Ƣ�� ������ �����
			*/
			while (m_PlayTimeAcc >= (*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time)
			{
				//	�켱 ���� iCurrentKeyFrameIndex�� ������Ű�� ä�����׵� Set_CurrentKeyFrame���� �־���
				m_Channels[i]->Set_CurrentKeyFrame(++iCurrentKeyFrameIndex);
			}

			_double		Ratio = (m_PlayTimeAcc - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time)
				/ ((*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time);

			//	�� Ű�����ӿ� ���� �����ϰ�(vSourScale), �� ���� Ű�����ӿ� ���� �����ϰ�(vDestScale)
			_vector		vSourScale, vDestScale;
			_vector		vSourRotation, vDestRotation;
			_vector		vSourPosition, vDestPosition;

			vSourScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vScale);
			vSourRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vRotation);
			vSourPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vPosition);

			vDestScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vScale);
			vDestRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vRotation);
			vDestPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vPosition);

			//	XMVectorLerp(vSour �� vDest ����(�� 1�κ���)�� �ۼ�����)Ratio, float���� �޾ƾ��ؼ� ĳ����
			vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
			//	Rotation�� XMQuaternionSlerp,,, ���ʹϾ� ���ñ⸦ ���������ϱ� ���� �Լ�
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));
		}

		vPosition = XMVectorSetW(vPosition, 1.f);

		//	XMMatrixAffineTransformation : ũ���̰��� ���ִ� �Լ�
		//	TransformationMatrix : ������ �������
		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		//	�켱�� ä�ο� �������ְ� �̰� ���̾��Ű��忡 ����	
		m_Channels[i]->Set_TransformationMatrix(TransformationMatrix);
	}

	return S_OK;
}

HRESULT CAnimation::Update_TransformMatricesOneTime(_double TimeDelta)
{
	//	���� �� �ִϸ��̼��� ��� ��ġ
	//	���� �� �ִϸ��̼��� ��� ��ġ(��� �ð��̶� �Ѵٸ� �׳� += TimeDelta, ��ġ�� m_TickPersec���� ����)
	m_PlayTimeAcc += m_TickPerSecond * TimeDelta * 2;

	//	������Ų �ð��� �� ��� ��갪(m_Duration)�� �Ѿ�� �ٽ� m_PlayTimeAcc�� 0���� ������ Finished �� ��
	if (m_PlayTimeAcc >= m_Duration)
	{
		m_PlayTimeAcc = 0.0;
		m_bLimitAnim = true;
		m_isFinished = true;

		//	���� �� �ִϸ��̼� ���¿��� ����� �ð��� ���� ��� ���� ���¸� ������
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

	//	���� �� �ִϸ��̼� ���¿��� ����� �ð��� ���� ��� ���� ���¸� ������
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		//	�ִϸ��̼� ����� ������ �ٽ� ����
		if (true == m_isFinished)
		{
			m_Channels[i]->Set_CurrentKeyFrame(0);
		}

		if (false == m_isFinished)
		{
			//	���� �� �ִϸ��̼� ���¿��� ����� �ð��� ���� ��� ���� ���¸� ������
			//	��� ���� �� ��ȸ�ϸ鼭 ������ ������ �ð����� ���� ���°��� ǥ���� Ű �����ӵ��� ������
			const vector<KEYFRAME*>*	pKeyFrames = m_Channels[i]->Get_KeyFrames();
			if (nullptr == pKeyFrames)
				return E_FAIL;

			_uint iNumKeyFrame = _uint(pKeyFrames->size());

			_uint iCurrentKeyFrameIndex = m_Channels[i]->Get_CurrentKeyFrame();
			m_iCurrentKeyFrame = m_Channels[i]->Get_CurrentKeyFrame();

			//	������ Ű�������� �Ѿ��
			if (m_PlayTimeAcc >= (*pKeyFrames)[iNumKeyFrame - 1]->Time)
			{
				vScale = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrame - 1]->vScale);
				vRotation = XMLoadFloat4(&(*pKeyFrames)[iNumKeyFrame - 1]->vRotation);
				vPosition = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrame - 1]->vPosition);
			}
			//	Ư�� Ű������ �ΰ� ���̿� �����Ҷ�?
			else
			{
				//	�� ���� Ű���������� �Ѿ��? ��� �������� ���������Ѵ�
				/*
				TimeDelta�� ū���� ������ m_PlayTimeAcc ���� �ʹ� Ŀ���� ���� �� �̻�����
				�ʹ� ū���� ���ͼ� Ű�������� �� 2���� �ǳʶٰ� �׷�, �ٵ� �츰 1���� �ǳʶٰ� �ذ����� Ű�������� �� ��ŵ�Ǵ� ������ �߻���
				Ratio�� 1���� ū���� ���͹���(1������ ���� �����ΰ� ­�⶧����)
				if���� while�� �ٲ��, �׷��� �� �޽�?���� �� Ƣ�� ������ �����
				*/
				while (m_PlayTimeAcc >= (*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time)
				{
					//	�켱 ���� iCurrentKeyFrameIndex�� ������Ű�� ä�����׵� Set_CurrentKeyFrame���� �־���
					m_Channels[i]->Set_CurrentKeyFrame(++iCurrentKeyFrameIndex);
				}

				_double		Ratio = (m_PlayTimeAcc - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time)
					/ ((*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time);

				//	�� Ű�����ӿ� ���� �����ϰ�(vSourScale), �� ���� Ű�����ӿ� ���� �����ϰ�(vDestScale)
				_vector		vSourScale, vDestScale;
				_vector		vSourRotation, vDestRotation;
				_vector		vSourPosition, vDestPosition;

				vSourScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vScale);
				vSourRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vRotation);
				vSourPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vPosition);

				vDestScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vScale);
				vDestRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vRotation);
				vDestPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vPosition);

				//	XMVectorLerp(vSour �� vDest ����(�� 1�κ���)�� �ۼ�����)Ratio, float���� �޾ƾ��ؼ� ĳ����
				vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
				//	Rotation�� XMQuaternionSlerp,,, ���ʹϾ� ���ñ⸦ ���������ϱ� ���� �Լ�
				vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
				vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));
			}

			vPosition = XMVectorSetW(vPosition, 1.f);

			//	XMMatrixAffineTransformation : ũ���̰��� ���ִ� �Լ�
			//	TransformationMatrix : ������ �������
			_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

			//	�켱�� ä�ο� �������ְ� �̰� ���̾��Ű��忡 ����	
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


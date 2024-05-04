#pragma once

#include "Base.h"

//	�ִϸ��̼��� �������ֱ� ���� Ŭ����

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	void							Reserve(_uint iNumChannels) { m_Channels.reserve(iNumChannels); m_iNumChannels = iNumChannels; }
	const vector<class CChannel*>*	Get_Channels() const { return &m_Channels; }

	void							Set_PlayTimeAcc();
	void							Set_LimitAnimFalse();

	_bool							Get_isFinished() { return m_isFinished; }
	_bool							Get_LimitAnim() { return m_bLimitAnim; }
	_uint							Get_CurrentKeyFrame() const { return m_iCurrentKeyFrame; }

public:
	HRESULT							NativeConstruct(const char* pName, _double Duration, _double TickPerSecond);
	HRESULT							Update_TransformMatrices(_double TimeDelta);

	void							Add_Channel(class CChannel* pChannel) { m_Channels.push_back(pChannel); }

	HRESULT							Update_TransformMatricesOneTime(_double TimeDelta);

private:
	//	�ִϸ��̼� �̸�(�ε����� �����ϴµ� �̸����� �����ϰ� ���� �ɰ� ����)
	char							m_szName[MAX_PATH] = "";
	//	�ִϸ��̼��� �� ����ð� 
	_double							m_Duration = 0.0;
	//	�ð��� �ִϸ��̼� ��� �ӵ�, ������ �ϰ� �Ͱų� ������ �ϰ������ �̰� ����
	_double							m_TickPerSecond = 0.0;
	//	���� �ִϸ��̼��� �÷���Ÿ��
	_double							m_PlayTimeAcc = 0.0;
	//	�� �ִϸ��̼��� ����ϴµ� ����ϴ� ���� ����
	_uint							m_iNumChannels = 0;

	//	ä�ε��� �����ϱ� ���� �����̳�
	vector<class CChannel*>			m_Channels;
	typedef vector<class CChannel*>	CHANNELS;

	_bool							m_isFinished = false;
	_bool							m_bLimitAnim = false;
	_uint							m_iCurrentKeyFrame = 0;

public:
	static CAnimation*				Create(const char* pName, _double Duration, _double TickPerSecond);
	virtual void					Free() override;
};

END
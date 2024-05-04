#pragma once

#include "Base.h"

//	애니메이션을 관리해주기 위한 클래스

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
	//	애니메이션 이름(인덱스로 접근하는데 이름으로 접근하게 만들어도 될거 같다)
	char							m_szName[MAX_PATH] = "";
	//	애니메이션의 총 재생시간 
	_double							m_Duration = 0.0;
	//	시간당 애니메이션 재생 속도, 빠르게 하고 싶거나 느리게 하고싶으면 이거 조절
	_double							m_TickPerSecond = 0.0;
	//	현재 애니메이션의 플레이타임
	_double							m_PlayTimeAcc = 0.0;
	//	이 애니메이션을 재생하는데 사용하는 뼈의 갯수
	_uint							m_iNumChannels = 0;

	//	채널들을 보관하기 위한 컨테이너
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
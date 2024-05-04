#pragma once

namespace Engine
{
	typedef	unsigned	char			_ubyte;
	typedef	signed		char			_byte;

	typedef	unsigned	short			_ushort;
	typedef	signed		short			_short;

	typedef	unsigned	int				_uint;
	typedef	signed		int				_int;

	typedef	unsigned	long			_ulong;
	typedef	signed		long			_long;

	typedef	float						_float;
	typedef	double						_double;

	typedef	bool						_bool;

	typedef	wchar_t						_tchar;

	//	XMFLOAT 타입, 기본적인 벡터타입만을 제공, 연산같은건 안됨
	typedef XMFLOAT2					_float2;
	typedef XMFLOAT3					_float3;

	//	생성자를 하나 만들때 위의 _float3오고 , _float fW로
	//	생성자로 XMFLOAT4를 호출(x, y, z, fW) 넣어서 생성자를 정의
	//	디폴트 생성자도 정의해줘야함
	typedef struct tagFloat4_Derived final : public XMFLOAT4
	{
		tagFloat4_Derived() = default;
		tagFloat4_Derived(_float3 vFloat3, _float fW) :
			XMFLOAT4(vFloat3.x, vFloat3.y, vFloat3.z, fW) { }
		tagFloat4_Derived(_float fX, _float fY, _float fZ, _float fW) :
			XMFLOAT4(fX, fY, fZ, fW) { }
	}_float4;

	//	XMFLOAT4 << 플롯 데이터가 4개가 있는 구조체, 각각 변수에 연산을 했었음
	//	XMVECTOR << 하나의 데이터 덩어리, 플롯이 4개 이어져있는 배열, 연산이 한번 일어남


	//	SIMD
	//	한번에 연산으로 모든 연산을 처리하기 위한 녀석
	typedef XMVECTOR					_vector;
	typedef FXMVECTOR					_fvector;

	typedef XMFLOAT4X4					_float4x4;
	typedef XMMATRIX					_matrix;
	typedef FXMMATRIX					_fmatrix;
	typedef CXMMATRIX					_cmatrix;

	//	함수 선언할때 벡터를 3개 받고 싶으면 FXMVECTOR, FXMVECTOR, FXMVECTOR로 받고 4개부터는 GXMVECTOR, 5개6개부터는 HXMVECTOR, 7개부터는 CXMVECTOR로....뭔 개소리일까
	//	포인터를 이용하지 않는게 좋다
}
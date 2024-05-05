#pragma once

#include "Base.h"

class MainApp
{
public:


private:
	//	처음 레벨을 뭐로 할지 정함
	HRESULT Open_Level();

public:
	//	static 함수이기 때문에 멤버변수를 사용할 수 없다
	static	MainApp* Create();
};

#pragma once

#include "MainApp.h"

class CLoader
{
public:
	HRESULT LoadingTest();

public:
	string Get_ResourceMap_Path(string PrototypeTag);

private:
	map<string, string> ResourceMap;

};


#pragma once

#include "MainApp.h"

class Loader
{
public:
	HRESULT LoadingTest();

public:
	HRESULT	Add_Prototype_Textures();

public:
	string Get_ResourceMap_Path(string PrototypeTag);

private:
	map<string, string> ResourceMap;

};


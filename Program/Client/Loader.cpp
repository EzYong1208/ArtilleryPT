#include "Loader.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include "AObject.h"

HRESULT Loader::LoadingTest()
{
	//if (FAILED(Add_Prototype_Textures()))
	//	return E_FAIL;

	AObject* AObject_ptr = new AObject();
	AObject_ptr->Test();

	return S_OK;
}

HRESULT Loader::Add_Prototype_Textures()
{
	//../../a/a.txt
	//filesystem::path ResourcePath("../../../Resource/TextureList.txt");
	//filesystem::path ResourcePath("../../../Resource");

	//filesystem::recursive_directory_iterator	recursive_directory_iter(ResourcePath);

	//while (recursive_directory_iter != filesystem::end(recursive_directory_iter))
	//{
	//	const filesystem::directory_entry& Entry = *recursive_directory_iter;
	//	filesystem::path temp = Entry.path();
	//	recursive_directory_iter++;
	//}

	string FileName("../../../Resource/TextureList.txt");

	vector<string>	lines;
	string line = "";

	ifstream input_file(FileName);

	if (!input_file.is_open())
	{
		int a = 0;
		return E_FAIL;
	}

	while (getline(input_file, line))
	{
		string PrototypeTag = "Prototype_Component_Texture_" + line.substr(0, line.find(" "));
		string Path = "../../../Resource/Textures/" +	line.substr(line.find(" ") + 1, line.length());

		ResourceMap.emplace(PrototypeTag, Path);

		//if (FAILED(pGameInstance->Add_Prototype(
		//	LEVEL_TEST,
		//	PrototypeTag,
		//	CTexture::Create(m_pDevice, m_pDeviceContext, Path))))
		//	return E_FAIL;
	}

	return S_OK;
}

string Loader::Get_ResourceMap_Path(string PrototypeTag)
{
	map<string, string>::iterator	iter;
	iter = ResourceMap.find(PrototypeTag);
	string Path = "";

	if (ResourceMap.end() == iter)
	{
		MSGBOX("Failed to Find Path");
	}
	else
	{
		Path = iter->second;
	}

	return Path;
}

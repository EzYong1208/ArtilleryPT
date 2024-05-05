#include "Loader.h"
#include <filesystem>
#include <fstream>
#include <iostream>

HRESULT CLoader::LoadingTest()
{
	//../../a/a.txt
	//filesystem::path ResourcePath("../../../Resource/TextureList.txt");
	//filesystem::path ResourcePath("../../../Resource");

	// 디렉토리 안에 모든 파일들 순회
	// 디렉토리는 책의 목차!
	// 쉽게 접근하기 위해서 filesystem 라이브러리에서 directory_iterator 라는 반복자를 제공
	// directory_iterator는 반복자 자체를 스스로 정의해야 하고, 반복자의 생성자에 우리가 탐색할 경로를 전달해줘야 함
	//filesystem::directory_iterator	iter(ResourcePath);

	// filesystem에 정의되어 있는 end 함수에 현재 반복자를 전달하면 해당 반복자의 끝을 얻어낼 수 있음
	//while (iter != filesystem::end(iter))
	//{
	//	// 각각의 반복자들은 디렉토리에 정의되어 있는 개개의 파일을 나타내는 directory_entry 를 가리키고 있음
	//	// directory_entry 에는 여러가지 정보들이 저장되어 있는데 파일의 이름이나, 크기 등등을 알 수 있음
	//	const filesystem::directory_entry& Entry = *iter;
	//	filesystem::path temp = Entry.path();
	//	iter++;
	//}
	
	// 단 directory_entry의 단점은 해당 디렉토리 안에 다른 디렉토리가 있을 경우 그 안까지는 살펴보지 않음!
	// 디렉토리 안에 서브 디렉토리까지 모두 순회할 수 있는 반복자를 사용하고 싶다면 recursive_directory_iterator 를 사용!!
	// 텍스처 등의 파일은 Resource 폴더 내의 다른 폴더를 생성해 넣을거고, 그 이름과 PrototypeTag를 써둔 TextureList는 Resource 폴더에다 넣을거니깐 적절히 쓰면 될거같다!


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
		string PrototypeTag = line.substr(0, line.find(" "));
		string Path = line.substr(line.find(" ") + 1, line.length());

		ResourceMap.emplace(PrototypeTag, Path);

		//if (FAILED(pGameInstance->Add_Prototype(
		//	0,
		//	PrototypeTag,
		//	CTexture::Create(m_pDevice, m_pDeviceContext, Path))))
		//	return E_FAIL;
	}

	return S_OK;
}

string CLoader::Get_ResourceMap_Path(string PrototypeTag)
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

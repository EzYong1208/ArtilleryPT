#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

unique_ptr<ResourceManager> ResourceManager::Create()
{
	return make_unique<ResourceManager>();
}

void ResourceManager::Release()
{
	// ResourceManager 객체의 자원 해제 또는 상태 정리 작업 수행
	// 파일 닫기, 데이터 정리 등
}

GameError ResourceManager::Add_Prototype_Textures()
{
	string FileName("../../../Resource/TextureList.txt");

	vector<string>	lines;
	string line = "";

	ifstream input_file(FileName);

	if (!input_file.is_open())
	{
		return GameError::HaveNoResource;
	}

	while (getline(input_file, line))
	{
		string PrototypeTag = "Prototype_Component_Texture_" + line.substr(0, line.find(" "));
		string Path = "../../../Resource/Textures/" + line.substr(line.find(" ") + 1, line.length());

		ResourceMap.emplace(PrototypeTag, Path);

		//if (FAILED(pGameInstance->Add_Prototype(
		//	LEVEL_TEST,
		//	PrototypeTag,
		//	CTexture::Create(m_pDevice, m_pDeviceContext, Path))))
		//	return E_FAIL;
	}

	return GameError::Success;
}

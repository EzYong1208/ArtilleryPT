#include "AResourceManager.h"

AResourceManager::AResourceManager()
{
}

GameError AResourceManager::Add_Textures()
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
	}

	return GameError::Success;
}

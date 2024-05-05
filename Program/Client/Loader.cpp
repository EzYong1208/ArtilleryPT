#include "Loader.h"
#include <filesystem>
#include <fstream>
#include <iostream>

HRESULT CLoader::LoadingTest()
{
	//../../a/a.txt
	//filesystem::path ResourcePath("../../../Resource/TextureList.txt");
	//filesystem::path ResourcePath("../../../Resource");

	// ���丮 �ȿ� ��� ���ϵ� ��ȸ
	// ���丮�� å�� ����!
	// ���� �����ϱ� ���ؼ� filesystem ���̺귯������ directory_iterator ��� �ݺ��ڸ� ����
	// directory_iterator�� �ݺ��� ��ü�� ������ �����ؾ� �ϰ�, �ݺ����� �����ڿ� �츮�� Ž���� ��θ� ��������� ��
	//filesystem::directory_iterator	iter(ResourcePath);

	// filesystem�� ���ǵǾ� �ִ� end �Լ��� ���� �ݺ��ڸ� �����ϸ� �ش� �ݺ����� ���� �� �� ����
	//while (iter != filesystem::end(iter))
	//{
	//	// ������ �ݺ��ڵ��� ���丮�� ���ǵǾ� �ִ� ������ ������ ��Ÿ���� directory_entry �� ����Ű�� ����
	//	// directory_entry ���� �������� �������� ����Ǿ� �ִµ� ������ �̸��̳�, ũ�� ����� �� �� ����
	//	const filesystem::directory_entry& Entry = *iter;
	//	filesystem::path temp = Entry.path();
	//	iter++;
	//}
	
	// �� directory_entry�� ������ �ش� ���丮 �ȿ� �ٸ� ���丮�� ���� ��� �� �ȱ����� ���캸�� ����!
	// ���丮 �ȿ� ���� ���丮���� ��� ��ȸ�� �� �ִ� �ݺ��ڸ� ����ϰ� �ʹٸ� recursive_directory_iterator �� ���!!
	// �ؽ�ó ���� ������ Resource ���� ���� �ٸ� ������ ������ �����Ű�, �� �̸��� PrototypeTag�� ��� TextureList�� Resource �������� �����Ŵϱ� ������ ���� �ɰŰ���!


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

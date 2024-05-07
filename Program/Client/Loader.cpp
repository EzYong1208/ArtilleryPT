#include "Loader.h"
#include "AObject.h"


Loader::Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: pDevice(pDevice)
	, pDeviceContext(pDeviceContext)
{
	// 다른 방법 없을까?
	//Safe_AddRef(pDevice);
	//Safe_AddRef(pDeviceContext);
}

unsigned int APIENTRY	ThreadFunction(void* pArg)
{
	//	Engine-Graphic_Device-InitDevice에서 장치에게 D3DCREATE_MULTITHREADED 전달을 꼭 해야 제대로 멀티스레드가 작동이 됨
	//	CLoader 객체의 주소를 가지고 있으면 좋을 것 같다
	Loader* pLoader = (Loader*)pArg;

	EnterCriticalSection(&pLoader->Get_CS());

	switch (pLoader->Get_LevelID())
	{
	case LEVEL_TEST:
		pLoader->LoadingTest();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CS());

	return 0;
}

HRESULT Loader::NativeConstruct(LEVEL eNextLevel)
{
	InitializeCriticalSection(&CS);

	hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadFunction, this, 0, nullptr);

	if (0 == hThread)
		return E_FAIL;

	eLevel = eNextLevel;

	return S_OK;
}

HRESULT Loader::LoadingTest()
{
	//if (FAILED(Add_Prototype_Textures()))
	//	return E_FAIL;

	//unique_ptr<AObject> AObject_ptr = new AObject();

	//AObject* AObject_ptr = new AObject();
	//AObject_ptr->Test();

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

Loader* Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel)
{
	Loader* pInstance = new Loader(pDevice, pDeviceContext);
	 
	//if (FAILED(pInstance->NativeConstruct(eNextLevel)))
	//{
	//	MSGBOX("Failed to Creating Loader");
	//	Safe_Release(pInstance);
	//}

	pInstance->NativeConstruct(eNextLevel);

	return pInstance;
}

void Loader::Release()
{
	// 다른 방법 없을까?
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	DeleteObject(hThread);
	DeleteCriticalSection(&CS);

	//Safe_Release(pDeviceContext);
	//Safe_Release(pDevice);
}

#pragma once

#include "AObject.h"
#include "AGameObject.h"

BEGIN(Engine)

class AGameObject;

enum class AGameObjectManagerError
{
	Success,
	Add_AGameObject_Failed,
	Find_AGameObject_Failed,
	MAX
};

class AGameObjectManager final : public AObject
{
public:
	// 링커 문제 고치기
	AGameObjectManager() {};

public:
	void Tick(_double TimeDelta)
	{
		auto iter = _AGameObjectMap.begin();
		while (iter != _AGameObjectMap.end())
		{
			iter->second->Tick(TimeDelta);

			if (iter->second &&
				iter->second->isAGameObjectDead())
			{
				iter = _AGameObjectMap.erase(iter);
			}
			else
				++iter;
		}
	}

	void LateTick()
	{
		auto iter = _AGameObjectMap.begin();
		while (iter != _AGameObjectMap.end())
		{
			iter->second->LateTick(iter->second.get());
			++iter;
		}
	}

public:
	AGameObjectManagerError Add_AGameObject(string TagName, _int ID, unique_ptr<AGameObject> GameObject)
	{
		_AGameObjectMap.emplace(make_pair(TagName, ID), move(GameObject));
		return AGameObjectManagerError::Success;
	}

	AGameObjectManagerError Find_AGameObject(string TagName, _int ID)
	{
		for (const auto& pair : _AGameObjectMap)
		{
			if (pair.first.first == TagName && pair.first.second == ID)
				return AGameObjectManagerError::Success;
		}

		return AGameObjectManagerError::Find_AGameObject_Failed;
	}

	map<pair<string, _int>, unique_ptr<AGameObject>>& Get_AGameObjectMap()
	{
		return _AGameObjectMap;
	}

private:
	map<pair<string, _int>, unique_ptr<AGameObject>> _AGameObjectMap;

};

END
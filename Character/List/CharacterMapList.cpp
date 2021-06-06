#include "../../pch.h"
#include "CharacterMapList.h"

#include "../Character/Character.h"

//エンティティを登録する関数
void CharacterMapList::RegisterEntity(Character* entity)
{
	//要素の追加(idをキーとして要素を追加する)
	m_pObjMapList.insert(std::make_pair(entity->GetID(), entity));
}

//エンティティを削除する関数
void CharacterMapList::RemoveEntity(Character* entity)
{
	m_pObjMapList.erase(m_pObjMapList.find(entity->GetID()));
}

//IDからエンティティを取得する関数
Character* CharacterMapList::GetEntityFromID(int id) const
{
	//マップの中をidで検索して要素(second)を渡す
	return m_pObjMapList.find(id)->second;
}

//全てのキーを取得する
std::vector<int>  CharacterMapList::GetAllEntityKey()const
{
	std::vector<int> allKey;

	for(const auto& itr : m_pObjMapList)
	{
		allKey.push_back(itr.first);
	}

	return allKey;
}

//リセット関数
void CharacterMapList::Reset()
{
	m_pObjMapList.clear();
}
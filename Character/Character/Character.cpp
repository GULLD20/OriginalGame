#include "../../pch.h"
#include "Character.h"

#include "../List/CharacterMapList.h"
#include "../../Scene/GameContext.h"

//1度
const float Character::ONE_DEGREE = DirectX::XM_PI / 180.0f;
//プレイヤーのタグ
const std::string Character::PLAYER_TAG = "player";
//エネミーのタグ
const std::string Character::ENEMY_TAG = "enemy";

int Character::m_nextID = 0;

Character::Character(int id, const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange, const State &state)
	:m_charaData{
	//キャラクターの位置
	pos,
	//回転角度
	DirectX::SimpleMath::Vector3::Zero,
	// プレイヤーのワールド行列
	DirectX::SimpleMath::Matrix::Identity,
	//プレイヤーの回転行列
	DirectX::SimpleMath::Matrix::Identity,
	//HP
	hp,
	//最大HP
	hp,
	//攻撃力
	attack,
	//速さ
	speed,
	//状態
	state,
	//当たり判定の範囲
	thisRange,
	//自分のカプセル(当たり判定用)
	Collision::Capsule3D{pos ,pos,thisRange},
	//キャラクターのID
	id,
}
{
	GameContext<CharacterMapList>::Get()->RegisterEntity(this);

	SetID(id);
}

Character::~Character()
{

}

void Character::SetID(int id)
{
	m_charaData.id = id;

	m_nextID = m_charaData.id + 1;
}
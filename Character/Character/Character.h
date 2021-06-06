#pragma once
#include <SimpleMath.h>
#include "../../StepTimer.h"
#include "../../MyLib/Collision.h"

class Character
{
public:
	enum State
	{
		Idle = 0,      //待機
		Move,          //移動
		Battle,        //戦闘
		Attack,        //攻撃
		Stop,          //停止
		HitDamage,     //ダメージ
		Avoidance,     //回避
		BlowAway,      //吹っ飛ばされている状態
		Deth           //HPがゼロ(自分以外がデータを参照するときに使う)
	};

	struct CharaacterData
	{
		//キャラクターの位置
		DirectX::SimpleMath::Vector3  pos;
		//回転角度
		DirectX::SimpleMath::Vector3  angle;
		// プレイヤーのワールド行列
		DirectX::SimpleMath::Matrix	  matrix;
		//プレイヤーの回転行列
		DirectX::SimpleMath::Matrix   rotation;
		//HP
		float                         hp;
		//最大HP
		float                         maxHp;
		//攻撃力
		float                         attack;
		//速さ
		float                         speed;
		//状態
		Character::State              state;
		//当たり判定の範囲
		float                         thisRange;
		//自分のカプセル(当たり判定用)
		Collision::Capsule3D          thisCapsule;
		//キャラクターのID
		int                           id;
	};

	//1度
	const static float                        ONE_DEGREE;
	//プレイヤーのタグ
	const static std::string                  PLAYER_TAG;
	//エネミーのタグ
	const static std::string                  ENEMY_TAG;

public:
	Character(int id, const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange , const State &state = State::Idle);
	virtual ~Character();

	//初期化処理
	virtual void Initialize() = 0;
	//更新処理
	virtual void Update(const DX::StepTimer &timer) = 0;
	//描画処理
	virtual void Draw(const DirectX::SimpleMath::Matrix &viewMatrix) = 0;
	//リセット処理
	virtual void Reset() = 0;

	//ダメージ処理
	virtual void Damage(float attack, const DirectX::SimpleMath::Matrix &rotation, bool blowAway, float power) = 0;
	//吹っ飛び処理
	virtual void BlowAwayMove() = 0;

public:
	//位置の取得・設定
	DirectX::SimpleMath::Vector3 GetPos()const { return m_charaData.pos; }
	void SetPos(const DirectX::SimpleMath::Vector3 &pos) { m_charaData.pos = pos; }

	//攻撃力の取得
	float GetAttack() const { return m_charaData.attack; }

	//体力の取得・設定
	float GetHP()const { return m_charaData.hp; }
	void SetHP(float hp) { m_charaData.hp = hp; }

	//速さの取得
	float GetSpeed() const { return m_charaData.speed; }

	//当たり判定の範囲の取得
	float GetThisRange() const { return m_charaData.thisRange; }

	//状態の取得
	State GetState()const { return m_charaData.state; }

	int GetID()const { return m_charaData.id; }
	void SetID(int id);

	Collision::Capsule3D GetCapsule() const { return m_charaData.thisCapsule; }
	void SetCapsule(const Collision::Capsule3D &capsule) { m_charaData.thisCapsule = capsule; };

	static int GetNextID() { return m_nextID; };

public:
	CharaacterData                  m_charaData;

	//無敵判定
	bool                            m_invincible;
	//無敵時間
	float                           m_invincibleTime;
	//攻撃が当たった判定
	bool                            m_hitAttack;
	//タグ
	std::string                     m_tag;

private:
	//次のID
	static int                      m_nextID;
};
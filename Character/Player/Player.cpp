#include "../../pch.h"

#include "../List/CharacterMapList.h"

#include "Weapon/Weapon.h"
#include "Weapon/WeaponType.h"

#include "Weapon/Sword/Sword.h"
#include "Weapon/Bow/Bow.h"
#include "Weapon/Spear/Spear.h"

#include "Player.h"

#include "../../DataManager/DataManager.h"
#include "../../Map/Map.h"
#include "../../DebugObj/DebugCapsule.h"
#include "../../MyLib/Collision.h"
#include "../../MyLib/MyLib.h"

#include "../../SE・BGM/Adx2.h"
#include "../../SE・BGM/acb/CueSheet_0.h"

//プレイヤーの大きさの上と下
const DirectX::SimpleMath::Vector3 Player::PLAYER_TOP_DOWN = DirectX::SimpleMath::Vector3(0.0f, 0.5f, 0.0f);

//回避時間
const float Player::AVOIDANCETIME = 0.15f;
//回避のクールタイム
const float Player::AVOIDANCECOOLTIME = 0.5f;
//吹っ飛ばされる時間
const float Player::BLOW_AWAY_TIME = 0.25f;
//無敵時間
const float Player::INVINCIBLE_TIME = 2.0f;

//エネミーを吹っ飛ばす力(1秒間に)
const float Player::BLOWAWAYPOWER_ENEMYTARGET = 40.0f;

Player::Player(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange)
	: Character(GetNextID(),pos,attack,hp,speed,thisRange)
	,m_pWeapon(nullptr)
	, m_pModel(nullptr)
	, m_pHPGauge(nullptr)
	//, m_pDebugCapsulePlayer(nullptr)
{
}


Player::~Player()
{
	if (m_pModel != nullptr)
	{
		m_pModel.reset(nullptr);
	}

	if (m_shadow != nullptr)
	{
		m_shadow.reset(nullptr);
	}

	if (m_pHPGauge != nullptr)
	{
		m_pHPGauge->Lost();
		m_pHPGauge.reset(nullptr);
	}

	/*if (m_pDebugCapsulePlayer != nullptr)
	{
		m_pDebugCapsulePlayer.reset(nullptr);
	}*/
}

void Player::Create(DX::DeviceResources* &deviceResources, const WeaponType &type)
{
	//武器の作成
	switch (type)
	{
	case WeaponType::Sword:
		m_pWeapon = std::make_unique<Sword>();
		break;

	case WeaponType::Bow:
		m_pWeapon = std::make_unique<Bow>();
		break;

	case WeaponType::Spear:
		m_pWeapon = std::make_unique<Spear>();
		break;

	default:
		break;
	}
	m_pWeapon->Create(deviceResources);

	// エフェクトファクトリの作成
	DirectX::EffectFactory * factory = new DirectX::EffectFactory(deviceResources->GetD3DDevice());
	// テクスチャの読み込みパス指定
	factory->SetDirectory(L"Resources/CMO");

	// ファイルを指定してモデルデータ読み込み
	m_pModel = DirectX::Model::CreateFromCMO(
		deviceResources->GetD3DDevice(),
		L"Resources/CMO/Character.cmo",
		*factory
	);

	//影のモデル
	m_shadow = DirectX::Model::CreateFromCMO(
		deviceResources->GetD3DDevice(),
		L"Resources/CMO/Shadow.cmo",
		*factory
	);

	delete factory;

	//HPゲージ
	m_pHPGauge = std::make_unique<HPGuage>();
	//エフェクトの個数と画像を指定(画像は使っていない)
	m_pHPGauge->Create(deviceResources,1);

	//m_pDebugCapsulePlayer = std::make_unique<DebugCapsule>(deviceResources->GetD3DDevice(), DirectX::SimpleMath::Vector3::Vector3::Zero, DirectX::SimpleMath::Vector3::Vector3::Zero, 0.5f);
}

//初期化処理
void Player::Initialize()
{
	m_charaData.pos = GetPos();
	m_charaData.angle = DirectX::SimpleMath::Vector3::Zero;
	m_charaData.hp = GetHP();

	//攻撃判定
	m_attack = false;

	//効果音
	m_playSE = false;

	//速度
	m_velocity = DirectX::SimpleMath::Vector3::Zero;
	//加速度
	m_acceleration = DirectX::SimpleMath::Vector3::Zero;
	//キー入力した方向のベクトル
	m_inputKeyVec = DirectX::SimpleMath::Vector3::Zero;

	//武器の初期化
	m_pWeapon->Initialize();
	//HPゲージの初期化
	m_pHPGauge->InitializeNormal(0.0f,DirectX::SimpleMath::Vector3::Zero);
	//最大のＨＰをエフェクト渡す
	m_pHPGauge->SetMaxHP(m_charaData.hp);
	//回避時間
	m_avoidance.TimeCount = 0.0f;
	//回避時の速さ
	m_avoidance.spead = GetSpeed()*3.0f;
	//回避のクールタイム
	m_avoidance.coolTimeCount = 0.0f;
	//回避をしたか判定
	m_avoidance.Check = false;
	//攻撃キーを入力
	m_attackKeyDown = false;
	//移動しているか判定
	m_moveCheck = false;

	//無敵判定の初期化
	m_invincible = false;
	//無敵時間の初期化
	m_invincibleTime = 0.0f;
	//無敵時間時のフレーム計算の初期化
	m_invincibleFrameCount = 0;
	//1フレームにかかった時間(合計すると1秒になる)
	m_elapsedTime = 0.0f;

	m_hitAttack = false;

	//1フレーム前のコンボ
	m_oldCombo = 0;

	//タグの設定
	m_tag = PLAYER_TAG;

	//吹っ飛ぶ方向の角度(行列)
	m_blowAwayRotation = DirectX::SimpleMath::Matrix::Identity;
	//吹っ飛ぶ力(1秒間)
	m_blowAwayPower = 0.0f;
	//吹っ飛び時間の計測
	m_blowAwayTime = 0.0f;
}

//更新処理
void Player::Update(const DX::StepTimer &timer)
{
	//1フレームに掛かった時間を設定
	m_elapsedTime = static_cast<float>(timer.GetElapsedSeconds());

	//空中に行かないために
	m_charaData.pos.y = 0.0f;

	//武器の更新処理
	//更新
	m_charaData.matrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_charaData.pos);

	//現在のHPをエフェクトに渡す
	m_pHPGauge->SetNowHP(m_charaData.hp);
	//ＨＰエフェクトの更新処理
	m_pHPGauge->Update(timer);

	//m_pDebugCapsulePlayer->SetCenterSegment(m_charaData + DirectX::SimpleMath::Vector3(0.0f, 0.8f, 0.0f), m_charaData - DirectX::SimpleMath::Vector3(0.0f, 0.8f, 0.0f));

	//キー入力
	InputKey();

	//移動処理
	Move();

	//プレイヤーの回転
	Rotation();


	//攻撃処理
	Attack();

	//回避の更新処理
	AvoidanceUpdate();

	//吹っ飛び処理
	BlowAwayMove();

	//無敵時間の更新処理
	InvincibleUpdate();

	Collision::Box3D player
	{
		m_charaData.pos,
		DirectX::SimpleMath::Vector3(GetThisRange(),1.0f,GetThisRange())
	};

	//マップとの処理
	Map* map = GameContext<Map>::Get();
	//壁に当たっているか
	m_charaData.pos = map->HitWallBox(player);
	//移動限界まで行っているか
	m_charaData.pos = map->Limit(m_charaData.pos, m_charaData.thisRange);
	//移動限界のエフェクト
	map->LimitEffect(m_charaData.pos, m_charaData.thisRange);

	m_pWeapon->Update(m_elapsedTime);

	//プレイヤーの自分の当たり判定の設定
	m_charaData.thisCapsule =
	{
		m_charaData.pos + PLAYER_TOP_DOWN ,
		m_charaData.pos - PLAYER_TOP_DOWN ,
		m_charaData.thisRange,
	};
}

//描画処理
void Player::Draw(const DirectX::SimpleMath::Matrix &viewMatrix)
{
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();
	ID3D11DeviceContext1* deviceContext = deviceResources->GetD3DDeviceContext();
	DirectX::CommonStates* CommonState = GameContext<DirectX::CommonStates>().Get();
	Projection* projection = GameContext<Projection>().Get();

	// モデル描画
	//影
	DirectX::SimpleMath::Matrix shadowMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(m_charaData.pos + DirectX::SimpleMath::Vector3(0.0f,-1.0f,0.0f));
	m_shadow->Draw(deviceContext, *CommonState, shadowMatrix, viewMatrix, projection->GetMatrix());

	//無敵時間の状態の時の描画処理
	if (m_invincible)
	{
		if (m_invincibleFrameCount % 2 == 0)
		{
			m_pModel->Draw(deviceContext, *CommonState, m_charaData.matrix, viewMatrix, projection->GetMatrix());
		}
	}
	else
	{
		//通常の状態の描画処理
		m_pModel->Draw(deviceContext, *CommonState, m_charaData.matrix, viewMatrix, projection->GetMatrix());
	}

	//武器の 描画処理
	m_pWeapon->Draw(deviceContext, *CommonState, viewMatrix, projection->GetMatrix());

	//HPゲージの描画
	m_pHPGauge->SetRenderState(DirectX::SimpleMath::Vector3::One,viewMatrix, projection->GetMatrix());
	m_pHPGauge->Render();
}

//リセット処理
void Player::Reset()
{
	//武器のリセット処理
	m_pWeapon->Reset();
}

//ダメージ処理
void Player::Damage(float attack, const DirectX::SimpleMath::Matrix &rotation, bool blowAway, float power)
{
	//無敵状態の時はダメージ処理をしない
	if (m_invincible) return;

	//体力を攻撃力分減らす
	m_charaData.hp -= attack;

	//受けたダメージをエフェクトに渡す
	m_pHPGauge->HitDamage(attack);

	//無敵時間状態に変更
	m_invincible = true;

	//吹っ飛び処理に必要なデータを設定
	m_blowAwayRotation = rotation;
	m_blowAwayPower = power;

	m_charaData.state = State::BlowAway;
}


//無敵時間の更新処理
void Player::InvincibleUpdate()
{
	if (m_invincible)
	{
		//無敵時間終了処理
		if (m_invincibleTime >= INVINCIBLE_TIME)
		{
			m_invincible = false;
			m_invincibleFrameCount = 0;
			m_invincibleTime = 0.0f;
		}
		//無敵時間の計算
		m_invincibleTime += m_elapsedTime;
		//無敵時間のフレーム計算
		m_invincibleFrameCount++;
	}
}

//角度の制限
DirectX::SimpleMath::Vector3 Player::LimitsAngle(const DirectX::SimpleMath::Vector3 &angle)
{
	DirectX::SimpleMath::Vector3 retAngle = angle;

	if (retAngle.x >= DirectX::XM_PI * 2)
	{
		retAngle.x = 0.0f;
	}
	else if (retAngle.x <= DirectX::XM_PI * -2)
	{
		retAngle.x = 0.0f;
	}

	if (retAngle.y >= DirectX::XM_PI * 2)
	{
		retAngle.y = 0.0f;
	}
	else if (retAngle.y <= DirectX::XM_PI * -2)
	{
		retAngle.y = 0.0f;
	}
	if (retAngle.z >= DirectX::XM_PI * 2)
	{
		retAngle.z = 0.0f;
	}
	else if (angle.z <= DirectX::XM_PI * -2)
	{
		retAngle.z = 0.0f;
	}

	return retAngle;
}

















void Player::Move()
{
	//// ビークルに掛かる操舵力
	//// ビークルのリスト内の各操舵行動から合力を計算する
	//DirectX::SimpleMath::Vector3 steeringForce = m_steering->Calculate();

	//// 加速度 = 力 ÷ 質量
	//DirectX::SimpleMath::Vector3 acceleration = steeringForce / GetMass();

	//// 速度を更新する（速度に加速度を足す）
	//DirectX::SimpleMath::Vector3 velocity = GetVelocity() + acceleration * elapsedTime;

	//// ビークルが最大速度を超えないようにする
	//if (velocity.Length() > GetMaxSpeed())
	//{
	//	velocity.Normalize();
	//	velocity *= GetMaxSpeed();
	//}

	//// 更新した速度を設定する
	//SetVelocity(velocity);

	//// 位置の更新する
	//SetPos(GetPos() + GetVelocity() * elapsedTime);




	//加速度を求める
	if (m_charaData.state == State::Move)
	{
		//加速度を求める
		m_acceleration += (m_inputKeyVec);
		m_acceleration.Normalize();

		//速度を求める
		m_velocity += m_acceleration* m_elapsedTime;

		//最大速度
		if (m_velocity.Length() >= 0.1f)
		{
			//
			m_velocity.Normalize();

			m_velocity *= 0.1f;
		}
	}
	else
	{
		//速度、加速度を減少
		m_acceleration *= 0.5f;
		m_velocity *= 0.5f;

		//停止
		if (m_velocity.Length() <= 0.001f)
		{
			m_acceleration = DirectX::SimpleMath::Vector3::Zero;
			m_velocity = DirectX::SimpleMath::Vector3::Zero;
		}
	}

	//移動処理
	m_charaData.pos += m_velocity;
}

//攻撃処理
void Player::Attack()
{
	if (m_charaData.state == State::Attack)
	{
		//攻撃開始
		m_pWeapon->StartAttack();
		//攻撃が当たったかの判定を初期化
		m_hitAttack = false;

		//攻撃の効果音の再生
		PlayAttackSE();

		//キャラクターのデータを取得
		CharacterMapList* list = GameContext<CharacterMapList>::Get();

		std::vector<int> key = list->GetAllEntityKey();

		//正面に向けたベクトル
		DirectX::SimpleMath::Vector3 frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, m_charaData.rotation);
		frontVec.Normalize();

		//視野内の一番近い敵の位置を取得
		DirectX::SimpleMath::Vector3 enemyPos =  EnemyDistanceClosePos(key,frontVec);

		//移動処理
		m_charaData.pos += (CalAttackMoveVel(frontVec, enemyPos)* m_elapsedTime);
	}
}

//攻撃の効果音の再生
void Player::PlayAttackSE()
{
	//SEが再生されていないなら
	if (!m_playSE)
	{
		//SEの再生
		GameContext<ADX2>::Get()->Play(CRI_CUESHEET_0_SLASH);
		//SE再生判定
		m_playSE = true;
	}
}

//攻撃時の移動速度を求める
DirectX::SimpleMath::Vector3 Player::CalAttackMoveVel(const DirectX::SimpleMath::Vector3 &front, const DirectX::SimpleMath::Vector3 &enemyPos)
{
	//キャラクターのデータを取得
	CharacterMapList* list = GameContext<CharacterMapList>::Get();

	//プレイヤーから敵に向けたベクトル
	DirectX::SimpleMath::Vector3 pToEVec = enemyPos - GetPos();
	//正規化
	pToEVec.Normalize();

	//プレイヤーの向いている角度を調整
	float cross = (front.x * pToEVec.z) - (front.z * pToEVec.x);
	m_charaData.angle.y = cross;

	//敵に向かせる
	DirectX::SimpleMath::Matrix matrix = MyLib::LockAt(GetPos(), enemyPos, DirectX::SimpleMath::Vector3::Forward);

	//移動速度
	DirectX::SimpleMath::Vector3 vel= DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, matrix) * 0.1f;

	return vel;
}

//自分の正面にいる敵の中で一番近い敵の位置を取得
DirectX::SimpleMath::Vector3 Player::EnemyDistanceClosePos(std::vector<int> allKey, const DirectX::SimpleMath::Vector3 &frontVecN)
{
	//キャラクターのデータを取得
	CharacterMapList* list = GameContext<CharacterMapList>::Get();

	//視野内の一番近い敵の位置
	DirectX::SimpleMath::Vector3 retPos = frontVecN;

	for (int i = 0; i < static_cast<int>(allKey.size()); i++)
	{
		//自分のデータは必要ないため
		if (allKey[i] == GetID())
		{
			continue;
		}

		//自分から敵に向けたベクトル
		DirectX::SimpleMath::Vector3 player_enemy_vec = list->GetEntityFromID(i)->GetPos() - m_charaData.pos;

		//正規化
		player_enemy_vec.Normalize();

		//プレイヤーの視界内に敵がいるか確認
		float dot = frontVecN.Dot(player_enemy_vec);
		//1.0fを超えないよに
		if (dot >= 1.0f)
		{
			dot = 1.0f;
		}

		//なす角の取得
		float angle = std::acos(dot);

		//視野内にいるかどうか（視野角 = 30度）
		if (angle <= ONE_DEGREE* 30.0f)
		{
			retPos = list->GetEntityFromID(i)->GetPos();
		}
	}

	//視野内に敵がいない場合正面を返す
	return retPos;
}

//回避のUpdate処理(Updateで呼び出す処理	)
void Player::AvoidanceUpdate()
{
	if (m_charaData.state == State::Avoidance)
	{
		//回避時の移動処理
		AvoidanceMove();
		//回避状態
		m_avoidance.Check = true;
		//回避時間の計測
		m_avoidance.TimeCount += m_elapsedTime;

		//回避の終了判定
		if (m_avoidance.TimeCount >= AVOIDANCETIME)
		{
			//もう一度回避ができるまでの時間
			m_avoidance.coolTimeCount = AVOIDANCECOOLTIME;
			//状態の初期化
			m_charaData.state = State::Idle;
			//回避時間を計測する変数の初期化
			m_avoidance.TimeCount = 0.0f;
			//回避終了判定
			m_avoidance.Check = false;
		}
	}

	//回避していないなら
	if (!m_avoidance.Check)
	{
		//回避がもう一度使えるか判定
		m_avoidance.coolTimeCount -= m_elapsedTime;
		if (m_avoidance.coolTimeCount < 0.0f)
		{
			m_avoidance.coolTimeCount = 0.0f;
		}
	}
}

//回避の移動処理
void Player::AvoidanceMove()
{
	//移動方向と速度を決める
	DirectX::SimpleMath::Vector3 vec;

	if (m_moveCheck)
	{
		vec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, (m_charaData.rotation)) * m_avoidance.spead;
	}
	else
	{
		vec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, (m_charaData.rotation)) *m_avoidance.spead;
	}

	//移動処理
	m_charaData.pos += (vec*m_elapsedTime);
}

//ジャスト回避
void Player::JusrAvoidance()
{

}

//攻撃が当たった処理
void Player::HitAttack(int id)
{
	CharacterMapList* list = GameContext<CharacterMapList>::Get();
	//敵のHPがゼロのときは処理をしない
	if (list->GetEntityFromID(id)->GetState() == State::Deth)
	{
		return;
	}

	//攻撃状態じゃないとき
	if (!m_attack)
	{
		m_hitAttack = false;
		return;
	}

	//コンボの修正
	if (m_oldCombo != m_pWeapon->GetCombo())
	{
		m_oldCombo = m_pWeapon->GetCombo();
		m_hitAttack = false;
	}

	//攻撃後数フレームの間
	if (!m_pWeapon->GetAttackSTate())
	{
		//SEの再生を停止する
		GameContext<ADX2>::Get()->Stop(CRI_CUESHEET_0_SLASH);
		m_playSE = false;
		return;
	}

	//上下の位置調整用にyを0.5fを足し引きする
	Collision::Capsule3D enemy =
	{
		list->GetEntityFromID(id)->GetPos() + DirectX::SimpleMath::Vector3(0.0f,0.5f,0.0f),
		list->GetEntityFromID(id)->GetPos() - DirectX::SimpleMath::Vector3(0.0f,0.5f,0.0f),
		list->GetEntityFromID(id)->GetThisRange(),
	};

	//武器の位置を取得
	DirectX::SimpleMath::Vector3 weaponPos = m_pWeapon->GetPos();
	//武器のベクトル(原点から先端に向けた)を取得
	DirectX::SimpleMath::Vector3 weaponVector = m_pWeapon->GetVector();
	//武器のベクトルにプレイヤーの回転をかける
	weaponVector = weaponVector.Transform(weaponVector, m_charaData.rotation);

	//武器の当たり判定
	Collision::Capsule3D weapon =
	{
		weaponPos + weaponVector ,
		weaponPos - weaponVector ,
		0.1f
	};

	//攻撃に当たっているか
	if (Collision::HitCheck_Capsule2Capsule(enemy, weapon))
	{
		//コンボ中攻撃が当たっていないなら
		if (!m_hitAttack)
		{
			//吹っ飛び判定
			bool blowAway = false;
			//最大コンボ数が3のため
			if (m_pWeapon->GetCombo() == 3)
			{
				blowAway = true;
			}
			//テスト用に10倍
			list->GetEntityFromID(id)->Damage(m_charaData.attack*10, m_charaData.rotation, blowAway, BLOWAWAYPOWER_ENEMYTARGET);
			m_hitAttack = true;
		}
	}
}


//エネミーと当たっているとき
void Player::HItEnemy(int id)
{

	CharacterMapList* list = GameContext<CharacterMapList>::Get();

	//敵のHPがゼロのときは処理をしない
	if (list->GetEntityFromID(id)->GetState() == State::Deth)
	{
		return;
	}

	float player_enemy_length = (list->GetEntityFromID(id)->GetPos() - m_charaData.pos).Length();

	float range = list->GetEntityFromID(id)->GetThisRange() + GetThisRange();

	//敵に当たっているか
	if (player_enemy_length <= range)
	{
		//敵から一定距離離す
		DirectX::SimpleMath::Vector3 vec =
			DirectX::SimpleMath::Vector3::Transform(
				DirectX::SimpleMath::Vector3::Forward,
				m_charaData.rotation)*
				(GetThisRange() + list->GetEntityFromID(id)->GetThisRange() + (GetSpeed()*m_elapsedTime));

		m_charaData.pos = list->GetEntityFromID(id)->GetPos() - vec;
	}

}

//吹っ飛び処理
void Player::BlowAwayMove()
{
	if (m_charaData.state == State::BlowAway)
	{
		//ノックバック(m_blowAwayPowerの数値分1秒換算で進む)
		DirectX::SimpleMath::Vector3 vec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, m_blowAwayRotation) * m_blowAwayPower;

		//移動処理
		m_charaData.pos += (vec*m_elapsedTime);

		if (m_blowAwayTime >= BLOW_AWAY_TIME)
		{
			m_charaData.state = State::Idle;
			m_blowAwayRotation = DirectX::SimpleMath::Matrix::Identity;
			m_blowAwayPower = 0.0f;
			m_blowAwayTime = 0.0f;
		}
		else
		{
			m_blowAwayTime += m_elapsedTime;
		}
	}
}

//プレイヤーの回転
void Player::Rotation()
{
	if (GetState() == State::Move)
	{
		DirectX::SimpleMath::Vector3 vector = m_velocity;

		vector.Normalize();
		float dot = vector.Dot(DirectX::SimpleMath::Vector3::Forward);
		if (dot > 1.0f)
		{
			dot = 1.0f;
		}

		//角度取得 0～180度
		float angle = std::acos(dot);

		//左右キーのどちらを入力しているか
		if (m_inputKeyVec.x > 0.0f)
		{
			m_charaData.angle.y = -angle;
		}
		else if (m_inputKeyVec.x <= 0.0f)
		{
			m_charaData.angle.y = angle;
		}
	}
}

//Matrixの設定
void Player::MatrixSetting()
{
	m_charaData.matrix = DirectX::SimpleMath::Matrix::CreateRotationY(m_charaData.angle.y);
	m_charaData.matrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_charaData.pos);

	m_charaData.rotation = DirectX::SimpleMath::Matrix::CreateRotationY(m_charaData.angle.y);

	m_pWeapon->SetPlayerMatrix(m_charaData.matrix);
	m_pWeapon->MatrixSetting();
}

//キー入力
void Player::InputKey()
{
	DirectX::Keyboard::State state = DirectX::Keyboard::Get().GetState();

	if (m_charaData.state == State::Avoidance)
	{
		return;
	}

	if (m_charaData.state == State::BlowAway)
	{
		return;
	}

	if (m_pWeapon->GetAttackSTate())
	{
		return;
	}

	m_charaData.state = State::Idle;
	m_inputKeyVec = DirectX::SimpleMath::Vector3::Zero;

	if (!state.W && !state.A && !state.S && !state.D)
	{
		//移動停止判定
		m_moveCheck = false;
	}

	//キー入力  移動
	if (state.W)
	{
		//移動判定
		m_moveCheck = true;
		//入力した方向へのベクトル
		m_inputKeyVec += DirectX::SimpleMath::Vector3::Forward;
		//移動状態
		m_charaData.state = State::Move;
	}

	if (state.D)
	{
		//移動判定
		m_moveCheck = true;
		//入力した方向へのベクトル
		m_inputKeyVec += DirectX::SimpleMath::Vector3::Right;
		//移動状態
		m_charaData.state = State::Move;
	}

	if (state.A)
	{
		//移動判定
		m_moveCheck = true;
		//入力した方向へのベクトル
		m_inputKeyVec += DirectX::SimpleMath::Vector3::Left;
		//移動状態
		m_charaData.state = State::Move;
	}

	if (state.S)
	{
		//移動判定
		m_moveCheck = true;
		//入力した方向へのベクトル
		m_inputKeyVec += DirectX::SimpleMath::Vector3::Backward;
		//移動状態
		m_charaData.state = State::Move;
	}

	//攻撃
	if (state.C && !m_attackKeyDown)
	{
		m_attack = true;
		m_charaData.state = State::Attack;
		m_attackKeyDown = true;
	}
	else if(!state.C)
	{
		m_attackKeyDown = false;
	}

	//回避
	if (state.Space)
	{
		if (m_avoidance.coolTimeCount <= 0.0f)
		{
			m_charaData.state = State::Avoidance;
		}
	}
}
#include "../../../pch.h"

#include "LongRange.h"

#include "../../DataManager/DataManager.h"
#include "../../List/CharacterMapList.h"

#include "../../Map/Map.h"

//攻撃時間
const float LongRange::ATTACK_TIME = 1.0f;
//攻撃範囲
const float LongRange::ATTACKRANGE = 20.0f;
//攻撃間隔
const int LongRange::ATTACK_INTERVAL = 2;
//戦闘範囲
const float LongRange::BATTLE_RANGE = 20.0f;
//弾の攻撃速度(1秒間の速度)
const float LongRange::ATTACK_SPEED = 36.0f;

//プレイヤーを吹っ飛ばす力(1秒間に)
const float LongRange::BLOW_AWAY_POWER_PLAYER_TARGET = 5.0f;

//エフェクトの個数
//攻撃範囲
const int LongRange::NUMBER_OF_EFFECT_ATTACK_AREA = 1;
//攻撃が当たった時
const int LongRange::NUMBER_OF_EFFECT_HIT = 25;
//死亡
const int LongRange::NUMBER_OF_EFFECT_DETH = 50;

//スロウ状態の計算用の数値
const float LongRange::SLOW_CAL = 0.025f;
//通常状態の計算用の数値
const float LongRange::NORMAL_CAL = 1.0f;

LongRange::LongRange(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange)
	:Enemy(pos, attack, hp, speed, thisRange)
{
}


LongRange::~LongRange()
{
	if (m_pAttackLine != nullptr)
	{
		m_pAttackLine->Lost();
		m_pAttackLine.reset(nullptr);
	}

	if (m_pLongRangeAttack != nullptr)
	{
		m_pLongRangeAttack.reset(nullptr);
	}
}

//エフェクトの作成
void LongRange::CreateEffect()
{
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();

	//HPゲージの作成
	m_pHPGuage = std::make_unique<Effect3D>();
	//画像の設定
	m_pHPGuage->Create(deviceResources, L"Resources/Textures/HPGuage/Player/HP.png");

	//名前の作成
	m_pName = std::make_unique<Effect3D>();
	//画像の設定
	m_pName->Create(deviceResources, L"Resources/Textures/Enemies/ゴブリン.png");

	//攻撃範囲の作成
	m_pAttackLine = std::make_unique<AttackLine>();
	//画像とエフェクトの作成数の設定
	m_pAttackLine->Create(deviceResources, NUMBER_OF_EFFECT_ATTACK_AREA);

	//攻撃が当たった時の演出(プレイヤーの攻撃がエネミーに当たったら)の作成
	m_pEnemyHit = std::make_unique<EnemyHit>();
	m_pEnemyHit->Create(deviceResources, NUMBER_OF_EFFECT_HIT);

	//死亡演出の作成
	m_pEnemyDeth = std::make_unique<EnemyDeth>();
	//画像とエフェクトの作成数の設定
	m_pEnemyDeth->Create(deviceResources, NUMBER_OF_EFFECT_DETH);
}

//派生クラス用更新処理
void LongRange::SubInitialize()
{
	m_pLongRangeAttack = std::make_unique<LongRangeAttack>();
	//攻撃のモデル作成
	m_pLongRangeAttack->CreatModel();
	//攻撃の初期化処理
	m_pLongRangeAttack->Initialize(ATTACK_SPEED,static_cast<int>(BATTLE_RANGE));
	//回転角度の初期化
	m_angle = 360.0f;
	//エネミーの吹っ飛び判定の初期化
	m_blowAway = false;
}

//エフェクトの初期化
void LongRange::EffectInitialize()
{
	//HPの画像の大きさの設定
	m_pHPGuage->SetTextureScaleTL(DirectX::SimpleMath::Vector2(1.0f, 0.06f));
	m_pHPGuage->SetTextureScaleBR(DirectX::SimpleMath::Vector2(-1.0f, -0.06f));
	//HPの画像の切り取り位置を設定
	m_pHPGuage->SetTextureSizeTL(DirectX::SimpleMath::Vector2(0.0f, 0.0f));
	m_pHPGuage->SetTextureSizeBR(DirectX::SimpleMath::Vector2(1.0f, 1.0f));

	//名前の画像の大きさの設定
	m_pName->SetTextureScaleTL(DirectX::SimpleMath::Vector2(0.60f, 0.15f));
	m_pName->SetTextureScaleBR(DirectX::SimpleMath::Vector2(-0.60f, -0.15f));
	//名前の画像の切り取り位置を設定
	m_pName->SetTextureSizeTL(DirectX::SimpleMath::Vector2(0.0f, 0.0f));
	m_pName->SetTextureSizeBR(DirectX::SimpleMath::Vector2(1.0f, 0.5f));

	m_pAttackLine->InitializeNormal(5, DirectX::SimpleMath::Vector3(0, 0, 0));
	DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3(0.15f, 1.0f, (ATTACKRANGE * 0.5f));

	m_pAttackLine->SetScale(scale);

	//攻撃が当たった時の演出(プレイヤーの攻撃がエネミーに当たったら)の初期化
	m_pEnemyHit->InitializeNormal(1, DirectX::SimpleMath::Vector3::Zero);
	m_pEnemyHit->SetScale(DirectX::SimpleMath::Vector3(0.2f, 0.2f, 0.2f));

	//死亡演出の初期化
	m_pEnemyDeth->InitializeCorn(10.0f, DirectX::SimpleMath::Vector3(0.0f, -0.25f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.1f, 0.0f));
	m_pEnemyDeth->SetScale(DirectX::SimpleMath::Vector3(0.2f, 0.2f, 0.2f));
}


//各Update関数-----------------
//派生クラス用更新処理
void LongRange::SubUpdate()
{
	//攻撃(弾)の更新処理
	m_pLongRangeAttack->SetSpeedCal(m_speedCal);
	m_pLongRangeAttack->Update(m_elapsedTime);
	HitAttack();
}

//エフェクトの更新処理
void LongRange::EffectUpdate(const DX::StepTimer &timer)
{
	if (DethUpdate(timer))
	{
		return;
	}

	HPTextureSize();
	//HPゲージの更新
	m_pHPGuage->RotationY(m_charaData.pos + DirectX::SimpleMath::Vector3(0.0f, 0.5f, 0.0f), m_eyePos);
	m_pName->RotationY(m_charaData.pos + DirectX::SimpleMath::Vector3(0.0f, 0.7f, 0.0f), m_eyePos);

	m_pAttackLine->Update(timer);

	if (m_charaData.state == State::Battle)
	{
		m_pAttackLine->SetPos(m_charaData.pos
			+ DirectX::SimpleMath::Vector3::Transform(
				DirectX::SimpleMath::Vector3::Forward*(ATTACKRANGE * 0.5f)
				, m_charaData.rotation));
	}

	m_pAttackLine->SetMainMatrix(m_charaData.rotation);

	//透明なら更新しない
	if (m_hitEffectAlpha > 0.0f)
	{
		m_hitEffectAlpha -= m_elapsedTime;
		m_pEnemyHit->Update(timer);
	}

	//画像の切り取り位置を設定
	if (!m_discovery)
	{
		m_pName->SetTextureSizeTL(DirectX::SimpleMath::Vector2(0.0f, 0.0f));
		m_pName->SetTextureSizeBR(DirectX::SimpleMath::Vector2(1.0f, 0.5f));
	}
	else
	{
		m_pName->SetTextureSizeTL(DirectX::SimpleMath::Vector2(0.0f, 0.5f));
		m_pName->SetTextureSizeBR(DirectX::SimpleMath::Vector2(1.0f, 1.0f));
	}
}

//-----------------------------

//各描画処理-------------------
//生存時の描画処理
void LongRange::AliveDraw(const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &projection)
{
	//描画に使うデータを取得
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();
	ID3D11DeviceContext1* deviceContext = deviceResources->GetD3DDeviceContext();
	DirectX::CommonStates* CommonState = GameContext<DirectX::CommonStates>().Get();

	// モデル描画
	//影
	DirectX::SimpleMath::Matrix shadowMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(m_charaData.pos + DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
	m_shadow->Draw(deviceContext, *CommonState, shadowMatrix, viewMatrix, projection);

	//キャラクター
	m_pModel->Draw(deviceContext, *CommonState, m_charaData.matrix, viewMatrix, projection);

	//攻撃の描画処理
	m_pLongRangeAttack->Draw(viewMatrix, projection);

	//HPゲージの表示
	m_pHPGuage->SetRenderState(viewMatrix.Translation(), viewMatrix, projection);
	m_pHPGuage->Render();

	//名前の表示
	m_pName->SetRenderState(viewMatrix.Translation(), viewMatrix, projection);
	m_pName->Render();

	//攻撃が当たった時のエフェクトの描画
	if (m_hitEffectAlpha > 0.0f)
	{
		m_pEnemyHit->SetRenderState(m_eyePos, viewMatrix, projection);
		m_pEnemyHit->Render();
	}

	//戦闘中
	if (m_charaData.state == State::Battle || m_charaData.state == State::Attack)
	{
		//攻撃範囲の表示
		m_pAttackLine->SetRenderState(m_eyePos, viewMatrix, projection);
		m_pAttackLine->Render();
	}
}

//戦闘中処理
void LongRange::Battle()
{
	//攻撃開始判定
	if (m_attackInterval >= ATTACK_INTERVAL)
	{
		//攻撃開始処理
		m_charaData.state = State::Attack;
		m_hitAttack = false;
	}
	else
	{
		//攻撃開始までの時間計算
		//m_speedCalはスロウ状態の時と通常状態の時の時間の計算速度を変えるため
		m_attackInterval += m_elapsedTime* m_speedCal;
		m_pAttackLine->SetTimming(m_attackInterval / ATTACK_INTERVAL);
	}
}

//攻撃処理
void LongRange::Attack()
{
	//弾の発射処理
	m_pLongRangeAttack->Shot(m_charaData.pos, m_charaData.rotation);
	EndAttack();
}

//停止処理
void LongRange::Stop()
{
	if (!m_alive) return;
}

//移動判定
void LongRange::SelectMove()
{
	//キャラクターのデータを保持しているリストを取得
	CharacterMapList* list = GameContext<CharacterMapList>::Get();
	//プレイヤーとエネミーの間の距離
	float length = (list->GetEntityFromID(m_playerID)->GetPos() - m_charaData.pos).Length();

	//攻撃範囲外
	if (length > BATTLE_RANGE)
	{
		if (m_charaData.state == State::Battle)
		{
			EndAttack();
		}
		m_charaData.state = State::Move;
	}
}

//戦闘開始判定
void LongRange::SelectBattle()
{
	//キャラクターのデータを保持しているリストを取得
	CharacterMapList* list = GameContext<CharacterMapList>::Get();
	//プレイヤーとエネミーの間の距離
	float length = (list->GetEntityFromID(m_playerID)->GetPos() - m_charaData.pos).Length();

	//攻撃範囲内
	if (length <= BATTLE_RANGE)
	{
		//エネミーからターゲットに向けたベクトル
		DirectX::SimpleMath::Vector3 enemy_target_vec = list->GetEntityFromID(m_playerID)->GetPos() - m_charaData.pos;

		//エネミーの正面に向けたベクトル
		DirectX::SimpleMath::Vector3 frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, m_charaData.rotation);

		//正規化
		enemy_target_vec.Normalize();
		frontVec.Normalize();

		//内積を求める
		float dot = enemy_target_vec.Dot(frontVec);

		//エラー対策(内積の最大は1だからそれを超えないように)
		if (dot > 1.0f)
		{
			dot = 1.0f;
		}

		//なす角
		float angle = std::acos(dot);

		//敵の正面の6度以内のにプレイヤーがいたら戦闘状態(正面を中心として6度以内にするために3をかける)
		if (angle <= (ONE_DEGREE * 3.0f))
		{
			m_charaData.state = State::Battle;
		}
	}
}

//攻撃開始判定
void LongRange::SelectAttack()
{
	if (m_charaData.state == State::Battle)
	{
		if (m_attackInterval >= ATTACK_INTERVAL)
		{
			m_charaData.state = State::Attack;
		}
		else
		{
			m_attackInterval += m_elapsedTime;
		}
	}
}

//停止判定
void LongRange::SelectStop()
{

}

//プレイヤーのジャスト回避判定
void LongRange::PlayerJustAvoidance()
{
	if (!m_alive) return;

	if (m_justAvoidance) return;

	//キャラクターのデータを保持しているリストを取得
	CharacterMapList* list = GameContext<CharacterMapList>::Get();

	//エネミーが攻撃状態の時
	//プレイヤーが回避状態の時
	if (list->GetEntityFromID(m_playerID)->GetState() == State::Avoidance)
	{
		//プレイヤーの当たり判定の設定
		Collision::Box3D player =
		{
 			list->GetEntityFromID(m_playerID)->GetPos(),
			DirectX::SimpleMath::Vector3(0.5f,1.0f,0.5f),
		};

		//弾の中で一番プレイヤーに近い弾の当たり判定のデータを取得
		Collision::Capsule3D enemyCapsuleData = m_pLongRangeAttack->GetCapsuleData(list->GetEntityFromID(m_playerID)->GetPos());
		//取得した弾の位置を取得
		DirectX::SimpleMath::Vector3 enemyCapsulePos = enemyCapsuleData.startPos + ((enemyCapsuleData.startPos - enemyCapsuleData.endPos)*0.5f);

		//攻撃範囲の当たり判定の設定(ジャスト回避の範囲)
		//本来の大きさより大きくするため5倍する
		Collision::Box3D enemyAttack =
		{
			enemyCapsulePos,
			DirectX::SimpleMath::Vector3(
				std::abs(enemyCapsuleData.startPos.x - enemyCapsuleData.endPos.x),
				std::abs(enemyCapsuleData.range),
				std::abs(enemyCapsuleData.startPos.z - enemyCapsuleData.endPos.z)
			)*5.0f,
		};

		//当たり判定の大きさが0だったら
		if (enemyAttack.size.x <= 0.1f)
		{
			enemyAttack.size.x = 0.5f;
		}

		if (enemyAttack.size.z <= 0.1f)
		{
			enemyAttack.size.z = 0.5f;
		}

		//当たり判定
		//プレイヤーと攻撃範囲
		if (Collision::HitCheckoxBox3D(player, enemyAttack))
		{
			//スロウ状態に変更
			StartSlow();
			for (int i = 0; i < static_cast<int>(m_pOtherEnemyList.size()); i++)
			{
				//他の敵のスロウ状態を開始する
				m_pOtherEnemyList[i]->StartSlow();
			}
		}
	}
}

void LongRange::EndAttack()
{
	m_attack_start_end = false;
	m_attackTime = 0.0f;
	m_attackInterval = 0.0f;
	m_charaData.state = State::Idle;
}

//攻撃の当たり判定
void LongRange::HitAttack()
{
	//キャラクターのデータを保持しているリストを取得
	CharacterMapList* list = GameContext<CharacterMapList>::Get();

	//当たり判定
	//プレイヤーと弾(プレイヤーに最も近い弾)
	if (Collision::HitCheck_Capsule2Capsule(
		list->GetEntityFromID(m_playerID)->GetCapsule(),
		m_pLongRangeAttack->GetCapsuleData(list->GetEntityFromID(m_playerID)->GetPos())
	))
	{
		//攻撃中一回も攻撃が当たっていなかったら
		//if (!m_hitAttack)
		{
			//弾の停止
			m_pLongRangeAttack->HitAttack();
			//プレイヤーのダメージ処理
			list->GetEntityFromID(m_playerID)->Damage(m_charaData.attack, m_charaData.rotation, true, BLOW_AWAY_POWER_PLAYER_TARGET);
			//攻撃が当たった判定
			m_hitAttack = true;
		}
	}
}
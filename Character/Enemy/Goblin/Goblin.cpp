#include "../../../pch.h"


#include "Goblin.h"

#include "../../DataManager/DataManager.h"
#include "../../List/CharacterMapList.h"

#include "../../Map/Map.h"

//攻撃時間
const float Goblin::ATTACK_TIME = 0.15f;
//攻撃範囲
const float Goblin::ATTACKRANGE = 5.0f;
//攻撃間隔
const int Goblin::ATTACK_INTERVAL = 1;
//戦闘範囲
const float Goblin::BATTLE_RANGE = 3.0f;

//プレイヤーを吹っ飛ばす力(1秒間に)
const float Goblin::BLOW_AWAY_POWER_PLAYER_TARGET = 20.0f;

//エフェクトの個数
//攻撃範囲
const int Goblin::NUMBER_OF_EFFECT_ATTACK_AREA = 1;
//攻撃が当たった時
const int Goblin::NUMBER_OF_EFFECT_HIT = 25;
//死亡
const int Goblin::NUMBER_OF_EFFECT_DETH = 50;

//スロウ状態の計算用の数値
const float Goblin::SLOW_CAL = 0.025f;
//通常状態の計算用の数値
const float Goblin::NORMAL_CAL = 1.0f;

Goblin::Goblin(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange)
	:Enemy(pos, attack, hp, speed, thisRange)
{
}


Goblin::~Goblin()
{
	if (m_pAttackLine != nullptr)
	{
		m_pAttackLine->Lost();
		m_pAttackLine.reset(nullptr);
	}
}

//エフェクトの作成
void Goblin::CreateEffect()
{
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();

	//HPゲージの作成
	m_pHPGuage = std::make_unique<Effect3D>();
	//画像の設定
	m_pHPGuage->Create(deviceResources, L"Resources/Textures/HPGuage/Player/HP.png");

	//名前の作成
	m_pName = std::make_unique<Effect3D>();
	//画像の設定
	m_pName->Create(deviceResources, L"Resources/Textures/Enemies/Goblin.png");

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
	m_pEnemyDeth->Create(deviceResources,NUMBER_OF_EFFECT_DETH);
}

//派生クラス用更新処理
void Goblin::SubInitialize()
{
	//回転角度の初期化
	m_angle = 360.0f;
	//エネミーの吹っ飛び判定の初期化
	m_blowAway = true;
}

//エフェクトの初期化
void Goblin::EffectInitialize()
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
	DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3(0.5f, 1.0f, (ATTACKRANGE * 0.5f));

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
void Goblin::SubUpdate()
{

}

//エフェクトの更新処理
void Goblin::EffectUpdate(const DX::StepTimer &timer)
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
			+ (DirectX::SimpleMath::Vector3::Down * 1.35f)
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
void Goblin::AliveDraw(const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &projection)
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
void Goblin::Battle()
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
void Goblin::Attack()
{
	float speedBase = ATTACKRANGE / ATTACK_TIME;

	//攻撃中の移動距離
	DirectX::SimpleMath::Vector3 moveVec =
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, m_charaData.rotation)
		*speedBase;

	//突進処理
	//m_speedCalはスロウ状態の時と通常状態の時の時間の計算速度を変えるため
	m_charaData.pos += ((moveVec*m_speedCal)*m_elapsedTime);
	//攻撃時間の計算
	m_attackTime += (m_elapsedTime*m_speedCal);

	//攻撃時間が終了したら
	if (m_attackTime >= ATTACK_TIME)
	{
		//攻撃終了処理
		EndAttack();
	}

	//キャラクターのデータを保持しているリストを取得
	CharacterMapList* list = GameContext<CharacterMapList>::Get();

	//当たり判定
	//プレイヤーと敵
	if (Collision::HitCheck_Capsule2Capsule(list->GetEntityFromID(m_playerID)->GetCapsule(), m_charaData.thisCapsule))
	{
		//攻撃中一回も攻撃が当たっていなかったら
		if (!m_hitAttack)
		{
			//プレイヤーのダメージ処理
			list->GetEntityFromID(m_playerID)->Damage(m_charaData.attack, m_charaData.rotation,true, BLOW_AWAY_POWER_PLAYER_TARGET);
			//攻撃が当たった判定
			m_hitAttack = true;
		}
	}
}

//停止処理
void Goblin::Stop()
{
	if (!m_alive) return;
}

//移動判定
void Goblin::SelectMove()
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
void Goblin::SelectBattle()
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
		//エラー対策
		if (dot > 1.0f)
		{
			dot = 1.0f;
		}

		//なす角
		float angle = std::acos(dot);

		//敵の正面の16度以内のにプレイヤーがいたら戦闘状態
		if (angle <= (ONE_DEGREE * 16.0f))
		{
			m_charaData.state = State::Battle;
		}
	}
}

//攻撃開始判定
void Goblin::SelectAttack()
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
void Goblin::SelectStop()
{

}

//プレイヤーのジャスト回避判定
void Goblin::PlayerJustAvoidance()
{
	if (!m_alive) return;

	if (m_justAvoidance) return;

	//キャラクターのデータを保持しているリストを取得
	CharacterMapList* list = GameContext<CharacterMapList>::Get();

	//エネミーが攻撃状態の時
	//プレイヤーが回避状態の時
	if (m_charaData.state == State::Attack &&
		list->GetEntityFromID(m_playerID)->GetState() == State::Avoidance)
	{
		//プレイヤーの当たり判定の設定
		Collision::Box3D player =
		{
 			list->GetEntityFromID(m_playerID)->GetPos(),
			DirectX::SimpleMath::Vector3(0.5f,1.0f,0.5f),
		};

		//エネミーの攻撃範囲の当たり判定の設定
		//攻撃範囲の中心位置の設定
		DirectX::SimpleMath::Vector3 AttackAreaCenter =
			m_charaData.pos +
			DirectX::SimpleMath::Vector3::Transform(
				DirectX::SimpleMath::Vector3::Forward,
				m_charaData.rotation)
			//中心位置にずらすために長さの半分を求めるために0.5fをかける
			*(ATTACKRANGE*0.5f);

		//攻撃範囲をVector3型に直す
		DirectX::SimpleMath::Vector3 EnemyAttackAreaBase = DirectX::SimpleMath::Vector3(2.0f, 1.0f, -ATTACKRANGE);

		//攻撃範囲の現在の大きさを求める
		DirectX::SimpleMath::Vector3 AttackArea =
			DirectX::SimpleMath::Vector3::Transform(
				EnemyAttackAreaBase,
				m_charaData.rotation);

		//-の値になっていないように
		AttackArea = DirectX::SimpleMath::Vector3(
			std::abs(AttackArea.x),
			std::abs(AttackArea.y),
			std::abs(AttackArea.z)
		);

		//攻撃範囲の当たり判定の設定
		Collision::Box3D enemyAttackArea =
		{
			AttackAreaCenter,
			AttackArea,
		};

		//当たり判定
		//プレイヤーと攻撃範囲
		if (Collision::HitCheckoxBox3D(player, enemyAttackArea))
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

void Goblin::EndAttack()
{
	m_attack_start_end = false;
	m_attackTime = 0.0f;
	m_attackInterval = 0.0f;
	m_charaData.state = State::Idle;
}

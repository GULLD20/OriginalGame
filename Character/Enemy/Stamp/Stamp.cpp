#include "../../../pch.h"


#include "Stamp.h"

#include "../../DataManager/DataManager.h"
#include "../../List/CharacterMapList.h"
#include "../../../MyLib/MyLib.h"

#include "../../Map/Map.h"

//攻撃範囲
const float Stamp::ATTACKRANGE = 3.0f;
//ジャンプの時間
const float Stamp::ATTACKJUMPTIME = 0.25f ;
//攻撃時の移動の時間
const float Stamp::ATTACKMOVETIME = 0.5f;
//攻撃時の落下の時間
const float Stamp::ATTACKFALLTIME = 0.25f;
//攻撃間隔
const int Stamp::ATTACK_INTERVAL = 2;
//戦闘範囲
const float Stamp::BATTLE_RANGE = 10.0f;
//攻撃時の最大の高さの位置
const float Stamp::ATTACKTOPPOS = 10.0f;

//プレイヤーを吹っ飛ばす力(1秒間に)
const float Stamp::BLOW_AWAY_POWER_PLAYER_TARGET = 30.0f;

//エフェクトの個数
//攻撃範囲
const int Stamp::NUMBER_OF_EFFECT_ATTACK_AREA = 1;
//攻撃が当たった時
const int Stamp::NUMBER_OF_EFFECT_HIT = 25;
//死亡
const int Stamp::NUMBER_OF_EFFECT_DETH = 50;

//スロウ状態の計算用の数値
const float Stamp::SLOW_CAL = 0.025f;
//通常状態の計算用の数値
const float Stamp::NORMAL_CAL = 1.0f;

Stamp::Stamp(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange)
	:Enemy(pos, attack, hp, speed, thisRange)
{
}


Stamp::~Stamp()
{
	if (m_pAttackArea != nullptr)
	{
		m_pAttackArea->Lost();
		m_pAttackArea.reset(nullptr);
	}
}

//エフェクトの作成
void Stamp::CreateEffect()
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
	m_pAttackArea = std::make_unique<AttackArea>();
	//画像とエフェクトの作成数の設定
	m_pAttackArea->Create(deviceResources, NUMBER_OF_EFFECT_ATTACK_AREA);

	//攻撃が当たった時の演出(プレイヤーの攻撃がエネミーに当たったら)の作成
	m_pEnemyHit = std::make_unique<EnemyHit>();
	m_pEnemyHit->Create(deviceResources, NUMBER_OF_EFFECT_HIT);

	//死亡演出の作成
	m_pEnemyDeth = std::make_unique<EnemyDeth>();
	//画像とエフェクトの作成数の設定
	m_pEnemyDeth->Create(deviceResources,  NUMBER_OF_EFFECT_DETH);
}

//派生クラス用更新処理
void Stamp::SubInitialize()
{
	//回転角度の初期化
	m_angle = 360.0f;

	//攻撃開始判定
	m_startAttack = false;

	//ジャンプ時の目的位置の初期化
	m_jumpDestination = DirectX::SimpleMath::Vector3(100.0f, 100.0f, 100.0f);

	//エネミーの吹っ飛び判定の初期化
	m_blowAway = true;

	//攻撃のジャンプの時間計測
	m_attackJumpTime = 0.0f;
	//攻撃の移動の時間計測
	m_attackMoveTime = 0.0f;
	//攻撃の落下の時間計測
	m_attackFallTime = 0.0f;

	//攻撃時の各判定の初期化
	//ジャンプ終了
	m_attackJump = false;
	//攻撃終了
	m_attackMove = false;
	//落下終了
	m_attackFall = false;

	//1フレーム余裕を持たせる変数の初期化
	m_oneFrameWait = 0;

	//攻撃終了判定の初期化
	m_endAttack = false;
}

//エフェクトの初期化
void Stamp::EffectInitialize()
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

	//攻撃範囲の初期化
	m_pAttackArea->InitializeNormal(5, DirectX::SimpleMath::Vector3(0, 0, 0));
	m_pAttackArea->SetScale(DirectX::SimpleMath::Vector3(ATTACKRANGE, 1.0f, ATTACKRANGE));

	//攻撃が当たった時の演出(プレイヤーの攻撃がエネミーに当たったら)の初期化
	m_pEnemyHit->InitializeNormal(1, DirectX::SimpleMath::Vector3::Zero);
	m_pEnemyHit->SetScale(DirectX::SimpleMath::Vector3(0.2f, 0.2f, 0.2f));

	//死亡演出の初期化
	m_pEnemyDeth->InitializeCorn(10.0f, DirectX::SimpleMath::Vector3(0.0f, -0.25f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.1f, 0.0f));
	m_pEnemyDeth->SetScale(DirectX::SimpleMath::Vector3(0.2f, 0.2f, 0.2f));
}


//各Update関数-----------------
//派生クラス用更新処理
void Stamp::SubUpdate()
{

}

//エフェクトの更新処理
void Stamp::EffectUpdate(const DX::StepTimer &timer)
{
	if (DethUpdate(timer))
	{
		return;
	}

	HPTextureSize();
	//HPゲージの更新
	m_pHPGuage->RotationY(m_charaData.pos + DirectX::SimpleMath::Vector3(0.0f, 0.5f, 0.0f), m_eyePos);
	m_pName->RotationY(m_charaData.pos + DirectX::SimpleMath::Vector3(0.0f, 0.7f, 0.0f), m_eyePos);

	m_pAttackArea->Update(timer);

	if (m_charaData.state == State::Battle)
	{
		m_pAttackArea->SetPos(GameContext<CharacterMapList>::Get()->GetEntityFromID(m_playerID)->GetPos() + (DirectX::SimpleMath::Vector3::Down));
	}

	m_pAttackArea->SetMainMatrix(m_charaData.rotation);

	//透明なら更新しない
	if (m_hitEffectAlpha > 0.0f)
	{
		m_hitEffectAlpha-= m_elapsedTime;
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
void Stamp::AliveDraw(const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &projection)
{
	//描画に使うデータを取得
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();
	ID3D11DeviceContext1* deviceContext = deviceResources->GetD3DDeviceContext();
	DirectX::CommonStates* CommonState = GameContext<DirectX::CommonStates>().Get();

	// モデル描画
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
	if (m_charaData.state == State::Attack)
	{
		//攻撃範囲の表示
		m_pAttackArea->SetRenderState(m_eyePos, viewMatrix, projection);
		m_pAttackArea->Render();
	}
}

//戦闘中処理
void Stamp::Battle()
{
	//攻撃開始判定
	if (m_attackInterval >= ATTACK_INTERVAL)
	{
		//攻撃開始処理
		m_charaData.state = State::Attack;
		m_hitAttack = false;
		m_startAttack = false;
		m_attackInterval = 0.0f;
	}
	else
	{
		//攻撃開始までの時間計算
		//m_speedCalはスロウ状態の時と通常状態の時の時間の計算速度を変えるため
		m_attackInterval += m_elapsedTime* m_speedCal;
	}
}

//攻撃処理
void Stamp::Attack()
{
	//目的地の設定
	if (m_jumpDestination.y >= 50.0f)
	{
		//キャラクターのデータを保持しているリストを取得
		CharacterMapList* list = GameContext<CharacterMapList>::Get();
		//プレイヤーの位置を取得
		m_jumpDestination = list->GetEntityFromID(m_playerID)->GetPos();
		//プレイヤーの上に位置を修正
		m_jumpDestination.y += ATTACKTOPPOS;
	}

	if (!m_attackJump)
	{
		//ジャンプ処理
		AttackJump();
	}
	else if (!m_attackMove)
	{
		AttackMove();
	}
	else if (!m_attackFall)
	{
		AttackFall();
	}

	if (!m_attackJump || !m_attackMove)
	{
		return;
	}

	if (m_oneFrameWait == 1)
	{
		//攻撃終了処理
		EndAttack();
	}

	if (m_endAttack)
	{
		//1フレーム余裕を持たせる
		m_oneFrameWait++;
	}

	//キャラクターのデータを保持しているリストを取得
	Character* player = GameContext<CharacterMapList>::Get()->GetEntityFromID(m_playerID);

	//当たり判定
	//プレイヤーと敵
	if (Collision::HitCheck_Capsule2Capsule(player->GetCapsule(), m_charaData.thisCapsule))
	{
		//エネミーからプライヤーに向けたベクトルの取得
		DirectX::SimpleMath::Vector3 enemy_player_vec =
			player->GetPos() - m_charaData.pos;

		//ベクトルが上方向に行かないため
		enemy_player_vec.y = 0.0f;

		//正規化
		enemy_player_vec.Normalize();

		//攻撃中一回も攻撃が当たっていなかったら
		if (!m_hitAttack)
		{
			//プレイヤーのダメージ処理
			player->Damage(m_charaData.attack, m_charaData.rotation,true, BLOW_AWAY_POWER_PLAYER_TARGET);
			//攻撃が当たった判定
			m_hitAttack = true;
		}
	}

	Collision::Capsule3D blowArea
	{
		m_charaData.thisCapsule.startPos,
		m_charaData.thisCapsule.endPos,
		ATTACKRANGE,
	};

	float areaS = blowArea.startPos.y - (m_charaData.thisCapsule.range*0.5f);
	float areaE = blowArea.endPos.y + (m_charaData.thisCapsule.range*0.5f);

	if (areaS > player->GetPos().y && areaE < player->GetPos().y)
	{

		if (Collision::HitCheck_Capsule2Capsule(player->GetCapsule(), blowArea))
		{
			//プレイヤーを飛ばすだけ
			player->Damage(0.0f,m_charaData.rotation, true, BLOW_AWAY_POWER_PLAYER_TARGET);
		}
	}
}

//停止処理
void Stamp::Stop()
{
	if (!m_alive) return;
}

//移動判定
void Stamp::SelectMove()
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
void Stamp::SelectBattle()
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
void Stamp::SelectAttack()
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
void Stamp::SelectStop()
{

}

//プレイヤーのジャスト回避判定
void Stamp::PlayerJustAvoidance()
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
		//カプセルの開始位置と終了位置
		DirectX::SimpleMath::Vector3 startPos = DirectX::SimpleMath::Vector3(m_charaData.thisCapsule.startPos.x, m_charaData.thisCapsule.startPos.y*5.0f, m_charaData.thisCapsule.startPos.z);
		DirectX::SimpleMath::Vector3 endPos = DirectX::SimpleMath::Vector3(m_charaData.thisCapsule.endPos.x, m_charaData.thisCapsule.endPos.y*5.0f, m_charaData.thisCapsule.endPos.z);

		//ジャスト回避用の当たり判定(半径を2倍する)
		Collision::Capsule3D enemyAvoidanceArea =
		{
			startPos,
			endPos,
			m_charaData.thisRange * 5.0f,
		};

		//当たり判定
		//プレイヤーと攻撃範囲
		if (Collision::HitCheck_Capsule2Capsule(list->GetEntityFromID(m_playerID)->GetCapsule(), enemyAvoidanceArea))
		{
			//スロウ状態に変更
			StartSlow();
			for (int i = 0; i <static_cast<int>(m_pOtherEnemyList.size()); i++)
			{
				//他の敵のスロウ状態を開始する
				m_pOtherEnemyList[i]->StartSlow();
			}
		}
	}
}

//攻撃終了
void Stamp::EndAttack()
{
	//攻撃判定初期化
	m_attack_start_end = false;
	m_attackTime = 0.0f;
	m_attackInterval = 0.0f;
	m_charaData.state = State::Idle;

	//ジャンプ時の目的地を必ず到達できない位置に設定
	m_jumpDestination.y = 55.0f;
	//敵の高さを調整
	m_charaData.pos.y = 0.0f;

	//自分の当たり判定を設定
	m_charaData.thisCapsule =
	{
		m_charaData.pos + ENEMYSIZE_TOP_DOWN,
		m_charaData.pos - ENEMYSIZE_TOP_DOWN,
		GetThisRange(),
	};

	//各時間の初期化
	m_attackJumpTime = 0.0f;
	m_attackMoveTime = 0.0f;
	m_attackFallTime = 0.0f;

	//各判定の初期化
	m_attackJump = false;
	m_attackMove = false;
	m_attackFall = false;

	//1フレーム余裕を持たせる変数の初期化
	m_oneFrameWait = 0;

	//攻撃終了判定の初期化
	m_endAttack = false;

	//色を戻す
	m_pAttackArea->SetTimming(0.0f);
}

void Stamp::AttackJump()
{
	//上昇処理
	m_charaData.pos.y = MyLib::Lerp(0.0f, ATTACKTOPPOS, m_attackJumpTime / static_cast<float>(ATTACKJUMPTIME));
	m_attackJumpTime += m_elapsedTime;

	//ジャンプ終了
	if (m_attackJumpTime >= ATTACKJUMPTIME)
	{
		m_startMovePos = m_charaData.pos;

		m_attackJump = true;
	}
}

void Stamp::AttackMove()
{
	//ジャンプ終了時のプレイヤーの位置の上に移動
	m_charaData.pos = DirectX::SimpleMath::Vector3::Lerp(m_startMovePos, m_jumpDestination, (m_attackMoveTime / ATTACKMOVETIME));
	m_attackMoveTime+= m_elapsedTime;

	//移動の終了
	if (m_attackMoveTime >= ATTACKMOVETIME)
	{
		m_attackMove = true;
	}
}

void Stamp::AttackFall()
{
	//落下処理(真下に移動)
	m_charaData.pos.y = MyLib::Lerp(ATTACKTOPPOS, 0.0f, (m_attackFallTime / ATTACKFALLTIME));
	m_attackFallTime += m_elapsedTime;

	//地面に当たるタイミング(半径が0.5fなため0.5fをかける)
	m_pAttackArea->SetTimming((m_attackFallTime / ATTACKFALLTIME)*0.5f);

	//攻撃時間が終了したら
	if (m_attackFallTime >= ATTACKFALLTIME)
	{
		m_endAttack = true;
	}
}
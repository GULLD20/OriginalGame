#include "../../../pch.h"

#include "Rowling.h"

#include "../../DataManager/DataManager.h"
#include "../../List/CharacterMapList.h"

#include "../../Map/Map.h"
#include "../../MyLib/MyLib.h"

//攻撃時間
const float Rowling::ATTACK_TIME = 3.0f;
//攻撃範囲
const float Rowling::ATTACKRANGE = 50.0f;
//攻撃間隔
const int Rowling::ATTACK_INTERVAL = 2;
//戦闘範囲
const float Rowling::BATTLE_RANGE = 50.0f;
//攻撃のときの移動速度(通常速度の何倍か)
const float Rowling::ATTACK_MOVE_SPEED = 3.0f;

//プレイヤーを吹っ飛ばす力(1秒間に)
const float Rowling::BLOW_AWAY_POWER_PLAYER_TARGET = 2.0f;

//エフェクトの個数
//攻撃範囲
const int Rowling::NUMBER_OF_EFFECT_ATTACK_AREA = 1;
//攻撃が当たった時
const int Rowling::NUMBER_OF_EFFECT_HIT = 25;
//死亡
const int Rowling::NUMBER_OF_EFFECT_DETH = 50;

//スロウ状態の計算用の数値
const float Rowling::SLOW_CAL = 0.025f;
//通常状態の計算用の数値
const float Rowling::NORMAL_CAL = 1.0f;

Rowling::Rowling(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange)
	:Enemy(pos, attack, hp, speed, thisRange)
{
}


Rowling::~Rowling()
{
	if (m_pAttackLine != nullptr)
	{
		m_pAttackLine->Lost();
		m_pAttackLine.reset(nullptr);
	}
}

//エフェクトの作成
void Rowling::CreateEffect()
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
void Rowling::SubInitialize()
{
	//回転角度の初期化(1秒間に何度回転するか)
	m_angle = 360.0f;

	//攻撃時の角度を保存の初期化
	m_attackAngle = 0.0f;

	//攻撃時の移動経路が決まったか判定
	m_setAttackLine = false;

	//エネミーの吹っ飛び判定の初期化
	m_blowAway = true;
}

//エフェクトの初期化
void Rowling::EffectInitialize()
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
void Rowling::SubUpdate()
{

}

//エフェクトの更新処理
void Rowling::EffectUpdate(const DX::StepTimer &timer)
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
void Rowling::AliveDraw(const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &projection)
{
	//描画に使うデータを取得
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();
	ID3D11DeviceContext1* deviceContext = deviceResources->GetD3DDeviceContext();
	DirectX::CommonStates* commonState = GameContext<DirectX::CommonStates>().Get();

	// モデル描画
	//影
	DirectX::SimpleMath::Matrix shadowMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(m_charaData.pos + DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
	m_shadow->Draw(deviceContext, *commonState, shadowMatrix, viewMatrix, projection);

	//キャラクター
	m_pModel->Draw(deviceContext, *commonState, m_charaData.matrix, viewMatrix, projection);

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
	if (m_charaData.state == State::Battle )
	{
		//攻撃範囲の表示
		m_pAttackLine->SetRenderState(m_eyePos, viewMatrix, projection);
		m_pAttackLine->Render();
	}
}

//戦闘中処理
void Rowling::Battle()
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
		if (!m_setAttackLine)
		{
			SetAttackLine();
		}
		//攻撃開始までの時間計算
		//m_speedCalはスロウ状態の時と通常状態の時の時間の計算速度を変えるため
		m_attackInterval += (m_elapsedTime* m_speedCal);
		m_pAttackLine->SetTimming(m_attackInterval / ATTACK_INTERVAL);
	}
}

//攻撃処理
void Rowling::Attack()
{
	//移動速度を求める
	DirectX::SimpleMath::Vector3 vec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, m_charaData.rotation)
		* (m_charaData.speed * ATTACK_MOVE_SPEED);

	//移動処理(m_elapsedTimeをかけて1秒間の速度を求める)
	m_charaData.pos += ((vec * m_speedCal)*m_elapsedTime);

	//スロウ状態の時は反射処理をしない
	if (!m_justAvoidance)
	{
		Reflect();
	}
	//位置を保存
	m_oldPos = m_charaData.pos;

	//攻撃時間の計算(スロウ状態のときは合わせて計算も遅らせる)
	m_attackTime += m_elapsedTime * m_speedCal;

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
void Rowling::Stop()
{
	if (!m_alive) return;
}

//移動判定
void Rowling::SelectMove()
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
			//攻撃終了処理
			EndAttack();
		}
		m_charaData.state = State::Move;
	}
}

//戦闘開始判定
void Rowling::SelectBattle()
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
		if (angle <= (ONE_DEGREE * 8.0f))
		{
			m_charaData.state = State::Battle;
		}
	}
}

//攻撃開始判定
void Rowling::SelectAttack()
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
void Rowling::SelectStop()
{

}

//プレイヤーのジャスト回避判定
void Rowling::PlayerJustAvoidance()
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
		Collision::Capsule3D enemyAvodanceArea =
		{
			m_charaData.thisCapsule.startPos,
			m_charaData.thisCapsule.endPos,
			m_charaData.thisRange * 10.0f,
		};

		//当たり判定
		//プレイヤーと攻撃範囲
		if (Collision::HitCheck_Capsule2Capsule(list->GetEntityFromID(m_playerID)->GetCapsule(), enemyAvodanceArea))
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

void Rowling::EndAttack()
{
	m_attack_start_end = false;
	m_attackTime = 0.0f;
	m_attackInterval = 0.0f;
	m_charaData.state = State::Idle;
	m_setAttackLine = false;
}

void Rowling::SetAttackLine()
{
	//移動方向を乱数で取得する(0度～360度)
	m_attackAngle = ONE_DEGREE * static_cast<float>(MyLib::MersenneTwisterI(0, 360));
	//角度の設定
	if (m_setAttackLine)
	{
		m_charaData.angle.y = m_attackAngle;
	}
}

//反射時の角度を求める
void Rowling::Reflect()
{
	if (ReflectNearlyEqual_Old_Now_Pos())
	{
		m_setAttackLine = true;
		return;
	}

	//マップの取得
	Map* map = GameContext<Map>::Get();
	//壁にあたっているか判定
	if (map->GetHitWallCorner(m_charaData.pos).pos1 != DirectX::SimpleMath::Vector3::Zero)
	{
		if (EndReflect())
		{
			return;
		}
		//壁に当たった時に攻撃がプレイヤーにあ当たるかの判定を戻す
		m_hitAttack = false;
		//最初以外は乱数で方向を決めるため
		m_setAttackLine = true;
		//次に向かう方向を決める
		SetAttackLine();
	}
}

//1フレーム前の位置と現在の位置がほぼ等しいか
bool  Rowling::ReflectNearlyEqual_Old_Now_Pos()
{
	//角にいるときに壁と当たっていない状態になるときがあるため
	//四角の当たり判定で近い位置にいるか判定
	Collision::Box3D old =
	{
		m_oldPos,
		DirectX::SimpleMath::Vector3(0.1f,0.1f,0.1f),
	};
	Collision::Box3D now =
	{
		m_charaData.pos,
		DirectX::SimpleMath::Vector3(0.1f,0.1f,0.1f),
	};

	if (Collision::HitCheckoxBox3D(now, old))
	{
		if (EndReflect())
		{
			return true;
		}
		//壁に当たった時に攻撃がプレイヤーにあ当たるかの判定を戻す
		m_hitAttack = false;
		//次に向かう方向を決める
		SetAttackLine();
		return true;
	}

	return false;
}

//反射の終了処理
bool Rowling::EndReflect()
{
	if (m_attackTime >= ATTACK_TIME)
	{
		//攻撃終了処理
		EndAttack();
		return true;
	}
	return false;
}
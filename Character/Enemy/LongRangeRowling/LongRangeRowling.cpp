#include "../../../pch.h"

#include "LongRangeRowling.h"

#include "../../DataManager/DataManager.h"
#include "../../List/CharacterMapList.h"

#include "../../Map/Map.h"

//UÔ
const float LongRangeRowling::ATTACK_TIME = 0.25f;
//UÍÍ
const float LongRangeRowling::ATTACKRANGE = 20.0f;
//UÔu
const float LongRangeRowling::ATTACK_INTERVAL = 0.25f;
//í¬ÍÍ
const float LongRangeRowling::BATTLE_RANGE = 20.0f;
//eÌU¬x(1bÔÌ¬x)
const float LongRangeRowling::ATTACK_SPEED = 15.0f;

//vC[ðÁòÎ·Í(1bÔÉ)
const float LongRangeRowling::BLOW_AWAY_POWER_PLAYER_TARGET = 5.0f;

//GtFNgÌÂ
//UÍÍ
const int LongRangeRowling::NUMBER_OF_EFFECT_ATTACK_AREA = 1;
//Uª½Á½
const int LongRangeRowling::NUMBER_OF_EFFECT_HIT = 25;
//S
const int LongRangeRowling::NUMBER_OF_EFFECT_DETH = 50;

//XEóÔÌvZpÌl
const float LongRangeRowling::SLOW_CAL = 0.025f;
//ÊíóÔÌvZpÌl
const float LongRangeRowling::NORMAL_CAL = 1.0f;

LongRangeRowling::LongRangeRowling(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange)
	:Enemy(pos, attack, hp, speed, thisRange)
{
}


LongRangeRowling::~LongRangeRowling()
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

//GtFNgÌì¬
void LongRangeRowling::CreateEffect()
{
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();

	//HPQ[WÌì¬
	m_pHPGuage = std::make_unique<Effect3D>();
	//æÌÝè
	m_pHPGuage->Create(deviceResources, L"Resources/Textures/HPGuage/Player/HP.png");

	//¼OÌì¬
	m_pName = std::make_unique<Effect3D>();
	//æÌÝè
	m_pName->Create(deviceResources, L"Resources/Textures/Enemies/Su.png");

	//UÍÍÌì¬
	m_pAttackLine = std::make_unique<AttackLine>();
	//æÆGtFNgÌì¬ÌÝè
	m_pAttackLine->Create(deviceResources, NUMBER_OF_EFFECT_ATTACK_AREA);

	//Uª½Á½Ìo(vC[ÌUªGl~[É½Á½ç)Ìì¬
	m_pEnemyHit = std::make_unique<EnemyHit>();
	m_pEnemyHit->Create(deviceResources, NUMBER_OF_EFFECT_HIT);

	//SoÌì¬
	m_pEnemyDeth = std::make_unique<EnemyDeth>();
	//æÆGtFNgÌì¬ÌÝè
	m_pEnemyDeth->Create(deviceResources, NUMBER_OF_EFFECT_DETH);
}

//h¶NXpXV
void LongRangeRowling::SubInitialize()
{
	m_pLongRangeAttack = std::make_unique<LongRangeAttack>();
	//UÌfì¬
	m_pLongRangeAttack->CreatModel();
	//UÌú»
	m_pLongRangeAttack->Initialize(ATTACK_SPEED,static_cast<int>(BATTLE_RANGE));
	//ñ]pxÌú»
	m_angle = 1.0f;
	//Gl~[ÌÁòÑ»èÌú»
	m_blowAway = false;

	//ñ]px
	m_anglY = 0.0f;
}

//GtFNgÌú»
void LongRangeRowling::EffectInitialize()
{
	//HPÌæÌå«³ÌÝè
	m_pHPGuage->SetTextureScaleTL(DirectX::SimpleMath::Vector2(1.0f, 0.06f));
	m_pHPGuage->SetTextureScaleBR(DirectX::SimpleMath::Vector2(-1.0f, -0.06f));
	//HPÌæÌØèæèÊuðÝè
	m_pHPGuage->SetTextureSizeTL(DirectX::SimpleMath::Vector2(0.0f, 0.0f));
	m_pHPGuage->SetTextureSizeBR(DirectX::SimpleMath::Vector2(1.0f, 1.0f));

	//¼OÌæÌå«³ÌÝè
	m_pName->SetTextureScaleTL(DirectX::SimpleMath::Vector2(0.60f, 0.15f));
	m_pName->SetTextureScaleBR(DirectX::SimpleMath::Vector2(-0.60f, -0.15f));
	//¼OÌæÌØèæèÊuðÝè
	m_pName->SetTextureSizeTL(DirectX::SimpleMath::Vector2(0.0f, 0.0f));
	m_pName->SetTextureSizeBR(DirectX::SimpleMath::Vector2(1.0f, 0.5f));

	m_pAttackLine->InitializeNormal(5, DirectX::SimpleMath::Vector3(0, 0, 0));
	DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3(0.15f, 1.0f, (ATTACKRANGE * 0.5f));

	m_pAttackLine->SetScale(scale);

	//Uª½Á½Ìo(vC[ÌUªGl~[É½Á½ç)Ìú»
	m_pEnemyHit->InitializeNormal(1, DirectX::SimpleMath::Vector3::Zero);
	m_pEnemyHit->SetScale(DirectX::SimpleMath::Vector3(0.2f, 0.2f, 0.2f));

	//SoÌú»
	m_pEnemyDeth->InitializeCorn(10.0f, DirectX::SimpleMath::Vector3(0.0f, -0.25f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.1f, 0.0f));
	m_pEnemyDeth->SetScale(DirectX::SimpleMath::Vector3(0.2f, 0.2f, 0.2f));
}


//eUpdateÖ-----------------
//h¶NXpXV
void LongRangeRowling::SubUpdate()
{
	//ñ]
	m_anglY += m_angle*m_elapsedTime *m_speedCal;
	//±ÌNXàÅÌÝñ]³¹é½ß
	m_charaData.angle.y = m_anglY;

	//U(e)ÌXV
	m_pLongRangeAttack->SetSpeedCal(m_speedCal);
	m_pLongRangeAttack->Update(m_elapsedTime);
	//eªGÉ½Á½©
	HitAttack();
}

//GtFNgÌXV
void LongRangeRowling::EffectUpdate(const DX::StepTimer &timer)
{
	if (DethUpdate(timer))
	{
		return;
	}

	HPTextureSize();
	//HPQ[WÌXV
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

	//§¾ÈçXVµÈ¢
	if (m_hitEffectAlpha > 0.0f)
	{
		m_hitEffectAlpha -= m_elapsedTime;
		m_pEnemyHit->Update(timer);
	}

	//æÌØèæèÊuðÝè
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

//e`æ-------------------
//¶¶Ì`æ
void LongRangeRowling::AliveDraw(const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &projection)
{
	//`æÉg¤f[^ðæ¾
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();
	ID3D11DeviceContext1* deviceContext = deviceResources->GetD3DDeviceContext();
	DirectX::CommonStates* CommonState = GameContext<DirectX::CommonStates>().Get();

	// f`æ
	//e
	DirectX::SimpleMath::Matrix shadowMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(m_charaData.pos + DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
	m_shadow->Draw(deviceContext, *CommonState, shadowMatrix, viewMatrix, projection);

	//LN^[
	m_pModel->Draw(deviceContext, *CommonState, m_charaData.matrix, viewMatrix, projection);

	//UÌ`æ
	m_pLongRangeAttack->Draw(viewMatrix, projection);


	//»Ý[[Nª­¶
	//HPQ[WÌ\¦
	m_pHPGuage->SetRenderState(viewMatrix.Translation(), viewMatrix, projection);
	m_pHPGuage->Render();


	//»Ý[[Nª­¶
	//¼OÌ\¦
	m_pName->SetRenderState(viewMatrix.Translation(), viewMatrix, projection);
	m_pName->Render();


	//Uª½Á½ÌGtFNgÌ`æ
	if (m_hitEffectAlpha > 0.0f)
	{
		m_pEnemyHit->SetRenderState(m_eyePos, viewMatrix, projection);
		m_pEnemyHit->Render();
	}


	//í¬
	if (m_charaData.state == State::Battle || m_charaData.state == State::Attack)
	{
		//UÍÍÌ\¦
		m_pAttackLine->SetRenderState(m_eyePos, viewMatrix, projection);
		m_pAttackLine->Render();
	}

}

//í¬
void LongRangeRowling::Battle()
{
	//UJn»è
	if (m_attackInterval >= ATTACK_INTERVAL)
	{
		//UJn
		m_charaData.state = State::Attack;
		m_hitAttack = false;
	}
	else
	{
		//UJnÜÅÌÔvZ
		//m_speedCalÍXEóÔÌÆÊíóÔÌÌÔÌvZ¬xðÏ¦é½ß
		m_attackInterval += m_elapsedTime* m_speedCal;
		m_pAttackLine->SetTimming(m_attackInterval / ATTACK_INTERVAL);
	}
}

//U
void LongRangeRowling::Attack()
{
	DirectX::SimpleMath::Matrix enemyRotation = m_charaData.rotation;

	//eÌ­Ë
	for (int i = 0; i < 4; i++)
	{
		enemyRotation *= DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XM_PI*0.5f);
		m_pLongRangeAttack->Shot(m_charaData.pos, enemyRotation);
	}
	EndAttack();
}

//â~
void LongRangeRowling::Stop()
{
	if (!m_alive) return;
}

//Ú®»è
void LongRangeRowling::SelectMove()
{
	//LN^[Ìf[^ðÛµÄ¢éXgðæ¾
	CharacterMapList* list = GameContext<CharacterMapList>::Get();
	//vC[ÆGl~[ÌÔÌ£
	float length = (list->GetEntityFromID(m_playerID)->GetPos() - m_charaData.pos).Length();

	//UÍÍO
	if (length > BATTLE_RANGE)
	{
		if (m_charaData.state == State::Battle)
		{
			EndAttack();
		}
		m_charaData.state = State::Move;
	}
}

//í¬Jn»è
void LongRangeRowling::SelectBattle()
{
	//LN^[Ìf[^ðÛµÄ¢éXgðæ¾
	CharacterMapList* list = GameContext<CharacterMapList>::Get();
	//vC[ÆGl~[ÌÔÌ£
	float length = (list->GetEntityFromID(m_playerID)->GetPos() - m_charaData.pos).Length();

	m_charaData.state = State::Battle;
	return;

	//UÍÍà
	if (length <= BATTLE_RANGE)
	{
		//Gl~[©ç^[QbgÉü¯½xNg
		DirectX::SimpleMath::Vector3 enemy_target_vec = list->GetEntityFromID(m_playerID)->GetPos() - m_charaData.pos;

		//Gl~[Ì³ÊÉü¯½xNg
		DirectX::SimpleMath::Vector3 frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, m_charaData.rotation);

		//³K»
		enemy_target_vec.Normalize();
		frontVec.Normalize();

		//àÏðßé
		float dot = enemy_target_vec.Dot(frontVec);

		//G[Îô(àÏÌÅåÍ1¾©ç»êð´¦È¢æ¤É)
		if (dot > 1.0f)
		{
			dot = 1.0f;
		}

		//È·p
		float angle = std::acos(dot);

		//GÌ³ÊÌ6xÈàÌÉvC[ª¢½çí¬óÔ(³ÊðSÆµÄ6xÈàÉ·é½ßÉ3ð©¯é)
		if (angle <= (ONE_DEGREE * 3.0f))
		{
			m_charaData.state = State::Battle;
		}
	}
}

//UJn»è
void LongRangeRowling::SelectAttack()
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

//â~»è
void LongRangeRowling::SelectStop()
{

}

//vC[ÌWXgñð»è
void LongRangeRowling::PlayerJustAvoidance()
{
	if (!m_alive) return;

	if (m_justAvoidance) return;

	//LN^[Ìf[^ðÛµÄ¢éXgðæ¾
	CharacterMapList* list = GameContext<CharacterMapList>::Get();

	//Gl~[ªUóÔÌ
	//vC[ªñðóÔÌ
	if (list->GetEntityFromID(m_playerID)->GetState() == State::Avoidance)
	{
		//vC[Ì½è»èÌÝè
		Collision::Box3D player =
		{
 			list->GetEntityFromID(m_playerID)->GetPos(),
			DirectX::SimpleMath::Vector3(0.5f,1.0f,0.5f),
		};

		//eÌÅêÔvC[Éß¢eÌ½è»èÌf[^ðæ¾
		Collision::Capsule3D enemyCapsuleData = m_pLongRangeAttack->GetCapsuleData(list->GetEntityFromID(m_playerID)->GetPos());
		//æ¾µ½eÌÊuðæ¾
		DirectX::SimpleMath::Vector3 enemyCapsulePos = enemyCapsuleData.startPos + ((enemyCapsuleData.startPos - enemyCapsuleData.endPos)*0.5f);

		//UÍÍÌ½è»èÌÝè(WXgñðÌÍÍ)
		//{Ìå«³æèå«­·é½ß5{·é
		Collision::Box3D enemyAttack =
		{
			enemyCapsulePos,
			DirectX::SimpleMath::Vector3(
				std::abs(enemyCapsuleData.startPos.x - enemyCapsuleData.endPos.x),
				std::abs(enemyCapsuleData.range),
				std::abs(enemyCapsuleData.startPos.z - enemyCapsuleData.endPos.z)
			)*5.0f,
		};

		//½è»èÌå«³ª0¾Á½ç
		if (enemyAttack.size.x <= 0.1f)
		{
			enemyAttack.size.x = 0.5f;
		}

		if (enemyAttack.size.z <= 0.1f)
		{
			enemyAttack.size.z = 0.5f;
		}

		//½è»è
		//vC[ÆUÍÍ
		if (Collision::HitCheckoxBox3D(player, enemyAttack))
		{
			//XEóÔÉÏX
			StartSlow();
			for (int i = 0; i < static_cast<int>(m_pOtherEnemyList.size()); i++)
			{
				//¼ÌGÌXEóÔðJn·é
				m_pOtherEnemyList[i]->StartSlow();
			}
		}
	}
}

void LongRangeRowling::EndAttack()
{
	m_attack_start_end = false;
	m_attackTime = 0.0f;
	m_attackInterval = 0.0f;
	m_charaData.state = State::Idle;
}

//UÌ½è»è
void LongRangeRowling::HitAttack()
{
	//LN^[Ìf[^ðÛµÄ¢éXgðæ¾
	CharacterMapList* list = GameContext<CharacterMapList>::Get();

	//½è»è
	//vC[Æe(vC[ÉÅàß¢e)
	if (Collision::HitCheck_Capsule2Capsule(
		list->GetEntityFromID(m_playerID)->GetCapsule(),
		m_pLongRangeAttack->GetCapsuleData(list->GetEntityFromID(m_playerID)->GetPos())
	))
	{
		//UêñàUª½ÁÄ¢È©Á½ç
		//if (!m_hitAttack)
		{
			//eÌâ~
			m_pLongRangeAttack->HitAttack();
			//vC[Ì_[W
			list->GetEntityFromID(m_playerID)->Damage(m_charaData.attack, m_charaData.rotation, true, BLOW_AWAY_POWER_PLAYER_TARGET);
			//Uª½Á½»è
			m_hitAttack = true;
		}
	}
}
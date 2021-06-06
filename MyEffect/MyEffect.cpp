//----------------------------------------
// MyEffect.cpp
//----------------------------------------

#include "../pch.h"
#include "Myeffect.h"

#include "../DeviceResources.h"
#include "../StepTimer.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include "BinaryFile.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;



void MyEffect::Initialize(float life, const DirectX::SimpleMath::Vector3 &pos, const DirectX::SimpleMath::Vector3 &velocity)
{
	m_startPosition = m_position = pos;
	m_startVelocity = m_velocity = velocity;
	m_startLife = m_life = life;
	m_gravity = Vector3(0, 0.001f, 0);
}

void MyEffect::Update(const DX::StepTimer &timer)
{
	float time = static_cast<float>((m_timer.GetTotalSeconds()));
	m_timer = timer;

	//m_position = Vector3(cosf(time+m_velocity.x), sinf(time + m_velocity.x),0);


	m_position += m_velocity;

	m_velocity += m_startVelocity * 0.2f;

	if (m_life < 0) {
		Initialize(m_startLife, m_startPosition, m_startVelocity);

	}
	m_life -= static_cast<float>(m_timer.GetElapsedSeconds());
}
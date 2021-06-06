//----------------------------------------
// MyEffect.h
//----------------------------------------
#pragma once

#include "../DeviceResources.h"
#include "../StepTimer.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>

class MyEffect
{
public:

	void Initialize(float life, const DirectX::SimpleMath::Vector3 &pos, const DirectX::SimpleMath::Vector3 &velocity);
	void Update(const DX::StepTimer &timer);

	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	DirectX::SimpleMath::Vector3 GetVelocity() const { return m_velocity; }

	void SetVector(const DirectX::SimpleMath::Vector3 &vel) { m_startVelocity = m_velocity = vel; };
private:

	DX::StepTimer                           m_timer;


	DirectX::SimpleMath::Vector3		    m_gravity;
	DirectX::SimpleMath::Vector3		    m_position;
	DirectX::SimpleMath::Vector3		    m_velocity;
	float								    m_life;

	DirectX::SimpleMath::Vector3		    m_startPosition;
	DirectX::SimpleMath::Vector3		    m_startVelocity;
	float								    m_startLife;

};
#include "stdafx.h"
#include "SteeringBehaviours.h"
#include "IBehaviourPlugin.h"

namespace SteeringBehaviours
{
	//SEEK
	//****
	PluginOutput Seek::CalculateSteering(float deltaT, AgentInfo* pAgent)
	{
		PluginOutput steering = {};

		auto targetVelocity = (*m_pTargetRef) - pAgent->Position;
		targetVelocity.Normalize();
		targetVelocity *= pAgent->MaxLinearSpeed;

		steering.LinearVelocity = targetVelocity - pAgent->LinearVelocity;

		return steering;
	}

	//FLEE
	//****
	PluginOutput Flee::CalculateSteering(float deltaT, AgentInfo* pAgent)
	{
		PluginOutput steering = {};

		auto targetVelocity = pAgent->Position  - (*m_pTargetRef);
		targetVelocity.Normalize();
		targetVelocity *= pAgent->MaxLinearSpeed;

		steering.LinearVelocity = targetVelocity - pAgent->LinearVelocity;

		return steering;
	}

	//WANDER
	//******
	PluginOutput Wander::CalculateSteering(float deltaT, AgentInfo* pAgent)
	{
		auto offset = pAgent->LinearVelocity;
		offset.Normalize();
		offset *= m_Offset;

		b2Vec2 circleOffset = { cos(m_WanderAngle) * m_Radius, sin(m_WanderAngle) * m_Radius };

		m_WanderAngle += randomFloat() * m_AngleChange -(m_AngleChange * .5f); //RAND[-angleChange/2,angleChange/2]

		auto newTarget = new b2Vec2 (pAgent->Position + offset + circleOffset);
		Seek::m_pTargetRef = newTarget;

		return Seek::CalculateSteering(deltaT, pAgent);
	}

	//ARRIVE
	//******
	PluginOutput Arrive::CalculateSteering(float deltaT, AgentInfo* pAgent)
	{
		PluginOutput steering = {};

		auto targetVelocity = (*m_pTargetRef - pAgent->Position);
		auto distance = targetVelocity.Normalize() - m_TargetRadius;

		if(distance < m_SlowRadius) //Inside SlowRadius
		{
			targetVelocity *= pAgent->MaxLinearSpeed * (distance / (m_SlowRadius + m_TargetRadius));
		}
		else
		{
			targetVelocity *= pAgent->MaxLinearSpeed;
		}

		//Calculate Steering
		steering.LinearVelocity = targetVelocity - pAgent->LinearVelocity;

		return steering;
	}
}

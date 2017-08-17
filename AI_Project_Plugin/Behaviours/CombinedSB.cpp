#include "stdafx.h"
#include "CombinedSB.h"

namespace CombinedSB
{
	//BLENDED STEERING
	//****************
	PluginOutput BlendedSteering::CalculateSteering(float deltaT, AgentInfo* pAgent)
	{
		PluginOutput steering = {};
		auto totalWeight = 0.f;

		for (auto pair : m_WeightedBehavioursVec)
		{
			auto retSteering = pair.pBehaviour->CalculateSteering(deltaT, pAgent);
			steering.LinearVelocity += pair.Weight * retSteering.LinearVelocity;
			steering.AngularVelocity += pair.Weight * retSteering.AngularVelocity;

			totalWeight += pair.Weight;
		}

		if (totalWeight > 0.f)
		{
			auto scale = 1.f / totalWeight;
			steering.LinearVelocity *= scale;
			steering.AngularVelocity *= scale;
		}

		return steering;
	}

	//PRIORITY STEERING
	//*****************
	PluginOutput PrioritySteering::CalculateSteering(float deltaT, AgentInfo* pAgent)
	{
		PluginOutput steering = {};

		for(auto pBehaviour : m_Behaviours)
		{
			steering = pBehaviour->CalculateSteering(deltaT, pAgent);

			if(!(steering.LinearVelocity.LengthSquared() + (steering.AngularVelocity*steering.AngularVelocity)) < (m_Epsilon*m_Epsilon))
				break;
		}

		//If non of the behaviour return a valid output, last behaviour is returned
		return steering;
	}

	//BLENDED-PRIORITY STEERING
	//*************************
	PluginOutput BlendedPrioritySteering::CalculateSteering(float deltaT, AgentInfo* pAgent)
	{
		PluginOutput steering = {};

		for (auto pBlendedBehaviour : m_PriorityGroups)
		{
			steering = pBlendedBehaviour->CalculateSteering(deltaT, pAgent);

			if (!(steering.LinearVelocity.LengthSquared() + (steering.AngularVelocity*steering.AngularVelocity)) < (m_Epsilon*m_Epsilon))
				break;
		}

		//If non of the behaviour return a valid output, last behaviour is returned
		return steering;
	}
}
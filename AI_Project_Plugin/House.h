#pragma once
#include <Box2D/Common/b2Math.h>

class House
{
public:
	House(b2Vec2 worldDim):m_WorldDim(worldDim){};
	~House(){};

	void Update(float dTime, AgentInfo* agent, bool finishedScouting)
	{
		if (m_FirstTime == -1)
		{
			m_FirstTime = (m_WorldDim.x * m_WorldDim.y) / 230.0f;
			m_Maxtime = (m_WorldDim.x * m_WorldDim.y) / 350.0f;
		}

		if (m_ReachedCenter)
		{
			m_TimeOut += dTime;
			if (m_TimeOut > m_FirstTime && m_isFirsttime)// && finishedScouting)//scoutround
			{
				m_TimeOut = 0;
				m_ReachedCenter = false;
				m_isFirsttime = false;
			}
			else if(m_TimeOut > m_Maxtime && !m_isFirsttime)
			{
				m_TimeOut = 0;
				m_ReachedCenter = false;
			}

		}
		else
		{
			auto distanceSquared = (m_Center - agent->Position).LengthSquared();
			if (distanceSquared < 1.0f)
				m_ReachedCenter = true;
		}
	}
	b2Vec2 m_WorldDim;
	b2Vec2 m_Center = b2Vec2_zero;
	b2Vec2 m_Size = b2Vec2_zero;
	bool m_ReachedCenter = false;
	float m_TimeOut = 0;

private:
	bool m_isFirsttime = true;
	float m_FirstTime = -1;
	float m_Maxtime = -1;
};


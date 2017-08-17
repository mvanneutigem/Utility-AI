#pragma once
#include <vector>
#include "IBehaviourPlugin.h"
#include "Behaviours/Blackboard.h"
#include <functional>
#include "Utility_Consideration.h"


//action
class Utility_Action
{
public:
	explicit Utility_Action(std::function<BehaviorState(Blackboard*)> fp, std::string name = "", int priority = 0, bool interruptable = true):
	m_fpAction(fp),
	m_Name(name),
	m_Priority(priority),
	m_Interruptable(interruptable){};
	~Utility_Action()
	{
		for (unsigned int i = 0; i< m_pConsiderationsArr.size(); ++i)
		{
			delete (m_pConsiderationsArr[i]);
			m_pConsiderationsArr[i] = nullptr;
		}
		m_pConsiderationsArr.clear();
	};

	float GetScore() const { return m_Score; }

	bool IsInterruptable() const { return m_Interruptable; }

	void AddConsideration(Utility_Consideration* consideration)
	{
		m_pConsiderationsArr.push_back(consideration);
	}

	void EvaluateAction()
	{
		m_Score = 0.0f;
		for (auto c : m_pConsiderationsArr)
		{
			m_Score += c->GetUtilityScore() * c->GetWeight();
		}
		m_Score /= m_pConsiderationsArr.size();
	}

	std::vector<Utility_Consideration*>& GetConsiderationsRef()
	{
		return m_pConsiderationsArr;
	}

	BehaviorState Execute(Blackboard* pBlackBoard) 
	{
		if (!called && counter == -1)
		{
			called = true;
			counter = m_Time;
		}

		if (counter > 0)
		{
			float* dTime;
			pBlackBoard->GetData("deltaT", dTime);
			counter -= *dTime;
		}
		else
			counter = -1;

		return m_fpAction(pBlackBoard);
	}

	std::string m_Name;
	int m_Priority;
	float m_Time = 0;
	bool m_Interruptable = true;
	float counter = -1;
	bool called = false;

private:

	float m_Score = 0.0f;
	std::vector<Utility_Consideration*> m_pConsiderationsArr;
	std::function<BehaviorState(Blackboard*)> m_fpAction = nullptr;

};


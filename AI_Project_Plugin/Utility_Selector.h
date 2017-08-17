#pragma once
#include "Utility_Action.h"

//base utility
class UtilitySelector
{
	//here i score all actions and select the current top action

public:
	UtilitySelector() {}
	~UtilitySelector()
	{
		for (unsigned int i = 0; i< m_pActionsArr.size(); ++i)
		{
			delete (m_pActionsArr[i]);
			m_pActionsArr[i] = nullptr;
		}
		m_pActionsArr.clear();
	}

	void AddAction(Utility_Action* consideration)
	{
		m_pActionsArr.push_back(consideration);
	}

	Utility_Action* GetTopAction()
	{
		//system("CLS");
		if (!m_pPrevAction)
			m_pPrevAction = m_pActionsArr[0];
		float topScore = 0;
		int topID = 0;
		if (m_pPrevAction->IsInterruptable() || (m_pPrevAction->counter == 0 && m_pPrevAction->called))
		{
			for (unsigned int i = 0; i < m_pActionsArr.size(); ++i)
			{
				m_pActionsArr[i]->EvaluateAction();
				auto currScore = m_pActionsArr[i]->GetScore();
				if (currScore > topScore)
				{

					if (currScore - topScore < 0.02f)//offset
					{
						//if close together pick higher priority one
						if (m_pActionsArr[i]->m_Priority > m_pActionsArr[topID]->m_Priority)
						{
							topID = i;
							topScore = currScore;
						}
					}
					else
					{
						topID = i;
						topScore = currScore;
					}
				}
				std::cout << m_pActionsArr[i]->m_Name << " score:" << currScore << std::endl;
			}

		}
		m_pPrevAction = m_pActionsArr[topID];
		return m_pActionsArr[topID];

	}

private:
	std::vector<Utility_Action*> m_pActionsArr;
	Utility_Action* m_pPrevAction = nullptr;
};


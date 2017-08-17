#pragma once
//Include
#include <projects/App_Steering/HelperStructs.h>
#include <projects/App_Steering/SteeringBehaviours.h>

class BehaviorTree;
class Agent;

//GUARD Class
class Guard
{
public:
	Guard();
	~Guard();

	void Initialize(Context* pContext);
	void Update(float deltaTime, const b2Vec2& targetPos, bool isSet);

private:
	Agent* m_pAgent = nullptr;
	vector<SteeringBehaviours::ISteeringBehaviour*> m_BehaviourVec = {};
	TargetData m_Target = {};
	BehaviorTree* m_pBehaviorTree = nullptr;
};


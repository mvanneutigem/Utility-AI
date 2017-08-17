#include "stdafx.h"
#include "Guard.h"
#include "projects/App_Steering/Agent.h"
#include "Blackboard.h"
#include "BehaviorTree.h"
#include "Behaviors.h"

Guard::Guard()
{}

Guard::~Guard()
{
	for (auto pb : m_BehaviourVec)
		SafeDelete(pb);
	m_BehaviourVec.clear();

	SafeDelete(m_pBehaviorTree);
	SafeDelete(m_pAgent);
}

void Guard::Initialize(Context* pContext)
{
	//Create agent
	m_pAgent = new Agent(pContext);
	//Create steering behaviors
	auto pSeekBehavior = new SteeringBehaviours::Seek();
	pSeekBehavior->SetTarget(&m_Target);
	m_BehaviourVec.push_back(pSeekBehavior);
	auto pWanderBehavior = new SteeringBehaviours::Wander(pContext);
	m_BehaviourVec.push_back(pWanderBehavior);
	//Set initial steering behavior
	m_pAgent->SetSteeringBehaviour(pWanderBehavior);

	//*** Create blackboard ***
	auto pBlackboard = new Blackboard();
	pBlackboard->AddData("Agent", m_pAgent);
	pBlackboard->AddData("TargetPosition", m_Target.Position);
	pBlackboard->AddData("TargetSet", false);
	pBlackboard->AddData("SeekBehaviour",
		static_cast<SteeringBehaviours::ISteeringBehaviour*>(pSeekBehavior));
	pBlackboard->AddData("WanderBehaviour",
		static_cast<SteeringBehaviours::ISteeringBehaviour*>(pWanderBehavior));
	pBlackboard->AddData("CloseToRadius", 1.0f);
	pBlackboard->AddData("IsBoxOpen", false);

	//*** Create behavior tree ***
	m_pBehaviorTree = new BehaviorTree(pBlackboard,
		new BehaviorSelector
		({
			new BehaviorSequence
			({
				new BehaviorConditional(HasTarget),
				new BehaviorConditional(NotCloseToTarget),
				new BehaviorAction(ChangeToSeek)
			}),
			new BehaviorSelector
			({
				new BehaviorSequence
				({
					new BehaviorConditional(HasTarget),
					new BehaviorConditional(IsBoxNotOpen),
					new BehaviorAction(OpenBox),
					new BehaviorAction(PickUpItem)
				}),
				new BehaviorSequence
				({
					new BehaviorConditional(HasTarget),
					new BehaviorConditional(PickUpItem)
				}),
				new BehaviorAction(ChangeToWander)
			})
		}));
}

void Guard::Update(float deltaTime, const b2Vec2& targetPos, bool isSet)
{
	//*** Change blackboard data ***
	auto pBlackboard = m_pBehaviorTree->GetBlackboard();
	if(pBlackboard)
	{
		if (isSet)
		{
			m_Target.Position = targetPos;
			pBlackboard->ChangeData("TargetPosition", targetPos);
			pBlackboard->ChangeData("TargetSet", true);

			auto r = rand() % 2;
			auto isOpen = r == 0 ? true : false;
			pBlackboard->ChangeData("IsBoxOpen", isOpen);
		}
	}

	//Update tree
	if(m_pBehaviorTree)
		m_pBehaviorTree->Update();

	//Update agent
	m_pAgent->Update(deltaTime);
}
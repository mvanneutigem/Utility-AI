#pragma once
#include "IBehaviourPlugin.h"
#include "Behaviours/SteeringBehaviours.h"
#include "House.h"
#include "Behaviours/CombinedSB_PipelineImp.h"


class UtilityBuilder;
class ExamPlugin : public IBehaviourPlugin
{
public:
	ExamPlugin();
	~ExamPlugin();

	void Start() override;
	PluginOutput Update(float dt) override;
	void ExtendUI_ImGui() override;
	void End() override;
	void ProcessEvents(const SDL_Event& e) override;

	std::vector<House*> m_houseArr;

protected:

	float m_SlowRadius = 10.f;
	float m_TargetRadius = 2.f;
	bool m_GrabAction = false;
	bool m_UseItemAction = false;
	bool m_RemoveItemAction = false;
	bool m_RunAction = false;
	int m_SelectedInventorySlot = 0;

	//decisionmaking things
	AgentInfo* m_pAgent = nullptr;
	float* m_pdTime = new float(1);
	b2Vec2 m_Target;
	b2Vec2 m_Item;
	PluginOutput* m_poutput = nullptr;
	EntityInfo* m_pEntity = nullptr;
	EntityInfo* m_pEnemy = nullptr;

	//steerinpipeline
	SteeringBehaviours::Seek* m_pSeekBehaviour = nullptr;
	SteeringBehaviours::Wander* m_pWanderBehaviour = nullptr;
	CombinedSB::SteeringPipeline* m_pSteeringPipeline = nullptr;
	CombinedSB::FixedGoalTargeter* m_pTargeter = nullptr;
	CombinedSB::AvoidAgentConstraint* m_pConstraint = nullptr;
	CombinedSB::BasicActuator* m_pActuator = nullptr;

	UtilityBuilder* m_pUtilityBuilder = nullptr;

	std::vector<ItemInfo*> m_InventoryArr;
	std::vector<EntityInfo*> m_KnowItemsArr;
	b2Vec2* m_pworldDimensions = nullptr;
	b2Vec2* m_pworldCenter = nullptr;
	int* m_pscout = nullptr;
	bool* m_pStepComplete = nullptr;
};


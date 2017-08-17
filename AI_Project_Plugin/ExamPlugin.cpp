#include "stdafx.h"
#include "ExamPlugin.h"
#include "Behaviours/SteeringBehaviours.h"
#include "Behaviours/Behaviors.h"
#include "Utility_Builder.h"
#include "mi.h"


ExamPlugin::ExamPlugin():IBehaviourPlugin(GameDebugParams(20,false,false, false))
{
}

ExamPlugin::~ExamPlugin()
{
	for(auto i = 0; i < m_houseArr.size(); ++i)
	{
		delete m_houseArr[i];
		m_houseArr[i] = nullptr;
	}

	for (auto i = 0; i < m_InventoryArr.size(); ++i)
	{
		delete m_InventoryArr[i];
		m_InventoryArr[i] = nullptr;
	}

	for (auto i = 0; i < m_KnowItemsArr.size(); ++i)
	{
		delete m_KnowItemsArr[i];
		m_KnowItemsArr[i] = nullptr;
	}
		
	delete m_pEnemy;
	m_pEnemy = nullptr;

	delete m_pEntity;
	m_pEntity = nullptr;

	delete m_poutput;
	m_poutput = nullptr;

	delete m_pdTime;
	m_pdTime = nullptr;

	delete m_pAgent;
	m_pAgent = nullptr;

	delete m_pUtilityBuilder;
	m_pUtilityBuilder = nullptr;

	delete m_pSeekBehaviour;
	m_pSeekBehaviour = nullptr;

	delete m_pWanderBehaviour;
	m_pWanderBehaviour = nullptr;

	delete m_pSteeringPipeline;
	m_pSteeringPipeline = nullptr;

	delete m_pTargeter;
	m_pTargeter = nullptr;

	delete m_pConstraint;
	m_pConstraint = nullptr;

	delete m_pActuator;
	m_pActuator = nullptr;

	delete m_pworldCenter;
	m_pworldCenter = nullptr;

	delete m_pworldDimensions;
	m_pworldDimensions = nullptr;

	delete m_pscout;
	m_pscout = nullptr;

	delete m_pStepComplete;
	m_pStepComplete = nullptr;
}

void ExamPlugin::Start()
{
	//memleak stuff:
	//_CrtSetBreakAlloc(60202);

	for(int i = 0; i < INVENTORY_GetCapacity(); ++i)
	{
		m_InventoryArr.push_back(nullptr);
	}
	for (int i = 0; i < 20; ++i)
	{
		m_KnowItemsArr.push_back(nullptr);
	}

	std::vector<EntityInfo> entities;
	std::vector<HouseInfo> houses;
	m_pEnemy = new EntityInfo();
	m_poutput = new PluginOutput();
	m_pEntity = new EntityInfo();
	// Create agent
	m_pAgent = new AgentInfo();

	m_pTargeter = new CombinedSB::FixedGoalTargeter();
	m_pConstraint = new CombinedSB::AvoidAgentConstraint();
	m_pSeekBehaviour = new SteeringBehaviours::Seek();
	m_pSeekBehaviour->SetTarget(&m_Target);
	m_pWanderBehaviour = new SteeringBehaviours::Wander();
	m_pActuator = new CombinedSB::BasicActuator(m_pSeekBehaviour, m_pWanderBehaviour);

	m_pSteeringPipeline = new CombinedSB::SteeringPipeline();
	m_pSteeringPipeline->SetTargeters({ m_pTargeter });
	m_pSteeringPipeline->SetConstraints({ m_pConstraint });
	m_pSteeringPipeline->SetActuator(m_pActuator);
	m_pSteeringPipeline->SetFallBack(m_pWanderBehaviour);

	//*** Create blackboard ***
	auto pBlackboard = new Blackboard();
	pBlackboard->AddData("Agent", m_pAgent);
	pBlackboard->AddData("Inventory", &m_InventoryArr);//cleanup
	pBlackboard->AddData("KnownItems", &m_KnowItemsArr);
	pBlackboard->AddData("TargetPosition", &m_Target);
	pBlackboard->AddData("ItemPosition", &m_Item);
	pBlackboard->AddData("TargetSet", false);
	pBlackboard->AddData("EntityInfo", entities);
	pBlackboard->AddData("Houses", houses);
	pBlackboard->AddData("Entity", m_pEntity);
	pBlackboard->AddData("EntitySet", false);
	pBlackboard->AddData("PlugIn", this);
	pBlackboard->AddData("Targetenemy", m_pEnemy);

	pBlackboard->AddData("Targeter",
		m_pTargeter);
	pBlackboard->AddData("SteeringPipeline",
		m_pSteeringPipeline);
	pBlackboard->AddData("SeekBehaviour",
		static_cast<SteeringBehaviours::ISteeringBehaviour*>(m_pSeekBehaviour));
	pBlackboard->AddData("WanderBehaviour",
		static_cast<SteeringBehaviours::ISteeringBehaviour*>(m_pWanderBehaviour));
	pBlackboard->AddData("CloseToRadius", 1.0f);
	pBlackboard->AddData("Output", m_poutput);
	pBlackboard->AddData("deltaT", m_pdTime);

	//scout
	m_pworldDimensions = new b2Vec2();
	*m_pworldDimensions = WORLD_GetInfo().Dimensions;
	m_pworldCenter = new b2Vec2();
	*m_pworldCenter = WORLD_GetInfo().Center;
	pBlackboard->AddData("WorldDimensions", m_pworldDimensions);
	pBlackboard->AddData("worldCenter", m_pworldCenter);
	m_pscout = new int();
	*m_pscout = 0;
	pBlackboard->AddData("scoutCount", m_pscout);
	m_pStepComplete = new bool();
	*m_pStepComplete = false;
	pBlackboard->AddData("StepComplete", m_pStepComplete);

	m_pUtilityBuilder = new UtilityBuilder(pBlackboard);
	m_pUtilityBuilder->BuildUtility();


}

PluginOutput ExamPlugin::Update(float dt)
{
	auto agentInfo = AGENT_GetInfo();
	*m_pAgent = agentInfo;

	auto entities = FOV_GetEntities();

	std::vector<b2Vec2> enemies;
	for(auto i : entities)
	{
		if (i.Type == eEntityType::ENEMY)
			enemies.push_back(i.Position);
	}
	m_pConstraint->GetAgentsRef() = enemies;

	auto houses = FOV_GetHouses();

	//*** Change/update blackboard data ***
	auto pBlackboard = m_pUtilityBuilder->GetBlackboard();
	if (pBlackboard)
	{
		pBlackboard->ChangeData("EntityInfo", entities);
		pBlackboard->ChangeData("Houses", houses);
	}

	if (m_pUtilityBuilder)
		m_pUtilityBuilder->UpdateUtility(m_pAgent, entities, m_houseArr,m_InventoryArr, dt);

	float* dTime = nullptr;
	pBlackboard->GetData("deltaT", dTime);
	*dTime = dt;
	m_poutput->RunMode = true;

	return *m_poutput;

}

//Extend the UI [ImGui call only!]
void ExamPlugin::ExtendUI_ImGui()
{
	ImGui::Text("Selected Slot: %i",m_SelectedInventorySlot);
}

void ExamPlugin::End()
{

}

//[Optional]> For Debugging
void ExamPlugin::ProcessEvents(const SDL_Event& e)
{
	switch(e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
	{
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			auto pos = b2Vec2(static_cast<float>(x), static_cast<float>(y));
			m_Target = DEBUG_ConvertScreenPosToWorldPos(pos);
		}
	}
		break;
	case SDL_KEYDOWN:
	{
		m_GrabAction = (e.key.keysym.sym == SDLK_SPACE);
		m_UseItemAction = (e.key.keysym.sym == SDLK_u);
		m_RemoveItemAction = (e.key.keysym.sym == SDLK_r);
		if (e.key.keysym.sym == SDLK_LCTRL)
		{
			m_RunAction = !m_RunAction;
		}

		//Slot Selection
		int currSlot = m_SelectedInventorySlot;
		m_SelectedInventorySlot = (e.key.keysym.sym == SDLK_0) ? 0 : m_SelectedInventorySlot;
		m_SelectedInventorySlot = (e.key.keysym.sym == SDLK_1) ? 1 : m_SelectedInventorySlot;
		m_SelectedInventorySlot = (e.key.keysym.sym == SDLK_2) ? 2 : m_SelectedInventorySlot;
		m_SelectedInventorySlot = (e.key.keysym.sym == SDLK_3) ? 3 : m_SelectedInventorySlot;
		m_SelectedInventorySlot = (e.key.keysym.sym == SDLK_4) ? 4 : m_SelectedInventorySlot;
	}
		break;
	}
}


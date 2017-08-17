#pragma once
#include "Utility_Selector.h"
#include "House.h"
#include "Behaviours/Behaviors.h"


class UtilityBuilder
{
	//here i update the values for all the considerations in the selector, 
	//---------------make sure they're normalized beforehand----------
	//I also call the top behaviour here with execute function which i get from the selector
public:
	UtilityBuilder(Blackboard* pBlackBoard) :
		m_pBlackboard(pBlackBoard) {}
	~UtilityBuilder()
	{
		delete m_pSelector;
		m_pSelector = nullptr;

		delete m_pBlackboard;
		m_pBlackboard = nullptr;
	}

	void BuildUtility()
	{
		//considerations:
		m_pHealthConsideration = new Utility_Consideration();
		m_pHealthConsideration->SetCurve(new LinearCurve());

		m_pFoodConsideration = new Utility_Consideration();
		m_pFoodConsideration->SetCurve(new LinearCurve());

		m_pItemInFOV = new Utility_Consideration();
		m_pItemInFOV->SetCurve(new LinearCurve());
		m_pItemInFOV->SetWeight(0.8f);

		m_pItemInReach = new Utility_Consideration();
		m_pItemInReach->SetWeight(2.0f);
		m_pItemInReach->SetCurve(new LinearCurve());

		m_pAvailableHouse = new Utility_Consideration();
		auto lin = new LinearCurve();
		lin->b = -0.4f;
		m_pAvailableHouse->SetCurve(lin);
		m_pAvailableHouse->SetWeight(1.0f);

		m_pHasFood = new Utility_Consideration();
		auto Step = new StepCurve();
		Step->a = 0.8f;
		Step->b = 1.4f;
		m_pHasFood->SetCurve(Step);

		m_pHasMedkit = new Utility_Consideration();
		auto Step3 = new StepCurve();
		Step3->a = 0.8f;
		Step3->b = 1.4f;
		m_pHasMedkit->SetWeight(0.5f);
		m_pHasMedkit->SetCurve(Step3);

		m_pHasGun = new Utility_Consideration();
		auto Step2 = new StepCurve();
		m_pHasGun->SetCurve(Step2);

		m_pEnemyInRange = new Utility_Consideration();
		m_pEnemyInRange->SetWeight(4.0f);
		m_pEnemyInRange->SetCurve(new LinearCurve());

		m_pBiasUtillity = new Utility_Consideration();
		m_pBiasUtillity->SetCurve(new LinearCurve());
		m_pBiasUtillity->SetScore(0.5f);

		//actions:
		m_pPickUpItemAction = new Utility_Action(PickUpItem, "collect item", 3);
		//m_pPickUpItemAction->AddConsideration(m_pFoodConsideration);
		m_pPickUpItemAction->AddConsideration(m_pItemInReach);

		m_pSeekItemAction = new Utility_Action(SeekItem, "seek item", 2);
		//m_pSeekItemAction->AddConsideration(m_pHealthConsideration);
		//m_pSeekItemAction->AddConsideration(m_pFoodConsideration);
		m_pSeekItemAction->AddConsideration(m_pItemInFOV);

		//default behaviour
		m_pWanderAction = new Utility_Action(Scout, "Scout", 0);
		m_pWanderAction->AddConsideration(m_pBiasUtillity);

		m_pGoIntoHouse = new Utility_Action(Seek, "seek shelter", 1);
		m_pGoIntoHouse->AddConsideration(m_pAvailableHouse);

		m_pEatFood = new Utility_Action(EatFood, "eat Food", 4);
		m_pEatFood->AddConsideration(m_pFoodConsideration);
		m_pEatFood->AddConsideration(m_pHasFood);

		m_pUseMedKit = new Utility_Action(UseMedKit, "use Medkit", 5);
		m_pUseMedKit->AddConsideration(m_pHealthConsideration);
		m_pUseMedKit->AddConsideration(m_pHasMedkit);

		m_pShootEnemy = new Utility_Action(ShootEnemy, "Shoot", 2);
		m_pShootEnemy->AddConsideration(m_pEnemyInRange);
		m_pShootEnemy->AddConsideration(m_pHasGun);

		//filling selector:
		m_pSelector = new UtilitySelector();
		m_pSelector->AddAction(m_pWanderAction);
		m_pSelector->AddAction(m_pPickUpItemAction);
		m_pSelector->AddAction(m_pSeekItemAction);
		m_pSelector->AddAction(m_pGoIntoHouse);
		m_pSelector->AddAction(m_pEatFood);
		m_pSelector->AddAction(m_pUseMedKit);
		m_pSelector->AddAction(m_pShootEnemy);

	}
	void UpdateUtility(AgentInfo* agent,
		const std::vector<EntityInfo>& entitities,
		std::vector<House*>& houseArr,
		std::vector<ItemInfo*>& inventory,
		float dTime)
	{
		//update consideration values here
		m_pHealthConsideration->SetScore(1 - (agent->Health / MAXHEALTH));
		m_pFoodConsideration->SetScore(1 - (agent->Energy / MAXFOOD));

		bool itemInFOV = false;
		std::vector<EntityInfo*>* knownItems;
		m_pBlackboard->GetData("KnownItems", knownItems);

		int closestEnemyID = -1;
		float closestDistance = 10000;
		int count = 0;
		for (auto i : entitities)
		{
			if (i.Type == ITEM)
			{
				
				bool known = false;
				int freeID = -1;
				int count = 0;
				for (auto b : *knownItems)
				{
					if (b)
					{
						if (b->Position == i.Position)
						{
							known = true;
						}
					}
					else
					{
						freeID = count;
					}
					++count;
				}
				if (!known)
					if (freeID != -1)
					{
						EntityInfo* x = new EntityInfo();
						*x = i;
						knownItems->at(freeID) = x;
					}
					else
						std::cout << "knowitems container too small to hold all items";
			}
			if (i.Type == ENEMY)
			{
				auto distanceSquared = (i.Position - agent->Position).LengthSquared();
				if (closestDistance > distanceSquared)
				{
					closestDistance = distanceSquared;
					closestEnemyID = count;
				}
			}
			++count;

		}

		//enemy
		if (closestEnemyID != -1)
		{
			EntityInfo* pEnemy = nullptr;
			m_pBlackboard->GetData("Targetenemy", pEnemy);
			pEnemy->Position = entitities[closestEnemyID].Position;
			if (closestDistance < 25)//steve only shoots in self defense
				m_pEnemyInRange->SetScore(1);
		}
		else
			m_pEnemyInRange->SetScore(-1);

		//items

		std::vector<EntityInfo*>* validKnownItems = new std::vector<EntityInfo*>();
		for (auto i  = 0; i < knownItems->size(); ++i)
		{
			if (knownItems->at(i))
			{
				validKnownItems->push_back(knownItems->at(i));
			}

		}
		validKnownItems->size() > 0 ? m_pItemInFOV->SetScore(1) : m_pItemInFOV->SetScore(-1);

		bool thing = false;
		if (validKnownItems->size() > 0)
		{
			thing = true;
			auto closeToRadius = 0.0f;
			m_pBlackboard->GetData("CloseToRadius", closeToRadius);
			int closestItem = -1;
			float shortestDistanceSqr = 1000;
			for (unsigned int i = 0; i < validKnownItems->size(); ++i)
			{
				auto distanceSquared = (validKnownItems->at(i)->Position - agent->Position).LengthSquared();
				if (shortestDistanceSqr > distanceSquared)
				{
					shortestDistanceSqr = distanceSquared;
					closestItem = i;
				}
			}
			if (closestItem >= 0)
			{
				b2Vec2* targetPos = nullptr;
				m_pBlackboard->GetData("ItemPosition", targetPos);
				*targetPos = validKnownItems->at(closestItem)->Position;
			}


			if (shortestDistanceSqr < closeToRadius * closeToRadius)
			{
				EntityInfo* entity;
				m_pBlackboard->GetData("Entity", entity);
				*entity = *validKnownItems->at(closestItem);
				m_pItemInReach->SetScore(1);
			}
			else
				m_pItemInReach->SetScore(-1);
		}
		else
			m_pItemInReach->SetScore(-1);

		delete validKnownItems;
		validKnownItems = nullptr;

		//check if gun
		std::vector<ItemInfo*>* inventoryArr = nullptr;
		m_pBlackboard->GetData("Inventory", inventoryArr);
		bool hasGun = false;
		for (auto x = 0; x < inventoryArr->size(); ++x)
		{
			if (inventoryArr->at(x))
			{
				if (inventoryArr->at(x)->Type == eItemType::PISTOL)
					hasGun = true;
			}

		}
		hasGun ? m_pHasGun->SetScore(1) : m_pHasGun->SetScore(-1);


		//houses
		HasHousesInFOV(m_pBlackboard);
		int closestHouse = -1;
		float shortestDistanceSqr = 100000;
		if (houseArr.size() > 0)
		{
			for (unsigned int i = 0; i < houseArr.size(); ++i)
			{
				int* scoutCount;
				m_pBlackboard->GetData("scoutCount", scoutCount);
				*scoutCount > 8 ? houseArr[i]->Update(dTime, agent, true) : houseArr[i]->Update(dTime, agent, false);
				
				auto distanceSquared = (houseArr[i]->m_Center - agent->Position).LengthSquared();
				if (shortestDistanceSqr > distanceSquared && !houseArr[i]->m_ReachedCenter)
				{
					shortestDistanceSqr = distanceSquared;
					closestHouse = i;
				}
			}

			if (closestHouse != -1)
			{
				b2Vec2* targetPos = nullptr;
				m_pBlackboard->GetData("TargetPosition", targetPos);
				*targetPos = houseArr[closestHouse]->m_Center;
				m_pAvailableHouse->SetScore(1);
			}
			else
				m_pAvailableHouse->SetScore(-1);

		}
		else
			m_pAvailableHouse->SetScore(-1);

		//items usage
		bool hasFood = false;
		bool hasMedkit = false;

		for (auto i : inventory)
		{
			if (i)
			{
				if (i->Type == eItemType::FOOD)
					hasFood = true;
				if (i->Type == eItemType::HEALTH)
					hasMedkit = true;
			}
		}

		hasMedkit ? m_pHasMedkit->SetScore(1.0f) : m_pHasMedkit->SetScore(-1.0f);
		hasFood ? m_pHasFood->SetScore(1.0f) : m_pHasFood->SetScore(-1.0f);

		//------------------------------------
		//get top action and execute it
		m_pSelector->GetTopAction()->Execute(m_pBlackboard);

	}
	Blackboard* GetBlackboard() const
	{
		return m_pBlackboard;
	}
private:
	Blackboard* m_pBlackboard = nullptr;
	UtilitySelector* m_pSelector = nullptr;

	//setup AI (pointers managed by selector)
	//considerations
	Utility_Consideration* m_pHealthConsideration = nullptr;
	Utility_Consideration* m_pFoodConsideration = nullptr;
	Utility_Consideration* m_pItemInFOV = nullptr;
	Utility_Consideration* m_pItemInReach = nullptr;
	Utility_Consideration* m_pAvailableHouse = nullptr;
	Utility_Consideration* m_pBiasUtillity = nullptr;
	Utility_Consideration* m_pHasFood = nullptr;
	Utility_Consideration* m_pHasMedkit = nullptr;
	Utility_Consideration* m_pHasGun = nullptr;
	Utility_Consideration* m_pEnemyInRange = nullptr;

	//actions
	Utility_Action* m_pGoIntoHouse = nullptr;
	Utility_Action* m_pPickUpItemAction = nullptr;
	Utility_Action* m_pSeekItemAction = nullptr;
	Utility_Action* m_pShootEnemy = nullptr;
	Utility_Action* m_pWanderAction = nullptr;
	Utility_Action* m_pEatFood = nullptr;
	Utility_Action* m_pUseMedKit = nullptr;

	//consts
	const float MAXHEALTH = 10.0f;
	const float MAXFOOD = 20.0f;
	const float MAXSTAMINA = 10.0f;
};

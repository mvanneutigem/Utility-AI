#pragma once
#include "stdafx.h"
#include "Blackboard.h"
#include "SteeringBehaviours.h"
#include <iostream>
#include "IBehaviourPlugin.h"
#include "mi.h"


//*** GENERAL BEHAVIORS ***
enum BehaviorState
{
	Failure,
	Success,
	Running
};

inline bool HasHousesInFOV(Blackboard* pBlackboard)
{
	//get data
#pragma region DATA
	std::vector<HouseInfo> houses;
	auto result = pBlackboard->GetData("Houses", houses);
	if (!result)
		return false;
	ExamPlugin* plugIn;
	result = pBlackboard->GetData("PlugIn", plugIn);
	if (!result)
		return Failure;
#pragma endregion
	if (houses.size() > 0)
	{
		for(auto h : houses)
		{ 
			bool exists = false;
			int index = 0;
			int count = 0;
			for(auto r : plugIn->m_houseArr)
			{
				if (r->m_Center.x == h.Center.x && r->m_Center.y == h.Center.y)
				{
					exists = true;
					index = count;
				}
				++count;
			}
			if (!exists)
			{
				House* house = new House(plugIn->WORLD_GetInfo().Dimensions);
				house->m_Center = h.Center;
				house->m_Size = h.Size;
				plugIn->m_houseArr.push_back(house);
				index = plugIn->m_houseArr.size() - 1;
			}

		}
		return true;
	}
	return false;
}

//ACTIONS

inline BehaviorState Seek(Blackboard* pBlackboard)
{
	//get data
#pragma region DATA
	AgentInfo* pAgent = nullptr;
	CombinedSB::FixedGoalTargeter* pTargeter = nullptr;
	CombinedSB::SteeringPipeline* pSteering = nullptr;
	PluginOutput* output = {};
	float* deltaT = 0;
	b2Vec2* target = nullptr;

	auto result = pBlackboard->GetData("Agent", pAgent)
		&& pBlackboard->GetData("Targeter", pTargeter)
		&& pBlackboard->GetData("SteeringPipeline", pSteering)
		&& pBlackboard->GetData("Output", output)
		&& pBlackboard->GetData("deltaT", deltaT)
		&& pBlackboard->GetData("TargetPosition", target);

	if (!result)
		return Failure;

	if (!pAgent || !pTargeter || !pSteering)
		return Failure;
	
	ExamPlugin* plugIn;
	result = pBlackboard->GetData("PlugIn", plugIn);
	if (!result)
		return Failure;
#pragma endregion 

	*target = plugIn->NAVMESH_GetClosestPathPoint(*target);
	pTargeter->GetGoalRef().Position = *target;
	pTargeter->GetGoalRef().PositionSet = true;
	*output = pSteering->CalculateSteering(*deltaT, pAgent);
	std::cout << "Seeking shelter" << std::endl;

	return Success;

}

inline BehaviorState Wander(Blackboard* pBlackboard)
{
	//get data
#pragma region DATA
	AgentInfo* pAgent = nullptr;
	ExamPlugin* plugIn = nullptr;
	CombinedSB::FixedGoalTargeter* pTargeter = nullptr;
	CombinedSB::SteeringPipeline* pSteering = nullptr;
	SteeringBehaviours::ISteeringBehaviour* pWanderBehaviour = nullptr;
	PluginOutput* output = {};
	float* deltaT = 0;

	auto result = pBlackboard->GetData("Agent", pAgent)
		&& pBlackboard->GetData("Targeter", pTargeter)
		&& pBlackboard->GetData("WanderBehaviour", pWanderBehaviour)
		&& pBlackboard->GetData("SteeringPipeline", pSteering)
		&& pBlackboard->GetData("Output", output)
		&& pBlackboard->GetData("deltaT", deltaT)
		&& pBlackboard->GetData("PlugIn", plugIn);
	if (!result)
		return Failure;
#pragma endregion 

	if (!pAgent || !pSteering || !pTargeter)
		return Failure;
	pWanderBehaviour->CalculateSteering(*deltaT, pAgent);
	auto target = plugIn->NAVMESH_GetClosestPathPoint(*pWanderBehaviour->GetTargetPos());
	pTargeter->GetGoalRef().Position = target;
	pTargeter->GetGoalRef().PositionSet = true;
	*output = pSteering->CalculateSteering(*deltaT, pAgent);
	output->AutoOrientate = true;
	std::cout << "Wandering" << std::endl;

	return Success;
}

inline BehaviorState Scout(Blackboard* pBlackboard)
{
	//get data
#pragma region DATA
	AgentInfo* pAgent = nullptr;
	b2Vec2* worldDimensions = nullptr;
	b2Vec2* worldCenter = nullptr;
	int* scout = nullptr;
	//bool* StepCompleted = nullptr;
	ExamPlugin* plugIn = nullptr;
	CombinedSB::FixedGoalTargeter* pTargeter = nullptr;
	CombinedSB::SteeringPipeline* pSteering = nullptr;
	PluginOutput* output = {};
	float* deltaT = 0;

	auto result = pBlackboard->GetData("Agent", pAgent)
		&& pBlackboard->GetData("Targeter", pTargeter)
		&& pBlackboard->GetData("WorldDimensions", worldDimensions)
		&& pBlackboard->GetData("worldCenter", worldCenter)
		&& pBlackboard->GetData("scoutCount", scout)
		&& pBlackboard->GetData("SteeringPipeline", pSteering)
		&& pBlackboard->GetData("Output", output)
		&& pBlackboard->GetData("deltaT", deltaT)
		&& pBlackboard->GetData("PlugIn", plugIn);
	if (!result)
		return Failure;
#pragma endregion 

	if (!pAgent || !pSteering || !pTargeter)
		return Failure;

	b2Vec2 target;
//x
	switch(*scout)
	{
	case 0:
	case 1:
	case 4:
		target.x = worldCenter->x - 0.2* worldDimensions->x;
		break;
	case 2:
	case 3:
		target.x = worldCenter->x + 0.2f* worldDimensions->x;
		break;
	case 5:
	case 6:
	case 9:
		target.x = worldCenter->x - 0.4f* worldDimensions->x;
		break;
	case 7:
	case 8:
		target.x = worldCenter->x + 0.4f* worldDimensions->x;
		break;
	default:
		target.x = 0;
		break;
	}

	//y
	switch (*scout)
	{
	case 0:
	case 3:
	case 4:
		target.y = worldCenter->y - 0.2* worldDimensions->y;
		break;
	case 2:
	case 1:
		target.y = worldCenter->y + 0.2f* worldDimensions->y;
		break;
	case 5:
	case 8:
	case 9:
		target.y = worldCenter->y - 0.4f* worldDimensions->y;
		break;
	case 7:
	case 6:
		target.y = worldCenter->y + 0.4f* worldDimensions->y;
		break;
	default:
		target.y = 0;
		break;
	}

	auto distanceSquared = (pAgent->Position - target).LengthSquared();
	if (distanceSquared < 50)
	{
		*scout += 1;
		if (*scout > 9)
			*scout = 0;
	}

	target = plugIn->NAVMESH_GetClosestPathPoint(target);
	pTargeter->GetGoalRef().Position = target;
	pTargeter->GetGoalRef().PositionSet = true;
	*output = pSteering->CalculateSteering(*deltaT, pAgent);
	output->AutoOrientate = true;
	std::cout << "Scouting step: " << *scout << std::endl;

	return Success;
}


inline BehaviorState PickUpItem(Blackboard* pBlackboard)
{
#pragma region DATA
	EntityInfo* entity;
	std::vector<ItemInfo*>* inventoryArr;
	std::vector<EntityInfo*>* knownItems;
	auto result = pBlackboard->GetData("Entity", entity)
				&& pBlackboard->GetData("KnownItems", knownItems)
				&& pBlackboard->GetData("Inventory", inventoryArr);
	if (!result)
		return Failure;

	ItemInfo item;
	ExamPlugin* plugIn;
	result = pBlackboard->GetData("PlugIn", plugIn);
	if (!result)
		return Failure;

#pragma endregion 
	std::cout << "Picking up item" << std::endl;

	std::vector<ItemInfo*> inventoryRef = *inventoryArr;

	auto succes = plugIn->ITEM_Grab(*entity, item);

	if(succes)
	{
		for (auto i = 0; i < knownItems->size(); ++i)
		{
			if (knownItems->operator[](i))
			{
				//known bug: when multiple items on top of eachother will be seen as one because the hash keeps changing
				if (knownItems->operator[](i)->Position == entity->Position)
				{
					delete knownItems->operator[](i);
					knownItems->operator[](i) = nullptr;
				}

			}

		}

		int slotID = -1;
		int medCount = 0;
		int foodCount = 0;
		int gunCount = 0;
		for (unsigned int i = 0; i < inventoryRef.size(); ++i)
		{
			if (inventoryArr->at(i) == nullptr || inventoryArr->at(i)->Type < 0)
			{
				slotID = i;
			}
			else
			{
				if (inventoryRef[i]->Type == eItemType::PISTOL)
					++gunCount;
				if (inventoryRef[i]->Type == eItemType::HEALTH)
					++medCount;
				if (inventoryRef[i]->Type == eItemType::FOOD)
					++foodCount;
			}

		}

		if (slotID != -1)
		{
			//keeping one slot open for throwing stuff away
			if (item.Type != eItemType::GARBAGE &&
				!(gunCount > 0 && item.Type == eItemType::PISTOL)
				&& !(foodCount > 0 && item.Type == eItemType::FOOD)
				&& !(medCount > 1 && item.Type == eItemType::HEALTH))
			{
				plugIn->INVENTORY_AddItem(slotID, item);
				auto itemptr = new ItemInfo();
				itemptr->Type = item.Type;
				itemptr->ItemHash = item.ItemHash;
				inventoryArr->at(slotID) = itemptr;

			}
			else
			{
				plugIn->INVENTORY_AddItem(slotID, item);
				plugIn->INVENTORY_RemoveItem(slotID);
			}
		}
	}
	

	return Success;
}

inline BehaviorState EatFood(Blackboard* pBlackboard)
{
#pragma region DATA
	std::vector<ItemInfo*>* inventoryArr;
	ExamPlugin* plugIn;
	auto result = pBlackboard->GetData("Inventory", inventoryArr)
				&& pBlackboard->GetData("PlugIn", plugIn);
	if (!result)
		return Failure;

#pragma endregion 

	std::vector<ItemInfo*> inventoryRef = *inventoryArr;

	int count = 0;
	for(auto i : inventoryRef)
	{
		if(i!=nullptr)
		{
			if (i->Type == eItemType::FOOD)
			{
				ItemInfo item;
				plugIn->INVENTORY_GetItem(count, item);
				plugIn->INVENTORY_UseItem(count);
				int amount;
				plugIn->ITEM_GetMetadata(item, "energy", amount);

				plugIn->INVENTORY_RemoveItem(count);
				delete inventoryArr->at(count);
				inventoryArr->at(count) = nullptr;
					
				break;
			}
		}
			
		++count;
	}

	std::cout << "eating food" << std::endl;
	return Success;
}

inline BehaviorState UseMedKit(Blackboard* pBlackboard)
{
#pragma region DATA
	std::vector<ItemInfo*>* inventoryArr;
	ExamPlugin* plugIn;
	auto result = pBlackboard->GetData("Inventory", inventoryArr)
		&& pBlackboard->GetData("PlugIn", plugIn);
	if (!result)
		return Failure;

#pragma endregion 

	std::vector<ItemInfo*> inventoryRef = *inventoryArr;

	int count = 0;
	for (auto i : inventoryRef)
	{
		if (i != nullptr)
		{
			if (i->Type == eItemType::HEALTH)
			{

				ItemInfo item;
				plugIn->INVENTORY_GetItem(count, item);
				plugIn->INVENTORY_UseItem(count);
				int amount;
				plugIn->ITEM_GetMetadata(item, "health", amount);

				delete inventoryArr->at(count);
				inventoryArr->at(count) = nullptr;
				plugIn->INVENTORY_RemoveItem(count);

				break;

			}
		}
		++count;
	}

	std::cout << "using medkit" << std::endl;
	return Success;
}

inline BehaviorState SeekItem(Blackboard* pBlackboard)
{
	//get data
#pragma region DATA
	AgentInfo* pAgent = nullptr;
	CombinedSB::FixedGoalTargeter* pTargeter = nullptr;
	CombinedSB::SteeringPipeline* pSteering = nullptr;
	PluginOutput* output = {};
	float* deltaT = 0;
	b2Vec2* target = nullptr;

	auto result = pBlackboard->GetData("Agent", pAgent)
		&& pBlackboard->GetData("Targeter", pTargeter)
		&& pBlackboard->GetData("SteeringPipeline", pSteering)
		&& pBlackboard->GetData("Output", output)
		&& pBlackboard->GetData("deltaT", deltaT)
		&& pBlackboard->GetData("ItemPosition", target);

	if (!result)
		return Failure;

	if (!pAgent || !pTargeter || !pSteering)
		return Failure;

	ExamPlugin* plugIn;
	result = pBlackboard->GetData("PlugIn", plugIn);
	if (!result)
		return Failure;
#pragma endregion 

	*target = plugIn->NAVMESH_GetClosestPathPoint(*target);
	pTargeter->GetGoalRef().Position = *target;
	pTargeter->GetGoalRef().PositionSet = true;
	*output = pSteering->CalculateSteering(*deltaT, pAgent);
	output->AutoOrientate = true;
	std::cout << "Seeking item" << std::endl;

	return Success;

}

inline BehaviorState ShootEnemy(Blackboard* pBlackboard)
{
	//get data
#pragma region DATA
	std::vector<ItemInfo*>* inventoryArr;
	AgentInfo* pAgent = nullptr;
	CombinedSB::FixedGoalTargeter* pTargeter = nullptr;
	CombinedSB::SteeringPipeline* pSteering = nullptr;
	PluginOutput* output = {};
	float* deltaT = 0;
	EntityInfo* pEnemy;

	auto result = pBlackboard->GetData("Agent", pAgent)
		&& pBlackboard->GetData("Inventory", inventoryArr)
		&& pBlackboard->GetData("Targeter", pTargeter)
		&& pBlackboard->GetData("SteeringPipeline", pSteering)
		&& pBlackboard->GetData("Output", output)
		&& pBlackboard->GetData("deltaT", deltaT)
		&& pBlackboard->GetData("Targetenemy", pEnemy);

	if (!result)
		return Failure;

	if (!pAgent || !pTargeter || !pSteering)
		return Failure;

	ExamPlugin* plugIn;
	result = pBlackboard->GetData("PlugIn", plugIn);
	if (!result)
		return Failure;
#pragma endregion 

	b2Vec2 target = pEnemy->Position;
	
	auto a2g = pTargeter->GetGoalRef().Position - pAgent->Position;
	
	if (a2g.LengthSquared() > 10)//if this close then just go
	{
		auto a2s = pEnemy->Position - pAgent->Position;
		auto moveDirection = a2g;
		moveDirection.Normalize();

		auto distanceToClosest = b2Dot(a2s, moveDirection);
		auto d = a2s.Length() - (distanceToClosest);

		//check for collision
		auto radius = 10;//large base margin
		if (d < radius)
		{
			if (distanceToClosest > 0.f )
			{
				//Find closest point
				auto closestPoint = pAgent->Position + (distanceToClosest * moveDirection);

				//find the point of avoidance
				auto offsetDir = closestPoint - pEnemy->Position;
				offsetDir.Normalize();
				auto t = plugIn->NAVMESH_GetClosestPathPoint(pEnemy->Position + (radius * offsetDir));
				pTargeter->GetGoalRef().Position = t;
				pTargeter->GetGoalRef().PositionSet = true;
			}
		}

	}
	
	*output = pSteering->CalculateSteering(*deltaT, pAgent);
	output->AutoOrientate = false;

	int pistolID = -1;
	for (unsigned int i = 0; i < inventoryArr->size(); ++i)
	{
		if(inventoryArr->at(i))
		{
			if (inventoryArr->at(i)->Type == eItemType::PISTOL)
				pistolID = i;
		}

	}

	auto vec = target - pAgent->Position;
	vec.Normalize();
	auto zerovec = b2Vec2(1, 0);
	auto dot = b2Dot(vec, zerovec);
	auto orient = acos(dot);
	auto goalOrient = pAgent->Orientation -orient;
	output->AngularVelocity = goalOrient;

	std::cout << "Shooting gun" << std::endl;

	ItemInfo item;
	if (pistolID != -1)// && (abs(output->AngularVelocity - orient)) < 0.3)
	{
		plugIn->INVENTORY_GetItem(pistolID, item);
		plugIn->INVENTORY_UseItem(pistolID);
		int amount;
		plugIn->ITEM_GetMetadata(item, "ammo", amount);
		if (amount <= 0 )
		{
			if (inventoryArr->at(pistolID))
			{
				delete inventoryArr->at(pistolID);
				inventoryArr->at(pistolID) = nullptr;
				plugIn->INVENTORY_RemoveItem(pistolID);
			}

		}
	}


	return Success;

}


inline BehaviorState Rest(Blackboard* pBlackboard)
{
#pragma region DATA
	AgentInfo* pAgent = nullptr;
	CombinedSB::FixedGoalTargeter* pTargeter = nullptr;
	CombinedSB::SteeringPipeline* pSteering = nullptr;
	PluginOutput* output = {};
	float* deltaT = 0;
	b2Vec2* target = nullptr;

	auto result = pBlackboard->GetData("Agent", pAgent)
		&& pBlackboard->GetData("Targeter", pTargeter)
		&& pBlackboard->GetData("SteeringPipeline", pSteering)
		&& pBlackboard->GetData("Output", output)
		&& pBlackboard->GetData("deltaT", deltaT)
		&& pBlackboard->GetData("ItemPosition", target);

	if (!result)
		return Failure;

	if (!pAgent || !pTargeter || !pSteering)
		return Failure;

	ExamPlugin* plugIn;
	result = pBlackboard->GetData("PlugIn", plugIn);
	if (!result)
		return Failure;
#pragma endregion 

	*target = pAgent->Position;
	pTargeter->GetGoalRef().Position = *target;
	pTargeter->GetGoalRef().PositionSet = true;
	*output = pSteering->CalculateSteering(*deltaT, pAgent);
	output->AutoOrientate = true;

	std::cout << "resting" << std::endl;

	return Success;

}

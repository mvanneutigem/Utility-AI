#pragma once
//#include "TestBoxPlugin.h"
#include "IBehaviourPlugin.h"

namespace SteeringBehaviours
{
	#pragma region **ISTEERINGBEHAVIOUR** (BASE)
	class ISteeringBehaviour
	{
	public:
		ISteeringBehaviour() {}
		ISteeringBehaviour(Context *pContext):m_pContext(pContext){}
		virtual ~ISteeringBehaviour(){}

		virtual PluginOutput CalculateSteering(float deltaT, AgentInfo* pAgent) = 0;

		template<class T, typename std::enable_if<std::is_base_of<ISteeringBehaviour, T>::value>::type* = nullptr>
		T* As()
		{
			return static_cast<T*>(this);
		}
		const b2Vec2* GetTargetPos() const { return m_pTargetRef; }

	protected:
		Context *m_pContext = nullptr;
		const b2Vec2* m_pTargetRef;
	};
	#pragma endregion

	//SEEK
	//****
	class Seek : public ISteeringBehaviour
	{
	public:
		Seek() {};
		virtual ~Seek() {};

		//Seek Behaviour
		PluginOutput CalculateSteering(float deltaT, AgentInfo* pAgent) override;

		//Seek Functions
		virtual void SetTarget(const b2Vec2* pTarget) { m_pTargetRef = pTarget; }

	protected:
		//const b2Vec2* m_pTargetRef;
	};

	//FLEE
	//****
	class Flee : public Seek
	{
		public:
			Flee() {};
			virtual ~Flee() {};

			//Seek Behaviour
			PluginOutput CalculateSteering(float deltaT, AgentInfo* pAgent) override;
	};

	//WANDER
	//******
	class Wander : public Seek
	{
		public:
			Wander() {};
			Wander(Context* pContext){ m_pContext = pContext; };
			virtual ~Wander() {};

			//Wander Behaviour
			PluginOutput CalculateSteering(float deltaT, AgentInfo* pAgent) override;

			void SetWanderOffset(float offset) { m_Offset = offset; }
			void SetWanderRadius(float radius) { m_Radius = radius; }
			void SetMaxAngleChange(float rad) { m_AngleChange = rad; }
			
	protected:

		float m_Offset = 6.f; //Offset (Agent Direction)
		float m_Radius = 4.f; //WanderRadius
		float m_AngleChange = ToRadians(45); //Max WanderAngle change per frame
		float m_WanderAngle = 0.f; //Internal

	private:
		void SetTarget(const b2Vec2* pTarget) override {} //Hide SetTarget, No Target needed for Wander
	};

	//ARRIVE
	//******
	class Arrive : public ISteeringBehaviour
	{
		public:
			Arrive() {};
			virtual ~Arrive() {};

			//Seek Behaviour
			PluginOutput CalculateSteering(float deltaT, AgentInfo* pAgent) override;

			//Seek Functions
			virtual void SetTarget(const b2Vec2* pTarget) { m_pTargetRef = pTarget; }
			void SetSlowRadius(float radius) { m_SlowRadius = radius; }

		protected:

			const b2Vec2* m_pTargetRef;
			float m_SlowRadius = 10.f;
			float m_TargetRadius = 2.f;
	};


}


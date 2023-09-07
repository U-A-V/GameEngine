#pragma once
#include "entt.hpp"
#include "Engine/Core/TimeStamp.h"

namespace Engine {
	class Scene {
	public:
		Scene();
		~Scene();

		entt::entity CreateEntity();

		entt::registry& Reg() { return m_Registry; }

		void OnUpdate(TimeStamp ts);
	private:
		entt::registry m_Registry;
	};
}
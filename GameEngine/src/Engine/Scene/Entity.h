#pragma once
#include "Engine/Scene/Scene.h"

#include <entt.hpp>

namespace Engine {
	class Entity {
	public:
		Entity() = default;
		Entity(entt::entity, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent() {
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args){
			EG_CORE_ASSERT(!HasComponent<T>(), "Entity already has component! {0}",T);
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent() {
			EG_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component! {0}", T);
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent() {
			EG_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component! {0}", T);
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !((*this)==other); }
	private:
		entt::entity m_EntityHandle = entt::null;
		Scene* m_Scene = nullptr;
	};

}
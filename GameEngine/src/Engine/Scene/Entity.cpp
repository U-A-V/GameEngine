#include "hzpch.h"
#include "Engine/Scene/Entity.h"

namespace Engine {
	Entity::Entity(entt::entity entity, Scene* scene)
		:m_EntityHandle(entity),m_Scene(scene){

	}
}
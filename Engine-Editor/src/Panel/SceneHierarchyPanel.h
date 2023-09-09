#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

namespace Engine {
	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponent(Entity entity);
	private:

		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}
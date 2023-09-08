#include "hzpch.h"

#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"

#include "Engine/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Engine {
	Scene::Scene() {

#if 0
		entt::entity entity = m_Registry.create();

		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view) {
			TransformComponent& tranform = m_Registry.get<TransformComponent>(entity);
		}
		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group) {
			auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}
#endif
	}

	Scene::~Scene() {

	}
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity =  { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}
	void Scene::OnUpdate(TimeStamp ts) {
		//Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		{
			auto view = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
			for (auto entity : view) {
				auto& [camera, transform] = m_Registry.get<CameraComponent, TransformComponent>(entity);
				if (camera.Primary) {
					mainCamera = &camera.Camera;
					cameraTransform = &transform.Transform;
					break;
				}
			}
		}

		if (mainCamera) {
			Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group) {
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			
				Renderer2D::DrawQuad(transform, sprite.Color);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view) {
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio) {
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}

}
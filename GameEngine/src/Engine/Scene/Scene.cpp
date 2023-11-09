#include "hzpch.h"

#include "Engine/Scene/Scene.h"
#include "Engine/Scene/ScriptableEntity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"

#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Renderer/Renderer3D.h"

#include <glm/glm.hpp>

//box2d
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>


namespace Engine {

	static b2BodyType Enginerb2dtypeTob2bType(Rigidbody2DComponent::BodyType bodyType) {
		switch (bodyType) {
			case Rigidbody2DComponent::BodyType::Static:	return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:	return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic:	return b2_kinematicBody;
		}

		EG_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	Scene::Scene() {

	}

	Scene::~Scene() {

	}

	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src,const std::unordered_map<UUID, entt::entity>& enttMap) {
		auto view = src.view<Component>();
		for (auto e : view) {
			UUID uuid = src.get<IDComponent>(e).ID;
			EG_CORE_ASSERT(enttMap.find(uuid) != enttMAp.end());
			entt::entity dstEnttID = enttMap.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEnttID, component);

		}
	}
	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src) {
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other) {
		Ref<Scene> newScene = CreateRef<Scene>();
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView) {
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<PointLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CubeRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SphereRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}
	void Scene::DestroyEntity(Entity entity) {
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart() {
		m_PhysicsWorld = new b2World({ 0.0f,-9.8f });
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view) {
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Enginerb2dtypeTob2bType(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);

			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>()) {
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
				
			}
			if (entity.HasComponent<CircleCollider2DComponent>()) {

				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnRuntimeStop() {
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnUpdateEditor(TimeStamp ts, EditorCamera& camera) {
		Renderer2D::BeginScene(camera);
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group) {
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}

		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view) {
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		Renderer2D::EndScene();
		Renderer3D::BeginScene(camera);

		{
			auto view = m_Registry.view<TransformComponent, PointLightComponent>();
			for (auto entity : view) {
				auto [transform, light] = view.get<TransformComponent, PointLightComponent>(entity);
				Renderer3D::AddPointLight(transform.GetTransform(), light, (int)entity);
			}
		}
		{
			auto view = m_Registry.view<TransformComponent, SphereRendererComponent>();
			for (auto entity : view) {
				auto [transform, sphere] = view.get<TransformComponent, SphereRendererComponent>(entity);

				Renderer3D::DrawSphere(transform.GetTransform(), sphere, (int)entity);
			}
		}
		{
			auto view = m_Registry.view<TransformComponent, CubeRendererComponent>();
			for (auto entity : view) {
				auto [transform, cube] = view.get<TransformComponent, CubeRendererComponent>(entity);

				Renderer3D::DrawCube(transform.GetTransform(), cube.Color, (int)entity);
			}
		}
		Renderer3D::EndScene();
	}

	void Scene::OnUpdateRuntime(TimeStamp ts) {
		//update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
				if (!nsc.Instance) {
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}
				nsc.Instance->OnUpdate(ts);
			});
		}
		//Physics
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIteration = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIteration);

			//Retrieve transform from Box2D

			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view) {
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
				
				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}

		}


		//Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<CameraComponent, TransformComponent>();
			for (auto entity : view) {
				auto [camera, transform] = view.get<CameraComponent, TransformComponent>(entity);
				if (camera.Primary) {
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera) {
			Renderer2D::BeginScene(*mainCamera, cameraTransform);
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group) {
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			
					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
			}

			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view) {
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
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

	void Scene::DuplicateEntity(Entity entity) {
		Entity newEntity = CreateEntity(entity.GetName());
		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<PointLightComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CubeRendererComponent>(newEntity, entity);
		CopyComponentIfExists<SphereRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);

	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view) {
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity(entity, this);
		}
		return {};
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) {
		//static_assert(false);
	}
	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
	

	template<>
	void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<CubeRendererComponent>(Entity entity, CubeRendererComponent& component) {

	}
	

	template<>
	void Scene::OnComponentAdded<SphereRendererComponent>(Entity entity, SphereRendererComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component) {

	}
}
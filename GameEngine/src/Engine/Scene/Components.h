#pragma once

#include "Engine/Scene/SceneCamera.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Core/UUID.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Engine {
	struct MaterialComponent {
		glm::vec4 Albedo{ 1.0f };
		float Roughness = 0.5f;
		float Metallic = 0.5f;

		uint32_t MaterialID = 0;
	};

	struct IDComponent {
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent {
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			:Tag(tag){}
	};

	struct TransformComponent {
		glm::vec3 Translation = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);


		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3 & translation)
			:Translation(translation) {}
		
		glm::mat4 GetTransform() const {

			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent {

		glm::vec4 Color{0.8f,0.8f,0.8f,1.0f};
		float TilingFactor = 1.0f;
		Ref<Texture2D> Texture;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 & color)
			:Color(color) {}
	};

	struct CircleRendererComponent {

		glm::vec4 Color{0.8f,0.8f,0.8f,1.0f};
		float Radius = 0.5f;
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct CubeRendererComponent {
		glm::vec4 Color{ 0.8f,0.8f,0.8f,1.0f };

		CubeRendererComponent() = default;
		CubeRendererComponent(const CubeRendererComponent&) = default;
		CubeRendererComponent(const glm::vec4& color)
			:Color(color) {}
	};

	struct SphereRendererComponent {
		glm::vec4 Color{ 0.8f, 0.8f, 0.8f, 1.0f };
		float Radius = 0.5f;
		int sectorCount = 32;
		int stackCount = 16;
		bool batch = false;
		SphereRendererComponent() = default;
		SphereRendererComponent(const SphereRendererComponent&) = default;
		SphereRendererComponent(const glm::vec4& color)
			:Color(color) {}

	};

	//Camera
	struct CameraComponent {
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

	};

	//lights
	struct PointLightComponent {
		float constant = 0.5f;
		float linear = 0.15f;
		float quadratic = 0.1f;

		glm::vec3 ambient = glm::vec3(0.5f);
		glm::vec3 diffuse = glm::vec3(0.85f);
		glm::vec3 specular = glm::vec3(0.85f);

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
	};

	//script

	class ScriptableEntity;
	struct NativeScriptComponent {

		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(* InstantiateScript)();
		void( *DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind() {

			InstantiateScript = []() {return static_cast<ScriptableEntity* >(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr; };

		}
	};


	//Physics

	struct Rigidbody2DComponent {
		enum class BodyType {
			Static= 0, Dynamic, Kinematic,
		};
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		//Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};



	struct BoxCollider2DComponent {
		glm::vec2 Offset = { 0.0f,0.0f };
		glm::vec2 Size = { 0.5f,0.5f };
	
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		//Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent {

		glm::vec2 Offset = { 0.0f,0.0f };
		float Radius = 0.5f;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		//Storage for runtime
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};
}
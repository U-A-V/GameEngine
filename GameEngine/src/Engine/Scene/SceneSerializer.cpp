#include "hzpch.h"

#include "SceneSerializer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"


#include <fstream>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>


//yaml defs
#define KEY(x) << YAML::Key << x
#define VAL(x) << YAML::Value << x
#define KEYVAL(x,y) KEY(x)VAL(y)
#define BEGIN_MAP <<YAML::BeginMap
#define END_MAP <<YAML::EndMap

namespace YAML {
	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}
		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}
		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec4>{
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}
		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Engine {


	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static std::string rb2dBodyTypetoString(Rigidbody2DComponent::BodyType bodyType) {
		switch (bodyType) {
			case Rigidbody2DComponent::BodyType::Static:	return "Static";
			case Rigidbody2DComponent::BodyType::Dynamic:	return "Dynamic";
			case Rigidbody2DComponent::BodyType::Kinematic:	return "Kinematic";
		}
		EG_CORE_ASSERT(false, "Unknown Body Type");
		return "Static";
	}

	static Rigidbody2DComponent::BodyType rb2dBodyTypeFormString(const std::string& bodyTypeString) {
		
		if( bodyTypeString == "Static")		return Rigidbody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic")	return Rigidbody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic")	return Rigidbody2DComponent::BodyType::Kinematic;
		else {
			EG_CORE_ASSERT(false, "Unknown Body Type");
			return Rigidbody2DComponent::BodyType::Static;
		}
	}

	SceneSerializer::SceneSerializer(const Ref<Scene> scene)
		:m_Scene(scene)
	{
	}
	static void SerializeEntity(YAML::Emitter& out, Entity entity) {
		EG_CORE_ASSERT(entity.HasComponent<IDComponent>());

		out BEGIN_MAP; //Entity
		out KEYVAL("Entity" ,entity.GetUUID()); //Entity id goes here

		if (entity.HasComponent<TagComponent>()) {
			out KEY("TagComponent");
			out BEGIN_MAP;
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out KEYVAL("Tag", tag);
			out END_MAP;
		}

		if (entity.HasComponent<TransformComponent>()) {
			out KEY("TransformComponent");
			out BEGIN_MAP;

			auto& tc = entity.GetComponent<TransformComponent>();
			out KEYVAL("Translation", tc.Translation);
			out KEYVAL("Rotation", tc.Rotation);
			out KEYVAL("Scale", tc.Scale);

			out END_MAP;
		}

		if (entity.HasComponent<CameraComponent>()) {
			out KEY("CameraComponent");
			out BEGIN_MAP;

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out KEY("Camera")<<YAML::Value;
			out BEGIN_MAP; //camera
			out KEYVAL("ProjectionType",(int)camera.GetProjectionType());
			out KEYVAL("PerspectiveFOV", camera.GetPerspectiveVerticalFov());
			out KEYVAL("PerspectiveNear",camera.GetPerspectiveNearClip() );
			out KEYVAL("PerspectiveFar",camera.GetPerspectiveFarClip() );
			out KEYVAL("OrthographicSize", camera.GetOrthographicSize());
			out KEYVAL("OrthographicNear", camera.GetOrthographicNearClip());
			out KEYVAL("OrthographicFar", camera.GetOrthographicFarClip());
			out END_MAP; //camera

			out KEYVAL("Primary", cameraComponent.Primary);
			out KEYVAL("FixedAspectRatio", cameraComponent.FixedAspectRatio);
			
			out END_MAP;
		}

		if (entity.HasComponent<SpriteRendererComponent>()) {
			out KEY("SpriteRendererComponent");
			out BEGIN_MAP;

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out KEYVAL("Color", spriteRendererComponent.Color);
			out END_MAP;
		}

		if (entity.HasComponent<Rigidbody2DComponent>()) {
			out KEY("Rigidbody2DComponent");
			out BEGIN_MAP;
			auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
			out KEYVAL("BodyType", rb2dBodyTypetoString(rb2dComponent.Type));
			out KEYVAL("FixedRotation", rb2dComponent.FixedRotation);
			out END_MAP;
		}

		if (entity.HasComponent<BoxCollider2DComponent>()) {
			out KEY("BoxCollider2DComponent");
			out BEGIN_MAP;
			auto& bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
			out KEYVAL("Offset", bc2dComponent.Offset);
			out KEYVAL("Size", bc2dComponent.Size);
			out KEYVAL("Density", bc2dComponent.Density);
			out KEYVAL("Friction", bc2dComponent.Friction);
			out KEYVAL("Restitution", bc2dComponent.Restitution);
			out KEYVAL("RestitutionThreshold", bc2dComponent.RestitutionThreshold);

			out END_MAP;
		}
		out END_MAP;
	}
	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out BEGIN_MAP;
		out KEYVAL("Scene","Untitled");
		out KEYVAL("Entities", YAML::BeginSeq);

		m_Scene->m_Registry.each([&](auto entityID) {
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;

			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out END_MAP;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}
	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		//not implemented
		EG_CORE_ASSERT(false);
	}
	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream strstream;
		strstream << stream.rdbuf();

		YAML::Node data = YAML::Load(strstream.str());
		if(!data["Scene"])
			return false;
		std::string sceneName = data["Scene"].as<std::string>();
		EG_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities) {
			for (auto entity : entities) {
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();
				EG_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntity(name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent) {
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
					EG_CORE_TRACE("Deserialized Transform component");
				}
				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent) {
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFov(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());
					
					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
					EG_CORE_TRACE("Deserialized Camera component");
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent) {
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					EG_CORE_TRACE("Deserialized Sprite component");
				}

				auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
				if (rigidbody2DComponent) {
					auto& src = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					src.Type = rb2dBodyTypeFormString(rigidbody2DComponent["BodyType"].as<std::string>());
					src.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
					EG_CORE_TRACE("Deserialized Rigidbody2D component");
				}

				auto bc2dComponent = entity["BoxCollider2DComponent"];
				if (bc2dComponent) {
					auto& src = deserializedEntity.AddComponent<BoxCollider2DComponent>();

					src.Offset = bc2dComponent["Offset"].as<glm::vec2>();
					src.Size = bc2dComponent["Size"].as<glm::vec2>();
					src.Density = bc2dComponent["Density"].as<float>();
					src.Friction = bc2dComponent["Friction"].as<float>();
					src.Restitution = bc2dComponent["Restitution"].as<float>();
					src.RestitutionThreshold = bc2dComponent["RestitutionThreshold"].as<float>();
					EG_CORE_TRACE("Deserialized BoxCollider2D Component");

				}
			}
		}
		return true;
	}
	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		//not implemented
		EG_CORE_ASSERT(false);
		return false;
	}
}
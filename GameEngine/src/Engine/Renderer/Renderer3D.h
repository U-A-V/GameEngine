#pragma once
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/SubTexture2D.h"
#include "Engine/Scene/Components.h"	

namespace Engine {
	class Renderer3D {
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();
		static void StartBatch();
		static void NextBatch();
		static void Flush();

		//primitives
		static void DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		static void DrawSphere(const glm::mat4& transfomr,const SphereRendererComponent& sphere, int entityID = -1);


		static void AddPointLight(const glm::mat4& transform, const PointLightComponent& light, int entityID=-1);

		static void CreateCubeMap(std::string& path);

		static void SetSkyBoxRenderer(bool canRender);

		struct Statistics {
			uint32_t DrawCalls = 0;
			uint32_t CubeCount = 0;
			uint32_t SphereCount = 0;
			uint32_t SphereIndexCount = 0;
			uint32_t SphereVertexCount = 0;

			uint32_t GetTotalVertexCount() { return ((SphereCount * SphereVertexCount) + (CubeCount * 4)); }
			uint32_t GetTotalIndexCount() { return ((CubeCount * 36) + (SphereCount * SphereIndexCount)); }
		};
		static Statistics GetStats();
		static void ResetStats();

	};
}
#pragma once
#include "Engine/Renderer/OrthographicCamera.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/SubTexture2D.h"
#include "Engine/Scene/Components.h"	
namespace Engine{

	class Renderer2D {
	public:

		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void StartBatch();
		static void NextBatch();
		static void Flush();

		//primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor = 1.0f);

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, int entityID = -1);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size,float rotation, const Ref<Texture2D>& texture, float tilingFactor=1.0f);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size,float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor=1.0f);

		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size,float rotation, const Ref<Texture2D>& texture, float tilingFactor=1.0f);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size,float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor=1.0f);

		static void DrawSprite(const glm::mat4& transform,SpriteRendererComponent& sprite, int entityID );

		static void DrawCircle(const glm::mat4& transform,const glm::vec4& color, float thickness=1.0f,float fade=0.005f, int entityID=-1 );
		static void DrawLine(const glm::vec3& p0,const glm::vec3& p1,const glm::vec4& color, int entityID=-1 );

		static float GetLineWidth();
		static void SetLineWidth(float width);

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID);

		//Stats
		struct Statistics {
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};
		static Statistics GetStats();
		static void ResetStats();
	private:
	};
}
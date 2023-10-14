#include "hzpch.h"

#include "Renderer2D.h"
#include "Engine/Renderer/UniformBuffer.h"
#include "RenderCommand.h"
#include "VertexArray.h"
#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine {
	struct QuadVertex {
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		//editor only
		int EntityID;

	};

	struct CircleVertex {
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;

		float Thickness;
		float Fade;

		//editor only
		int EntityID;
	};

	struct Renderer2DStorage {
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTexturesSlots = 32;

		//Quad
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		//Circle
		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTexturesSlots> TexturesSlots;
		uint32_t TextureSlotIndex = 1; //slot 0 == whiteTexture;

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;

		struct CameraData {
			glm::mat4 ViewProjection;
		};

		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUnifromBuffer;
	};
	static Renderer2DStorage s_Data;

	void Renderer2D::Init()
	{
		EG_PROFILE_FUNCTION();

		//Quads
		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

		s_Data.QuadVertexBuffer->SetLayout({
			{ShaderDataType::Float3,	"a_Position"},
			{ShaderDataType::Float4,	"a_Color"},
			{ShaderDataType::Float2,	"a_TexCoord"},
			{ShaderDataType::Float,		"a_TexIndex"},
			{ShaderDataType::Float,		"a_TilingFactor"},
			{ShaderDataType::Int,		"a_EntityID"},
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);

		//Circles
		s_Data.CircleVertexArray = VertexArray::Create();

		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ShaderDataType::Float3,	"a_WorldPosition"	},
			{ShaderDataType::Float3,	"a_LocalPosition"	},
			{ShaderDataType::Float4,	"a_Color"			},
			{ShaderDataType::Float,		"a_Thickness"		},
			{ShaderDataType::Float,		"a_Fade"			},
			{ShaderDataType::Int,		"a_EntityID"		},
		});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.CircleVertexArray->SetIndexBuffer(quadIB); //use quadIB
		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];

		delete[] quadIndices;

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int samplers[s_Data.MaxTexturesSlots];
		for (uint32_t i = 0; i < s_Data.MaxTexturesSlots; i++)
			samplers[i] = i;

		s_Data.QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");
		s_Data.CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");

		s_Data.TexturesSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f,	-0.5f,	0.0f,	1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f,		-0.5f,	0.0f,	1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,		0.5f,	0.0f,	1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,	0.5f,	0.0f,	1.0f };

		s_Data.CameraUnifromBuffer = UniformBuffer::Create(sizeof(Renderer2DStorage::CameraData), 0);
	}
	void Renderer2D::Shutdown()
	{
		EG_PROFILE_FUNCTION();
	}
	void Renderer2D::BeginScene(const EditorCamera& camera) {
		EG_PROFILE_FUNCTION();
		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUnifromBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DStorage::CameraData));

		StartBatch();

	}
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		EG_PROFILE_FUNCTION();

		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetMat4(camera.GetViewProjectionMatrix(),"u_ViewProjection");

		StartBatch();

	}	
	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		EG_PROFILE_FUNCTION();
		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUnifromBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DStorage::CameraData));
		StartBatch();
	}
	void Renderer2D::EndScene()
	{
		EG_PROFILE_FUNCTION();
		Flush();

	}
	void Renderer2D::StartBatch() {

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.TextureSlotIndex = 1;

	}
	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount) {

			uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			//Bind Textxures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TexturesSlots[i]->Bind(i);
			s_Data.QuadShader->Bind();
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
		}
		if (s_Data.CircleIndexCount) {
			uint32_t dataSize = (uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase;
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

			s_Data.CircleShader->Bind();
			RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
			s_Data.Stats.DrawCalls++;
		}
	}

	void Renderer2D::NextBatch() {
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y, 0.0f }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		EG_PROFILE_FUNCTION();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
		DrawQuad(transform, color);

	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor)
	{
		DrawQuad({ position.x,position.y, 0.0f }, size, texture, tilingFactor);

	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor)
	{
		EG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
		DrawQuad(transform, texture, tilingFactor);

	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor) {
		DrawQuad({ position.x,position.y, 0.0f }, size, subtexture, tilingFactor);

	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor) {
		EG_PROFILE_FUNCTION();
		if (s_Data.QuadIndexCount >= Renderer2DStorage::MaxIndices) {
			NextBatch();
		}

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec4 color = { 1.0f,1.0f,1.0f,1.0f };
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();
		if (s_Data.QuadIndexCount >= Renderer2DStorage::MaxIndices) {
			NextBatch();
		}
		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {

			if (*s_Data.TexturesSlots[i].get() == *texture.get()) {
				textureIndex = (float)i;
				break;
			}

		}

		if (textureIndex == 0.0f) {
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TexturesSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;

		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		for (uint32_t i = 0; i < quadVertexCount; i++) {
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		EG_PROFILE_FUNCTION();
		if (s_Data.QuadIndexCount >= Renderer2DStorage::MaxIndices) {
			NextBatch();
		}

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f} };

		const float texIndex = 0.0f;
		const float tilingFactor = 1.0f;

		for (uint32_t i = 0; i < quadVertexCount; i++) {

			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, int entityID)
	{
		EG_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec4 color = { 1.0f,1.0f,1.0f,1.0f };
		constexpr glm::vec2 textureCoords[] = { {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f} };

		if (s_Data.QuadIndexCount >= Renderer2DStorage::MaxIndices) {
			NextBatch();
		}
		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {

			if (*s_Data.TexturesSlots[i].get() == *texture.get()) {
				textureIndex = (float)i;
				break;
			}

		}

		if (textureIndex == 0.0f) {
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TexturesSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;

		}

		for (uint32_t i = 0; i < quadVertexCount; i++) {
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x,position.y, 0.0f }, size, rotation, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor)
	{
		DrawRotatedQuad({ position.x,position.y, 0.0f }, size, rotation, texture, tilingFactor);

	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor) {
		DrawRotatedQuad({ position.x,position.y, 0.0f }, size, rotation, subtexture, tilingFactor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		EG_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f} };

		if (s_Data.QuadIndexCount >= Renderer2DStorage::MaxIndices) {
			NextBatch();
		}
		const float texIndex = 0.0f;
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f,0.0f,1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		for (uint32_t i = 0; i < quadVertexCount; i++) {

			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;

	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor)
	{
		EG_PROFILE_FUNCTION();
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec4 color = { 1.0f,1.0f,1.0f,1.0f };
		constexpr glm::vec2 textureCoords[] = { {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f} };

		if (s_Data.QuadIndexCount >= Renderer2DStorage::MaxIndices) {
			NextBatch();
		}
		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {

			if (*s_Data.TexturesSlots[i].get() == *texture.get()) {
				textureIndex = (float)i;
				break;
			}

		}

		if (textureIndex == 0.0f) {
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TexturesSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;

		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f,0.0f,1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		for (uint32_t i = 0; i < quadVertexCount; i++) {
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}
		
		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor) {
		EG_PROFILE_FUNCTION();
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec4 color = { 1.0f,1.0f,1.0f,1.0f };
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		Ref<Texture2D> texture = subtexture->GetTexture();

		if (s_Data.QuadIndexCount >= Renderer2DStorage::MaxIndices) {
			NextBatch();
		}
		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {

			if (*s_Data.TexturesSlots[i].get() == *texture.get()) {
				textureIndex = (float)i;
				break;
			}

		}

		if (textureIndex == 0.0f) {
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TexturesSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;

		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f,0.0f,1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		for (uint32_t i = 0; i < quadVertexCount; i++) {
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.CircleIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}
	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& sprite, int entityID)
	{
		if (sprite.Texture)
			DrawQuad(transform, sprite.Texture, sprite.TilingFactor, entityID);
		else
			DrawQuad(transform, sprite.Color, entityID);
	}
	void Renderer2D::DrawCircle(const glm::mat4& transform, glm::vec4& color, float thickness, float fade, int entityID) {
		EG_PROFILE_FUNCTION();
		if (s_Data.QuadIndexCount >= Renderer2DStorage::MaxIndices) {
			NextBatch();
		}

		constexpr size_t quadVertexCount = 4;

		for (uint32_t i = 0; i < quadVertexCount; i++) {

			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = color;
			s_Data.CircleVertexBufferPtr->Thickness = thickness;
			s_Data.CircleVertexBufferPtr->Fade = fade;
			s_Data.CircleVertexBufferPtr->EntityID = entityID;
			s_Data.CircleVertexBufferPtr++;
		}

		s_Data.CircleIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	Renderer2D::Statistics Renderer2D::GetStats() {
		return s_Data.Stats;
	}
	void Renderer2D::ResetStats() {
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}
}
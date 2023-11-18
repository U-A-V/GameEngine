#include "hzpch.h"
#include "Renderer3D.h"

#include "Engine/Renderer/UniformBuffer.h"
#include "Engine/Renderer/3DPrimitives/Sphere.h"
#include "RenderCommand.h"
#include "VertexArray.h"
#include "Shader.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine {
	struct Material {
		glm::vec4 Albedo{ 1.0f };
		float Roughness = 1.0f;
		float Metallic = 0.0f;
		float padding = 0.0f;
		float padding1 = 0.0f;
	};

	struct CubeVertex {
		glm::vec3 Position;
		glm::vec4 Color;

		//editor only
		int EntityID;
	};

	struct SphereVertex {
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		float TexIndex;

		//editor only
		int EntityID;
	};

	struct SphereComponent {
		Ref<VertexArray> SphereVertexArray;
		uint32_t indexCount;
	};

	struct PointLight {
		glm::vec4 Position;

		float constant;
		float linear;
		float quadratic;
		float padding;

		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	struct R3Storage {

		static const uint32_t MaxTexturesSlots = 32;
		//materials
		Material Mat[100];
		uint32_t MaterialCount = 1;
		Ref<UniformBuffer> MaterialUniformBuffer;
		//cubes
		static const uint32_t MaxCubes = 1000;
		static const uint32_t MaxVertices = MaxCubes * 8;
		static const uint32_t MaxIndices = MaxCubes * 36;
		Ref<VertexArray> CubeVertexArray;
		Ref<VertexBuffer> CubeVertexBuffer;
		Ref<Shader> CubeShader;
		//Ref<Texture2D> WhiteTexture;

		uint32_t CubeIndexCount = 0;
		CubeVertex* CubeVertexBufferBase = nullptr;
		CubeVertex* CubeVertexBufferPtr = nullptr;

		glm::vec4 CubeVertexPositions[8];

		//sphere
		std::unordered_map<int, SphereComponent> SphereArray;
		static const uint32_t sectorCount = 32;
		static const uint32_t stackCount = 16;

		static const uint32_t MaxSphere = 1000;
		static const uint32_t MaxSphereVertices = 1000 * (sectorCount+1) * (stackCount+1);
		uint32_t MaxSphereIndices ;


		std::vector<glm::vec4> VertexPositions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> TexCoords;
		std::vector<uint32_t> SphereIndices;
		uint32_t indexCount = 0;

		Ref<VertexArray> SphereVertexArray;
		Ref<VertexBuffer> SphereVertexBuffer;
		Ref<Shader> SphereShader;

		uint32_t SphereIndexCount = 0;
		SphereVertex* SphereVertexBufferBase;
		SphereVertex* SphereVertexBufferPtr;

		Renderer3D::Statistics Stats;

		//point Light
		PointLight PointLightBuffer[100];
		Ref<UniformBuffer> PointLightUniformBuffer;

		uint32_t PointLightCount = 0;
		Ref<UniformBuffer> PointLightCountUniformBuffer;
		struct CameraData {
			glm::mat4 ViewProjection;
			glm::vec4 CameraPosition;
		};
		
		//skybox
		std::vector<std::string> faces = { "","","","","","" };
		bool RenderSkyBox;
		Ref<Shader> SkyBoxShader;
		Ref<VertexArray> SkyboxVertexArray;
		Ref<VertexBuffer> SkyboxVertexBuffer;
		Ref<Shader> HDRItoCubeMap;
		Ref<CubeTexture> CubeMap;

		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUnifromBuffer;
	};
	static R3Storage s_R3Data;
		void Renderer3D::Init() {
			EG_PROFILE_FUNCTION();

			//material 

			s_R3Data.Mat[0].Albedo = glm::vec4(1.0f, 1.0f,1.0f,1.0f);
			s_R3Data.Mat[0].Roughness = 0.1f;
			s_R3Data.Mat[0].Metallic = 0.7f;

			s_R3Data.MaterialUniformBuffer = UniformBuffer::Create(sizeof(Material) * 100, 3);


			//cube
			s_R3Data.CubeVertexArray = VertexArray::Create();
			s_R3Data.CubeVertexBuffer = VertexBuffer::Create(s_R3Data.MaxVertices*sizeof(CubeVertex));
			s_R3Data.CubeVertexBuffer->SetLayout({
				{ShaderDataType::Float3,		"a_Position"},
				{ShaderDataType::Float4,		"a_Color"},
				{ShaderDataType::Int,			"a_EntityID"},
			});
			s_R3Data.CubeVertexArray->AddVertexBuffer(s_R3Data.CubeVertexBuffer);
			s_R3Data.CubeVertexBufferBase = new CubeVertex[s_R3Data.MaxVertices];

			uint32_t* cubeIndices = new uint32_t[s_R3Data.MaxIndices];

			uint32_t offset = 0;

			for (uint32_t i = 0; i < s_R3Data.MaxIndices; i += 36) {
				cubeIndices[i + 0] = offset + 0;
				cubeIndices[i + 1] = offset + 1;
				cubeIndices[i + 2] = offset + 2;

				cubeIndices[i + 3] = offset + 2;
				cubeIndices[i + 4] = offset + 3;
				cubeIndices[i + 5] = offset + 0;

				cubeIndices[i + 6] = offset + 0;
				cubeIndices[i + 7] = offset + 3;
				cubeIndices[i + 8] = offset + 4;

				cubeIndices[i + 9] = offset + 4;
				cubeIndices[i + 10] = offset + 5;
				cubeIndices[i + 11] = offset + 0;

				cubeIndices[i + 12] = offset + 0;
				cubeIndices[i + 13] = offset + 5;
				cubeIndices[i + 14] = offset + 6;

				cubeIndices[i + 15] = offset + 6;
				cubeIndices[i + 16] = offset + 1;
				cubeIndices[i + 17] = offset + 0;

				cubeIndices[i + 18] = offset + 7;
				cubeIndices[i + 19] = offset + 4;
				cubeIndices[i + 20] = offset + 3;

				cubeIndices[i + 21] = offset + 3;
				cubeIndices[i + 22] = offset + 2;
				cubeIndices[i + 23] = offset + 7;

				cubeIndices[i + 24] = offset + 7;
				cubeIndices[i + 25] = offset + 2;
				cubeIndices[i + 26] = offset + 1;

				cubeIndices[i + 27] = offset + 1;
				cubeIndices[i + 28] = offset + 6;
				cubeIndices[i + 29] = offset + 7;

				cubeIndices[i + 30] = offset + 7;
				cubeIndices[i + 31] = offset + 6;
				cubeIndices[i + 32] = offset + 5;

				cubeIndices[i + 33] = offset + 5;
				cubeIndices[i + 34] = offset + 4;
				cubeIndices[i + 35] = offset + 7;


				offset += 8;
			}

			Ref<IndexBuffer> cubeIB = IndexBuffer::Create(cubeIndices, s_R3Data.MaxIndices);
			s_R3Data.CubeVertexArray->SetIndexBuffer(cubeIB);

			delete[] cubeIndices;

			//SkyBox

			s_R3Data.HDRItoCubeMap = Shader::Create("assets/shaders/HDRI_to_CubeMap.glsl");
			Ref<Texture2D> HDRTexture = Texture2D::Create("assests/textures/birchwood_4k.hdr");
			s_R3Data.CubeMap = CubeTexture::Create(1024);
			HDRTexture->ComputeCubeMap(s_R3Data.CubeMap, s_R3Data.HDRItoCubeMap);

			s_R3Data.SkyboxVertexArray = VertexArray::Create();
			s_R3Data.SkyboxVertexBuffer = VertexBuffer::Create(sizeof(glm::vec4)*8);
			s_R3Data.SkyboxVertexBuffer->SetLayout({
				{ShaderDataType::Float4,		"a_Position"},
				});
			s_R3Data.SkyboxVertexArray->AddVertexBuffer(s_R3Data.SkyboxVertexBuffer);
			s_R3Data.SkyboxVertexArray->SetIndexBuffer(cubeIB);
			


			//Sphere 
			s_R3Data.SphereVertexArray = VertexArray::Create();
			s_R3Data.SphereVertexBuffer = VertexBuffer::Create(s_R3Data.MaxSphereVertices * sizeof(SphereVertex));
			s_R3Data.SphereVertexBuffer->SetLayout({
				{ShaderDataType::Float3,		"a_Position"},
				{ShaderDataType::Float4,		"a_Color"},
				{ShaderDataType::Float3,		"a_Normal"},
				{ShaderDataType::Float2,		"a_TexCoord"},
				{ShaderDataType::Float,			"a_TexIndex"},
				{ShaderDataType::Int,			"a_EntityID"},
				});
			s_R3Data.SphereVertexArray->AddVertexBuffer(s_R3Data.SphereVertexBuffer);
			s_R3Data.SphereVertexBufferBase = new SphereVertex[s_R3Data.MaxSphereVertices];



			Sphere::Sphere( 0.5f, s_R3Data.sectorCount, s_R3Data.stackCount, s_R3Data.indexCount,
				s_R3Data.VertexPositions, s_R3Data.Normals, s_R3Data.TexCoords, s_R3Data.SphereIndices);
			s_R3Data.MaxSphereIndices = s_R3Data.MaxSphere * s_R3Data.SphereIndices.size();

			s_R3Data.Stats.SphereIndexCount = s_R3Data.SphereIndices.size();
			s_R3Data.Stats.SphereVertexCount = s_R3Data.VertexPositions.size();
			uint32_t SphereIndexOffset = 0;

			uint32_t* sphereIndices;
			sphereIndices = new uint32_t[s_R3Data.MaxSphereIndices];
			for (uint32_t i = 0; i < s_R3Data.MaxSphereIndices; i+= s_R3Data.SphereIndices.size()) {
				for (uint32_t k=0; k < s_R3Data.SphereIndices.size();k++) {
					sphereIndices[k + i] = SphereIndexOffset + s_R3Data.SphereIndices[k];
				}
				SphereIndexOffset += (s_R3Data.sectorCount + 1) * (s_R3Data.stackCount + 1);
			}

			Ref<IndexBuffer> sphereIB = IndexBuffer::Create(sphereIndices, s_R3Data.MaxSphereIndices);
			s_R3Data.SphereVertexArray->SetIndexBuffer(sphereIB);
			delete[] sphereIndices;

			int samplers[s_R3Data.MaxTexturesSlots];
			for (uint32_t i = 0; i < s_R3Data.MaxTexturesSlots; i++)
				samplers[i] = i;

			s_R3Data.CubeShader = Shader::Create("assets/shaders/Renderer3D_Cube.glsl");
			s_R3Data.SphereShader = Shader::Create("assets/shaders/Renderer3D_Sphere.glsl");
			s_R3Data.SkyBoxShader = Shader::Create("assets/shaders/Renderer3D_CubeMap.glsl");

			s_R3Data.CubeVertexPositions[0] = { -0.5f,	0.5f,	0.5f,	1.0f };
			s_R3Data.CubeVertexPositions[1] = { -0.5f,	-0.5f,	0.5f,	1.0f };
			s_R3Data.CubeVertexPositions[2] = {  0.5f,	-0.5f,	0.5f,	1.0f };
			s_R3Data.CubeVertexPositions[3] = {  0.5f,	0.5f,	0.5f,	1.0f };
			s_R3Data.CubeVertexPositions[4] = {  0.5f,	0.5f,	-0.5f,	1.0f };
			s_R3Data.CubeVertexPositions[5] = { -0.5f,	0.5f,	-0.5f,	1.0f };
			s_R3Data.CubeVertexPositions[6] = { -0.5f,	-0.5f,	-0.5f,	1.0f };
			s_R3Data.CubeVertexPositions[7] = {  0.5f,	-0.5f,	-0.5f,	1.0f };

			s_R3Data.SkyboxVertexBuffer->SetData(s_R3Data.CubeVertexPositions, 4*8*sizeof(float));

			s_R3Data.CameraUnifromBuffer = UniformBuffer::Create(sizeof(R3Storage::CameraData), 0);
			s_R3Data.PointLightUniformBuffer = UniformBuffer::Create(sizeof(PointLight)*100, 1);
			s_R3Data.PointLightCountUniformBuffer = UniformBuffer::Create(sizeof(uint32_t), 2);
		}
		void Renderer3D::Shutdown() {
			EG_PROFILE_FUNCTION();
		}

		void Renderer3D::BeginScene(const EditorCamera& camera) {
			EG_PROFILE_FUNCTION();
			s_R3Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
			s_R3Data.CameraBuffer.CameraPosition = glm::vec4(camera.GetPosition(), 1.0f);
			s_R3Data.CameraUnifromBuffer->SetData(&s_R3Data.CameraBuffer, sizeof(R3Storage::CameraData));

			StartBatch();
		}
		void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform) {
			EG_PROFILE_FUNCTION();
			s_R3Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
			s_R3Data.CameraBuffer.CameraPosition = glm::vec4(transform[0][3], transform[1][3], transform[2][3], transform[3][3]);
			s_R3Data.CameraUnifromBuffer->SetData(&s_R3Data.CameraBuffer, sizeof(R3Storage::CameraData));
			StartBatch();
		}

		void Renderer3D::EndScene() {
			EG_PROFILE_FUNCTION();
			s_R3Data.PointLightUniformBuffer->SetData(&s_R3Data.PointLightBuffer, sizeof(PointLight) * 100);
			s_R3Data.PointLightCountUniformBuffer->SetData(&s_R3Data.PointLightCount, sizeof(uint32_t));
			s_R3Data.MaterialUniformBuffer->SetData(&s_R3Data.Mat, sizeof(Material) * 100);

			Flush();
		}

		void Renderer3D::StartBatch() {
			s_R3Data.CubeIndexCount = 0;
			s_R3Data.CubeVertexBufferPtr = s_R3Data.CubeVertexBufferBase;

			s_R3Data.SphereIndexCount = 0;
			s_R3Data.SphereVertexBufferPtr = s_R3Data.SphereVertexBufferBase;

			//lights
			s_R3Data.PointLightCount = 0;

		}

		bool CanRender(std::vector<std::string>& faces) {
			bool res = true;
			for (size_t i = 0; i < 6; i++) {
				if (faces[i] == "") {
					res = false;
					break;
				}
			}
			return res;
		}

		void Renderer3D::Flush() {
			if (s_R3Data.CubeIndexCount) {
				uint32_t dataSize = (uint8_t*)s_R3Data.CubeVertexBufferPtr - (uint8_t*)s_R3Data.CubeVertexBufferBase;
				s_R3Data.CubeVertexBuffer->SetData(s_R3Data.CubeVertexBufferBase, dataSize);

				s_R3Data.CubeShader->Bind();
				RenderCommand::DrawIndexed(s_R3Data.CubeVertexArray, s_R3Data.CubeIndexCount);
				s_R3Data.Stats.DrawCalls++;
			}

			if (s_R3Data.SphereIndexCount) {

				uint32_t dataSize = (uint8_t*)s_R3Data.SphereVertexBufferPtr - (uint8_t*)s_R3Data.SphereVertexBufferBase;
				s_R3Data.SphereVertexBuffer->SetData(s_R3Data.SphereVertexBufferBase, dataSize);

				s_R3Data.SphereShader->Bind();
				s_R3Data.CubeMap->Bind();

				RenderCommand::DrawIndexed(s_R3Data.SphereVertexArray, s_R3Data.SphereIndexCount);
				s_R3Data.Stats.DrawCalls++;
			}

			{
				s_R3Data.SphereShader->Bind();
				s_R3Data.CubeMap->Bind();
				for (auto i : s_R3Data.SphereArray) {

					RenderCommand::DrawIndexed(i.second.SphereVertexArray, i.second.indexCount);
					s_R3Data.Stats.DrawCalls++;
					s_R3Data.Stats.SphereCount++;
				}
			}
			if (s_R3Data.RenderSkyBox) {

				glDepthFunc(GL_LEQUAL);
				s_R3Data.SkyBoxShader->Bind();
				s_R3Data.CubeMap->Bind();
				RenderCommand::DrawIndexed(s_R3Data.SkyboxVertexArray, 36);
				s_R3Data.Stats.DrawCalls++;
				glDepthFunc(GL_LESS);

			}
		}
		void Renderer3D::NextBatch() {
			Flush();
			StartBatch();
		}

		void Renderer3D::DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID) {
			EG_PROFILE_FUNCTION();
			if (s_R3Data.CubeIndexCount >= R3Storage::MaxIndices) {
				NextBatch();
			}

			for (uint32_t i = 0; i < 8; i++) {
				
				s_R3Data.CubeVertexBufferPtr->Position = transform * s_R3Data.CubeVertexPositions[i];
				s_R3Data.CubeVertexBufferPtr->Color = color;
				s_R3Data.CubeVertexBufferPtr->EntityID = entityID;
				s_R3Data.CubeVertexBufferPtr++;
			}

			s_R3Data.CubeIndexCount += 36;
			s_R3Data.Stats.CubeCount++;
		}

		void Renderer3D::DrawBatchSphere(const glm::mat4& transform, const SphereRendererComponent& sphere, int entityID) {
			EG_PROFILE_FUNCTION();
			uint32_t vertexCount = (s_R3Data.stackCount + 1) * (s_R3Data.sectorCount + 1);
			if (s_R3Data.SphereIndexCount >= s_R3Data.MaxSphereIndices) {
				NextBatch();
			}
			float textureIndex = 0.0f;

			for (uint32_t i = 0; i < vertexCount; i++) {
				s_R3Data.SphereVertexBufferPtr->Position = transform * s_R3Data.VertexPositions[i];
				s_R3Data.SphereVertexBufferPtr->Color = sphere.Color;
				s_R3Data.SphereVertexBufferPtr->Normal = s_R3Data.Normals[i];
				s_R3Data.SphereVertexBufferPtr->TexCoord = s_R3Data.TexCoords[i];
				s_R3Data.SphereVertexBufferPtr->TexIndex = textureIndex;
				s_R3Data.SphereVertexBufferPtr->EntityID = entityID;
				s_R3Data.SphereVertexBufferPtr++;
			}
			s_R3Data.Stats.SphereCount++;
			s_R3Data.SphereIndexCount += s_R3Data.SphereIndices.size();
		}
		void Renderer3D::DrawUniqueSphere(const glm::mat4& transform, const SphereRendererComponent& sphere, int entityID) {
			if (s_R3Data.SphereArray.find(entityID) == s_R3Data.SphereArray.end()) {
				UpdateSphere(transform, sphere, entityID);
			}
		}
		void Renderer3D::UpdateSphere(const glm::mat4& transform, const SphereRendererComponent& sphere, int entityID) {
			SphereComponent Sphereunit;
			std::vector<glm::vec4> VertexPositions;
			std::vector<glm::vec3> Normals;
			std::vector<glm::vec2> TexCoords;
			std::vector<uint32_t> SphereIndices;
			Sphere::Sphere(sphere.Radius, sphere.sectorCount, sphere.stackCount, Sphereunit.indexCount,
				VertexPositions, Normals, TexCoords, SphereIndices);
			Sphereunit.SphereVertexArray = VertexArray::Create();
			Ref<VertexBuffer> vbuffer = VertexBuffer::Create(VertexPositions.size() * sizeof(SphereVertex));
			vbuffer->SetLayout({
				{ShaderDataType::Float3,		"a_Position"},
				{ShaderDataType::Float4,		"a_Color"},
				{ShaderDataType::Float3,		"a_Normal"},
				{ShaderDataType::Float2,		"a_TexCoord"},
				{ShaderDataType::Float,			"a_TexIndex"},
				{ShaderDataType::Int,			"a_EntityID"},
				});
			Sphereunit.SphereVertexArray->AddVertexBuffer(vbuffer);
			uint32_t* indices = new uint32_t[SphereIndices.size()];
			for (uint32_t i = 0; i < SphereIndices.size(); i++) indices[i] = SphereIndices[i];
			Sphereunit.indexCount = SphereIndices.size();
			Ref<IndexBuffer> sphereIB = IndexBuffer::Create(indices, SphereIndices.size());
			Sphereunit.SphereVertexArray->SetIndexBuffer(sphereIB);

			float textureIndex = 0.0f;
			SphereVertex* SphereVertexBufferBase = new SphereVertex[VertexPositions.size()];
			SphereVertex* SphereVertexBufferPtr = SphereVertexBufferBase;
			for (uint32_t i = 0; i < VertexPositions.size(); i++) {
				SphereVertexBufferPtr->Position = transform * VertexPositions[i];
				SphereVertexBufferPtr->Color = sphere.Color;
				SphereVertexBufferPtr->Normal = Normals[i];
				SphereVertexBufferPtr->TexCoord = TexCoords[i];
				SphereVertexBufferPtr->TexIndex = textureIndex;
				SphereVertexBufferPtr->EntityID = entityID;
				SphereVertexBufferPtr++;
			}
			uint32_t dataSize = (uint8_t*)SphereVertexBufferPtr - (uint8_t*)SphereVertexBufferBase;
			vbuffer->SetData(SphereVertexBufferBase, dataSize);
			s_R3Data.SphereArray[entityID] = Sphereunit;
		}
		void Renderer3D::DrawSphere(const glm::mat4& transform, const SphereRendererComponent& sphere, int entityID) {
			if (sphere.batch)	DrawBatchSphere(transform, sphere, entityID);
			else DrawUniqueSphere(transform, sphere, entityID);

		}

		void Renderer3D::AddPointLight(const glm::mat4& transform, const PointLightComponent& light, int entityID) {
			s_R3Data.PointLightBuffer[s_R3Data.PointLightCount].ambient = glm::vec4(light.ambient, 1.0f);
			s_R3Data.PointLightBuffer[s_R3Data.PointLightCount].Position = transform*glm::vec4(glm::vec3(0.0f), 1.0f);

			s_R3Data.PointLightBuffer[s_R3Data.PointLightCount].diffuse = glm::vec4(light.diffuse, 1.0f);
			s_R3Data.PointLightBuffer[s_R3Data.PointLightCount].specular = glm::vec4(light.specular, 1.0f);
			s_R3Data.PointLightBuffer[s_R3Data.PointLightCount].constant = light.constant;
			s_R3Data.PointLightBuffer[s_R3Data.PointLightCount].linear = light.linear;
			s_R3Data.PointLightBuffer[s_R3Data.PointLightCount++].quadratic = light.quadratic;

			
			DrawCube(transform, glm::vec4(light.diffuse, 1.0f), entityID);

		}

		void Renderer3D::UpdateMaterial(const MaterialComponent& mat) {
			s_R3Data.Mat[0].Albedo = mat.Albedo;
			s_R3Data.Mat[0].Roughness = mat.Roughness;
			s_R3Data.Mat[0].Metallic = mat.Metallic;
		}

		void Renderer3D::CreateCubeMap(std::string& path) {
			Ref<Texture2D> HDRTex = Texture2D::Create(path);
			HDRTex->ComputeCubeMap(s_R3Data.CubeMap, s_R3Data.HDRItoCubeMap);
		}
		void Renderer3D::SetSkyBoxRenderer(bool canRender) {
			s_R3Data.RenderSkyBox = canRender;
		}
		Renderer3D::Statistics Renderer3D::GetStats() {
			return s_R3Data.Stats;
		}

		void Renderer3D::ResetStats() {
			memset(&s_R3Data.Stats, 0, sizeof(Statistics));
		}
}
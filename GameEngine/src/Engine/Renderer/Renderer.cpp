#include "hzpch.h"

#include "Renderer.h"
#include "Engine/Renderer/Renderer2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Engine {
	//Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
	void Renderer::Init() {
		RenderCommand::Init();
		Renderer2D::Init();

	}
	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::SetViewport(0, 0, width, height);
	}

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
	void Renderer::BeginScene(OrthographicCamera& camera){
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}
	void Renderer::EndScene(){

	}
	void Renderer::Submit(const Ref<Shader>& shader,const Ref<VertexArray>& vertexArray,const glm::mat4& transform){

		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4(m_SceneData->ViewProjectionMatrix, "u_ViewProjection");
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4(transform, "u_Transform");

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}
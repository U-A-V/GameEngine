#include "hzpch.h"

#include "Renderer.h"

namespace Engine {
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
	void Renderer::BeginScene(OrthographicCamera& camera){
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}
	void Renderer::EndScene(){

	}
	void Renderer::Submit(const std::shared_ptr<Shader>& shader,const std::shared_ptr<VertexArray>& vertexArray,const glm::mat4& transform){
		shader->Bind();
		shader->UploadUniformMat4(m_SceneData->ViewProjectionMatrix, "u_ViewProjection");
		shader->UploadUniformMat4(transform, "u_Transform");
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}
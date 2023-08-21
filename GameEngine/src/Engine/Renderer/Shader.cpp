#include "hzpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Engine {
	Ref<Shader> Shader::Create(const std::string& filepath) {

		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:	EG_CORE_ASSERT(false, "RendererAPI::None is not supported!!");
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLShader>(filepath);
		}

		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {

		switch (Renderer::GetAPI()) {

			case RendererAPI::API::None:	EG_CORE_ASSERT(false, "RendererAPI::None is not supported!!");
			case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


	void ShaderLibrary::Add(const Ref<Shader>& shader) {
		auto& name = shader->GetName();
		Add(name, shader);
	}
	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader) {
		EG_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}


	Ref<Shader> ShaderLibrary::Load(const std::string& filepath) {
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name) {
		EG_CORE_ASSERT(Exists(name), "Shader already exists!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const {
		return m_Shaders.find(name) != m_Shaders.end();
	}

}
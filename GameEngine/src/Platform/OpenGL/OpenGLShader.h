#pragma once

#include "Engine/Renderer/Shader.h"
#include <glm/glm.hpp>

//
typedef unsigned int GLenum;

namespace Engine {
	class OpenGLShader:public Shader {
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const std::string& GetName() const override { return m_Name; }

		void UploadUniformInt(int value, const std::string& name);

		void UploadUniformFloat(float value, const std::string& name);
		void UploadUniformFloat2(const glm::vec2& values, const std::string& name);
		void UploadUniformFloat3(const glm::vec3& values, const std::string& name);
		void UploadUniformFloat4(const glm::vec4& values, const std::string& name);

		void UploadUniformMat3(const glm::mat3& matrix, const std::string& name);
		void UploadUniformMat4(const glm::mat4& matrix, const std::string& name);
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& shaderSrc);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSrc);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};
}
#pragma once
#include <string>

#include "glm/glm.hpp"

namespace Engine {
	class Shader {
	public:
		Shader(const std::string vertexSrc, const std::string fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void UploadUniformMat4(const glm::mat4 matrix, const std::string name);
	private:
		uint32_t m_RendererID;

	};
}
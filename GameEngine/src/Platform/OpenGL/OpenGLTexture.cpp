#include "hzpch.h"

#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Engine {
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) 
		:m_Path(path){
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data=stbi_load(path.c_str(),&width,&height, &channels,0);
		EG_CORE_ASSERT(data, "Failed to load {1}",path);
		m_Width = width;
		m_Height = height;

		GLenum InternalFormat = 0, dataFormat=0;
		if (channels == 4) {
			InternalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3) {
			InternalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		EG_CORE_ASSERT(InternalFormat && dataFormat, "Formate not supported!!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, InternalFormat, m_Width, m_Height);
  
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	OpenGLTexture2D::~OpenGLTexture2D() {
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_RendererID);
	}
}
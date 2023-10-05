#include "hzpch.h"

#include "OpenGLTexture.h"

#include "stb_image.h"


namespace Engine {
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		:m_Width(width), m_Height(height){
		EG_PROFILE_FUNCTION();


		m_Internalformat = GL_RGBA8;
		m_Format = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_Internalformat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_Path(path){
		EG_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			EG_PROFILE_SCOPE("stb_i_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data=stbi_load(path.c_str(),&width,&height, &channels,0);
		}
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
		m_Internalformat = InternalFormat;
		m_Format = dataFormat;
		EG_CORE_ASSERT(InternalFormat && dataFormat, "Formate not supported!!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, InternalFormat, m_Width, m_Height);
  
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	OpenGLTexture2D::~OpenGLTexture2D() {
		EG_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		EG_PROFILE_FUNCTION();

		uint32_t bpc = m_Format == GL_RGBA ? 4 : 3;
		EG_CORE_ASSERT(size == m_Width * m_Height * bpc, "Data must be entire texture");
		glTextureSubImage2D(m_RendererID, 0,0,0,m_Width,m_Height,m_Format,GL_UNSIGNED_BYTE,data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const {
		EG_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}
}
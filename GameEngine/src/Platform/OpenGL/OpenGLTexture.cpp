#include "hzpch.h"

#include "OpenGLTexture.h"

#include "stb_image.h"
#include "Engine/Renderer/Shader.h"
#include "stb_image_write.h"
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
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, int size)
		:m_Path(path){
		EG_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);

		float* fdata = nullptr;
		stbi_uc* data = nullptr;
		if (size == 32) {
			fdata = stbi_loadf(path.c_str(), &width, &height, &channels,STBI_rgb_alpha);
			if (fdata == nullptr)	EG_CORE_WARN("could not load hrd");
		}
		else {
			EG_PROFILE_SCOPE("stb_i_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 4);
		}
		channels = 4;
		EG_CORE_ASSERT(data, "Failed to load {1}",path);
		m_Width = width;
		m_Height = height;

		GLenum InternalFormat = 0, dataFormat=0, pixelType = 0;
		if (channels == 4) {
			if (size == 8) {
				InternalFormat = GL_RGBA8;
				pixelType = GL_UNSIGNED_BYTE;
			}
			else if (size == 32) {
				InternalFormat = GL_RGBA32F;
				pixelType = GL_FLOAT;
				
			}
			dataFormat = GL_RGBA;
		}
		else if (channels == 3) {
			if (size == 8) {
				InternalFormat = GL_RGB8;
				pixelType = GL_UNSIGNED_BYTE;
			}
			else if (size == 32) {
				InternalFormat = GL_RGB32F;
				pixelType = GL_FLOAT;

			}
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

		if(size == 32)
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, pixelType, fdata);
		else
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, pixelType, data);


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

	void OpenGLTexture2D::ComputeCubeMap(Ref<CubeTexture>& target, Ref<Shader>& shader) {
		shader->Bind();

		glBindImageTexture(0, m_RendererID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
		glBindImageTexture(1, target->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);



		GLuint numGroupsX = target->GetSize();
		GLuint numGroupsY = target->GetSize();
		GLuint numGroupsZ = 6;
		glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		/*
		GLubyte* cubemapData = new GLubyte[4 * target->GetSize() * target->GetSize() * 6];

		for (int face = 0; face < 6; ++face) {
			glBindTexture(GL_TEXTURE_CUBE_MAP, target->GetRendererID());
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


			glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA, GL_UNSIGNED_BYTE, cubemapData + 4 * target->GetSize() * target->GetSize() * face);
		}


		if (stbi_write_png("assets/textures/cubemap.png", target->GetSize(), target->GetSize() * 6, 4, cubemapData, target->GetSize() * 4) == 0) {
			std::cerr << "Failed to write PNG file." << std::endl;
		}
		else {
			std::cout << "Cubemap saved as cubema";
		}
		delete[] cubemapData;
		*/
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const {
		EG_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}
}
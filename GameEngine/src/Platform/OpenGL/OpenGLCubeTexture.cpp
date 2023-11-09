#include "hzpch.h"
#include "OpenGLCubeTexture.h"
#include "stb_image.h"


namespace Engine {
	OpenGLCubeTexture::OpenGLCubeTexture(const std::vector<std::string>& faces) {
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(false);
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            }
            else
            {
                EG_CORE_WARN("Cubemap tex failed to load at path: {0}", faces[i]);
                stbi_image_free(data);
            }
        }
        EG_CORE_INFO("Loded CubeMap Tex Successfully ");
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

    OpenGLCubeTexture::~OpenGLCubeTexture() {
        EG_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLCubeTexture::SetData(const std::string& filePath, uint8_t index) {
        m_FilePath[index] = filePath;
    }

    bool OpenGLCubeTexture::CanRender() const {
        bool res = true;
        for (size_t i = 0; i < 6; i++) {
            if (m_FilePath[i] == "") {
                res = false;
                break;
            }
        }
        return res;
    }

    std::vector<std::string> OpenGLCubeTexture::GetFilePath() {
        return m_FilePath;
    }

    void OpenGLCubeTexture::Bind(uint32_t slot) const {
        EG_PROFILE_FUNCTION();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
    }
}
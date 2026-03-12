#include "OpenGLTexture.h"
#include "BSAEngine/Log/Log.h"

#include <glad/glad.h>
#include "stb_image.h"

namespace BSA {

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
        : m_Path(path), m_Width(0), m_Height(0)
    {
        // OpenGL resimerin 0,0 noktasini sol-alt olarak kabul eder, ozel araclar genelde sol-uste koyar
        stbi_set_flip_vertically_on_load(1);

        int width, height, channels;
        // Resim verisini RAM'e yükle
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (data) {
            m_Width = width;
            m_Height = height;

            GLenum internalFormat = 0, dataFormat = 0;
            if (channels == 4) {
                internalFormat = GL_RGBA8;
                dataFormat = GL_RGBA;
            } else if (channels == 3) {
                internalFormat = GL_RGB8;
                dataFormat = GL_RGB;
            }

            if (internalFormat == 0) {
                BSA_ENGINE_ERROR("Texture formati desteklenmiyor! (Kanal sayisi: {0})", channels);
            } else {
                // Texture objesini olustur
                glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
                glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

                // Min/Mag filtrelerini ayarla (Linear = Yumusak/Bulanik scale, Nearest = Piksel bazli scale)
                glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                // Texture eksen tekrarlarini belirle
                glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

                // RAM'deki veriyi VRAM'e (GPU bellegine) gonder
                glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
            }

            // RAM'den veri temizle
            stbi_image_free(data);
        } else {
            BSA_ENGINE_ERROR("Texture yuklenemedi! Dosya yolu: {0}", path);
        }
    }

    OpenGLTexture2D::~OpenGLTexture2D() {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const {
        // İlgili slota (orn: 0, 1, 2) texture objesini bagla
        glBindTextureUnit(slot, m_RendererID);
    }

} // namespace BSA

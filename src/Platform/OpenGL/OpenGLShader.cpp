#include "OpenGLShader.h"
#include "BSAEngine/Log/Log.h"
#include <glad/glad.h>
#include <vector>

namespace BSA {

    OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
        
        // Boş bir referans program objesi oluştur
        GLuint program = glCreateProgram();

        // -------------------------------------------------
        // 1. VERTEX SHADER DERLEME VE HATA KONTROLÜ
        // -------------------------------------------------
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const GLchar* source = (const GLchar*)vertexSrc.c_str();
        glShaderSource(vertexShader, 1, &source, 0);
        glCompileShader(vertexShader);

        GLint isCompiled = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(vertexShader);

            BSA_ENGINE_ERROR("Vertex Shader derleme hatasi:\n{0}", infoLog.data());
            return;
        }

        // -------------------------------------------------
        // 2. FRAGMENT SHADER DERLEME VE HATA KONTROLÜ
        // -------------------------------------------------
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        source = (const GLchar*)fragmentSrc.c_str();
        glShaderSource(fragmentShader, 1, &source, 0);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(fragmentShader);
            glDeleteShader(vertexShader);

            BSA_ENGINE_ERROR("Fragment Shader derleme hatasi:\n{0}", infoLog.data());
            return;
        }

        // -------------------------------------------------
        // 3. PROGRAM LINKLEME VE HATA KONTROLÜ
        // -------------------------------------------------
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            glDeleteProgram(program);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            BSA_ENGINE_ERROR("Shader Linkleme (Baglama) hatasi:\n{0}", infoLog.data());
            return;
        }

        // Bağlantı başarılıysa artık shader objelerine ihtiyacımız yok
        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        m_RendererID = program;
    }

    OpenGLShader::~OpenGLShader() {
        glDeleteProgram(m_RendererID);
    }

    void OpenGLShader::Bind() const {
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const {
        glUseProgram(0);
    }

    void OpenGLShader::UploadUniformMat4(const std::string& name, const BSA::Math::Matrix4& matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, matrix.ValuePtr());
    }

} // namespace BSA

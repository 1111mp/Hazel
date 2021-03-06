#include "OpenGLTexture.h"

#include "stb_image.h"

namespace Hazel
{

  OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
      : m_Width(width), m_Height(height)
  {
    HZ_PROFILE_FUNCTION();

    m_InternalFormat = GL_RGBA8;
    m_DataFormat = GL_RGBA;

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, width, height, 0, m_DataFormat, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    // glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

    // glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  OpenGLTexture2D::OpenGLTexture2D(const std::string &path)
      : m_Path(path)
  {
    HZ_PROFILE_FUNCTION();

    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc *data = nullptr;
    {
      HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
      data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    }
    HZ_CORE_ASSERT(data, "Failed to load image!");
    m_Width = width;
    m_Height = height;

    // GLenum internalFormat = 0, dataFormat = 0;
    if (channels == 4)
    {
      m_InternalFormat = GL_RGBA8;
      m_DataFormat = GL_RGBA;
    }
    else if (channels == 3)
    {
      m_InternalFormat = GL_RGB8;
      m_DataFormat = GL_RGB;
    }

    HZ_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Format not supported!");

#ifdef __APPLE__
    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, width, height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
#else
    glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

    glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
#endif

    stbi_image_free(data);
  }

  OpenGLTexture2D::~OpenGLTexture2D()
  {
    HZ_PROFILE_FUNCTION();

    glDeleteTextures(1, &m_RendererID);
  }

  void OpenGLTexture2D::SetData(void *data, uint32_t size)
  {
    HZ_PROFILE_FUNCTION();

    uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
    HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
#ifdef __APPLE__
    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
#else
    glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
#endif
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  void OpenGLTexture2D::Bind(uint32_t slot) const
  {
    HZ_PROFILE_FUNCTION();
#ifdef __APPLE__
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
#else
    glBindTextureUnit(slot, m_RendererID);
#endif
  }

}
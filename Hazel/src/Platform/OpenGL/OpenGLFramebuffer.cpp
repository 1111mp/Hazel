#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Hazel
{

  OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification &spec)
      : m_Specification(spec)
  {
    Invalidate();
  }

  OpenGLFramebuffer::~OpenGLFramebuffer()
  {
    glDeleteFramebuffers(1, &m_RendererID);
    glDeleteTextures(1, &m_ColorAttachment);
    glDeleteTextures(1, &m_DepthAttachment);
  }

  void OpenGLFramebuffer::Invalidate()
  {
    if (m_RendererID)
    {
      glDeleteFramebuffers(1, &m_RendererID);
      glDeleteTextures(1, &m_ColorAttachment);
      glDeleteTextures(1, &m_DepthAttachment);
    }

#ifdef __APPLE__
    glGenFramebuffers(1, &m_RendererID);
#else
    glCreateFramebuffers(1, &m_RendererID);
#endif
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    m_ColorAttachment = GenerateAttachmentTexture(false, false);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

    m_DepthAttachment = GenerateAttachmentTexture(true, false);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

    HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void OpenGLFramebuffer::Bind()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glViewport(0, 0, m_Specification.Width, m_Specification.Height);
  }

  void OpenGLFramebuffer::UnBind()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
  {
    m_Specification.Width = width;
    m_Specification.Height = height;

    Invalidate();
  }

  // Generates a texture that is suited for attachments to a framebuffer
  uint32_t OpenGLFramebuffer::GenerateAttachmentTexture(bool depth, bool stencil)
  {
    // What enum to use?
    GLenum attachment_type;
    if (!depth && !stencil)
      attachment_type = GL_RGB;
    else if (depth && !stencil)
      attachment_type = GL_DEPTH_COMPONENT;
    else if (!depth && stencil)
      attachment_type = GL_STENCIL_INDEX;

    // Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    if (!depth && !stencil)
      glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, m_Specification.Width, m_Specification.Height, 0, attachment_type, GL_UNSIGNED_BYTE, nullptr);
    else // Using both a stencil and depth test, needs special format arguments
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
  }

}
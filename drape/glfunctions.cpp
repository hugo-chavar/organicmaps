#include "glfunctions.hpp"
#include "glIncludes.hpp"
#include "../base/assert.hpp"

namespace
{
  inline GLboolean convert(bool v)
  {
    return (v == true) ? GL_TRUE : GL_FALSE;
  }

  /// VAO
  void (*glGenVertexArraysFn)(GLsizei n, GLuint * ids)                                                    = NULL;
  void (*glBindVertexArrayFn)(GLuint id)                                                                  = NULL;
  void (*glDeleteVertexArrayFn)(GLsizei n, const GLuint * ids)                                            = NULL;

  /// VBO
  void (*glGenBuffersFn)(GLsizei n, GLuint *buffers)                                                      = NULL;
  void (*glBindBufferFn)(GLenum target, GLuint buffer)                                                    = NULL;
  void (*glDeleteBuffersFn)(GLsizei n, const GLuint *buffers)                                             = NULL;
  void (*glBufferDataFn)(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage)               = NULL;
  void (*glBufferSubDataFn)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data)         = NULL;

  /// Shaders
  GLuint (*glCreateShaderFn)(GLenum type)                                                                 = NULL;
  void (*glShaderSourceFn)(GLuint shaderID, GLsizei count, const GLchar ** string, const GLint * length)  = NULL;
  void (*glCompileShaderFn)(GLuint shaderID)                                                              = NULL;
  void (*glDeleteShaderFn)(GLuint shaderID)                                                               = NULL;
  void (*glGetShaderivFn)(GLuint shaderID, GLenum name, GLint * p)                                        = NULL;
  void (*glGetShaderInfoLogFn)(GLuint shaderID, GLsizei maxLength, GLsizei * length, GLchar * infoLog)    = NULL;

  GLuint (*glCreateProgramFn)()                                                                           = NULL;
  void (*glAttachShaderFn)(GLuint programID, GLuint shaderID)                                             = NULL;
  void (*glDetachShaderFn)(GLuint programID, GLuint shaderID)                                             = NULL;
  void (*glLinkProgramFn)(GLuint programID)                                                               = NULL;
  void (*glDeleteProgramFn)(GLuint programID)                                                             = NULL;
  void (*glGetProgramivFn)(GLuint programID, GLenum name, GLint * p)                                      = NULL;
  void (*glGetProgramInfoLogFn)(GLuint programID, GLsizei maxLength, GLsizei * length, GLchar * infoLog)  = NULL;

  void (*glUseProgramFn)(GLuint programID)                                                                = NULL;
  GLint (*glGetAttribLocationFn)(GLuint program, const GLchar * name)                                     = NULL;
  void (*glBindAttribLocationFn)(GLuint program, GLuint index, const GLchar * name)                       = NULL;

  void (*glEnableVertexAttributeFn)(GLuint location)                                                      = NULL;
  void (*glVertexAttributePointerFn)(GLuint index,
                                     GLint count,
                                     GLenum type,
                                     GLboolean normalize,
                                     GLsizei stride,
                                     const GLvoid * p)                                                    = NULL;

  GLint (*glGetUniformLocationFn)(GLuint programID, const GLchar * name)                                  = NULL;
  void (*glUniform1iFn)(GLint location, GLint value)                                                      = NULL;
  void (*glUniform2iFn)(GLint location, GLint v1, GLint v2)                                               = NULL;
  void (*glUniform3iFn)(GLint location, GLint v1, GLint v2, GLint v3)                                     = NULL;
  void (*glUniform4iFn)(GLint location, GLint v1, GLint v2, GLint v3, GLint v4)                           = NULL;

  void (*glUniform1fFn)(GLint location, GLfloat value)                                                    = NULL;
  void (*glUniform2fFn)(GLint location, GLfloat v1, GLfloat v2)                                           = NULL;
  void (*glUniform3fFn)(GLint location, GLfloat v1, GLfloat v2, GLfloat v3)                               = NULL;
  void (*glUniform4fFn)(GLint location, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4)                   = NULL;

  void (*glUniformMatrix4fvFn)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;

  const int GLCompileStatus = GL_COMPILE_STATUS;
  const int GLLinkStatus = GL_LINK_STATUS;
}

namespace GLConst
{
  const glConst GLArrayBuffer         = GL_ARRAY_BUFFER;
  const glConst GLElementArrayBuffer  = GL_ELEMENT_ARRAY_BUFFER;

  const glConst GLStaticDraw          = GL_STATIC_DRAW;
  const glConst GLStreamDraw          = GL_STREAM_DRAW;
  const glConst GLDynamicDraw         = GL_DYNAMIC_DRAW;

  const glConst GLVertexShader        = GL_VERTEX_SHADER;
  const glConst GLFragmentShader      = GL_FRAGMENT_SHADER;
  const glConst GLCurrentProgram      = GL_CURRENT_PROGRAM;

  const glConst GL8BitOnChannel       = GL_UNSIGNED_BYTE;
  const glConst GL4BitOnChannel       = GL_UNSIGNED_SHORT_4_4_4_4;

  const glConst GLByteType            = GL_BYTE;
  const glConst GLUnsignedByteType    = GL_UNSIGNED_BYTE;
  const glConst GLShortType           = GL_SHORT;
  const glConst GLUnsignedShortType   = GL_UNSIGNED_SHORT;
  const glConst GLIntType             = GL_INT;
  const glConst GLUnsignedIntType     = GL_UNSIGNED_INT;
  const glConst GLFloatType           = GL_FLOAT;
  const glConst GLDoubleType          = GL_DOUBLE;
}

void GLFunctions::Init()
{
  /// VAO
#if defined(OGL_MAC)
  glGenVertexArraysFn = &glGenVertexArraysAPPLE;
  glBindVertexArrayFn = &glBindVertexArrayAPPLE;
  glDeleteVertexArrayFn = &glDeleteVertexArraysAPPLE;
#elif defined(OGL_IOS) || defined(OGL_ANDROID)
  glGenVertexArraysFn = &glGenVertexArraysOES;
  glBindVertexArrayFn = &glBindVertexArrayOES;
  glDeleteVertexArrayFn = &glDeleteVertexArraysOES;
#endif

  /// VBO
  glGenBuffersFn = &::glGenBuffers;
  glBindBufferFn = &::glBindBuffer;
  glDeleteBuffersFn = &::glDeleteBuffers;
  glBufferDataFn = &::glBufferData;
  glBufferSubDataFn = &::glBufferSubData;

  /// Shaders
  glCreateShaderFn = &::glCreateShader;
  typedef void (*glShaderSource_Type)(GLuint shaderID, GLsizei count, const GLchar ** string, const GLint * length);
  glShaderSourceFn = reinterpret_cast<glShaderSource_Type>(&::glShaderSource);
  glCompileShaderFn = &::glCompileShader;
  glDeleteShaderFn = &::glDeleteShader;
  glGetShaderivFn = &::glGetShaderiv;
  glGetShaderInfoLogFn = &::glGetShaderInfoLog;

  glCreateProgramFn = &::glCreateProgram;
  glAttachShaderFn = &::glAttachShader;
  glDetachShaderFn = &::glDetachShader;
  glLinkProgramFn = &::glLinkProgram;
  glDeleteProgramFn = &::glDeleteProgram;
  glGetProgramivFn = &::glGetProgramiv;
  glGetProgramInfoLogFn = &::glGetProgramInfoLog;

  glUseProgramFn = &::glUseProgram;
  glGetAttribLocationFn = &::glGetAttribLocation;
  glBindAttribLocationFn = &::glBindAttribLocation;

  glEnableVertexAttributeFn = &::glEnableVertexAttribArray;
  glVertexAttributePointerFn = &::glVertexAttribPointer;

  glGetUniformLocationFn = &::glGetUniformLocation;
  glUniform1iFn = &::glUniform1i;
  glUniform2iFn = &::glUniform2i;
  glUniform3iFn = &::glUniform3i;
  glUniform4iFn = &::glUniform4i;

  glUniform1fFn = &::glUniform1f;
  glUniform2fFn = &::glUniform2f;
  glUniform3fFn = &::glUniform3f;
  glUniform4fFn = &::glUniform4f;

  glUniformMatrix4fvFn = &glUniformMatrix4fv;
}

bool GLFunctions::glHasExtension(const string & name)
{
  const char * extensions = reinterpret_cast<char const * >(glGetString(GL_EXTENSIONS));
  const char * extName = name.c_str();
  const char * ptr = NULL;
  while ((ptr = strstr(extensions, extName)) != NULL)
  {
    const char * end = ptr + strlen(extName);
    if (isspace(*end) || *end == '\0')
        return true;

    extensions = ptr;
  }

  return false;
}

int GLFunctions::glGenVertexArray()
{
  ASSERT(glGenVertexArraysFn != NULL, ());
  GLuint result = 0;
  GLCHECK(glGenVertexArraysFn(1, &result));
  return result;
}

void GLFunctions::glBindVertexArray(uint32_t vao)
{
  ASSERT(glBindVertexArrayFn != NULL, ());
  GLCHECK(glBindVertexArrayFn(vao));
}

void GLFunctions::glDeleteVertexArray(uint32_t vao)
{
  ASSERT(glDeleteVertexArrayFn != NULL, ());
  GLCHECK(glDeleteVertexArrayFn(1, &vao));
}

uint32_t GLFunctions::glGenBuffer()
{
  ASSERT(glGenBuffersFn != NULL, ());
  GLuint result = (GLuint)-1;
  GLCHECK(glGenBuffersFn(1, &result));
  return result;
}

void GLFunctions::glBindBuffer(uint32_t vbo, uint32_t target)
{
  ASSERT(glBindBufferFn != NULL, ());
  GLCHECK(glBindBufferFn(target, vbo));
}

void GLFunctions::glDeleteBuffer(uint32_t vbo)
{
  ASSERT(glDeleteBuffersFn != NULL, ());
  GLCHECK(glDeleteBuffersFn(1, &vbo));
}

void GLFunctions::glBufferData(glConst target, uint32_t size, const void * data, glConst usage)
{
  ASSERT(glBufferDataFn != NULL, ());
  GLCHECK(glBufferDataFn(target, size, data, usage));
}

void GLFunctions::glBufferSubData(glConst target, uint32_t size, const void * data, uint32_t offset)
{
  ASSERT(glBufferSubDataFn != NULL, ());
  GLCHECK(glBufferSubDataFn(target, offset, size, data));
}

uint32_t GLFunctions::glCreateShader(glConst type)
{
  ASSERT(glCreateShaderFn != NULL, ());
  GLuint result = glCreateShaderFn(type);
  GLCHECKCALL();
  return result;
}

void GLFunctions::glShaderSource(uint32_t shaderID, const string & src)
{
  ASSERT(glShaderSourceFn != NULL, ());
  const GLchar * source = src.c_str();
  GLint length = src.size();
  GLCHECK(glShaderSourceFn(shaderID, 1, &source, &length));
}

bool GLFunctions::glCompileShader(uint32_t shaderID, string &errorLog)
{
  ASSERT(glCompileShaderFn != NULL, ());
  ASSERT(glGetShaderivFn != NULL, ());
  ASSERT(glGetShaderInfoLogFn != NULL, ());
  GLCHECK(glCompileShaderFn(shaderID));

  GLint result = GL_FALSE;
  GLCHECK(glGetShaderivFn(shaderID, GLCompileStatus, &result));
  if (result == GL_TRUE)
    return true;

  GLchar buf[1024];
  GLint length = 0;
  GLCHECK(glGetShaderInfoLogFn(shaderID, 1024, &length, buf));
  errorLog = string(buf, length);
  return false;
}

void GLFunctions::glDeleteShader(uint32_t shaderID)
{
  ASSERT(glDeleteShaderFn != NULL, ());
  GLCHECK(glDeleteBuffersFn(1, &shaderID));
}

uint32_t GLFunctions::glCreateProgram()
{
  ASSERT(glCreateProgramFn != NULL, ());
  GLuint result = glCreateProgramFn();
  GLCHECKCALL();
  return result;
}

void GLFunctions::glAttachShader(uint32_t programID, uint32_t shaderID)
{
  ASSERT(glAttachShaderFn != NULL, ());
  GLCHECK(glAttachShaderFn(programID, shaderID));
}

void GLFunctions::glDetachShader(uint32_t programID, uint32_t shaderID)
{
  ASSERT(glDetachShaderFn != NULL, ());
  GLCHECK(glDetachShaderFn(programID, shaderID));
}

bool GLFunctions::glLinkProgram(uint32_t programID, string & errorLog)
{
  ASSERT(glLinkProgramFn != NULL, ());
  ASSERT(glGetProgramivFn != NULL, ());
  ASSERT(glGetProgramInfoLogFn != NULL, ());
  GLCHECK(glLinkProgramFn(programID));

  GLint result = GL_FALSE;
  GLCHECK(glGetProgramivFn(programID, GLLinkStatus, &result));

  if (result == GL_TRUE)
    return true;

  GLchar buf[1024];
  GLint length = 0;
  GLCHECK(glGetProgramInfoLogFn(programID, 1024, &length, buf));
  errorLog = string(buf, length);
  return false;
}

void GLFunctions::glDeleteProgram(uint32_t programID)
{
  ASSERT(glDeleteProgramFn != NULL, ());
  GLCHECK(glDeleteProgramFn(programID));
}

void GLFunctions::glUseProgram(uint32_t programID)
{
  ASSERT(glUseProgramFn != NULL, ());
  GLCHECK(glUseProgramFn(programID));
}

int8_t GLFunctions::glGetAttribLocation(uint32_t programID, const string & name)
{
  ASSERT(glGetAttribLocationFn != NULL, ());
  int result = glGetAttribLocationFn(programID, name.c_str());
  GLCHECKCALL();
  ASSERT(result != -1, ());
  return result;
}

void GLFunctions::glBindAttribLocation(uint32_t programID, uint8_t index, const string & name)
{
  ASSERT(glBindAttribLocationFn != NULL, ());
  GLCHECK(glBindAttribLocationFn(programID, index, name.c_str()));
}

void GLFunctions::glEnableVertexAttribute(int attributeLocation)
{
  ASSERT(glEnableVertexAttributeFn != NULL, ());
  GLCHECK(glEnableVertexAttributeFn(attributeLocation));
}

void GLFunctions::glVertexAttributePointer(int attrLocation,
                              uint32_t count,
                              glConst type,
                              bool needNormalize,
                              uint32_t stride,
                              uint32_t offset)
{
  ASSERT(glVertexAttributePointerFn != NULL, ());
  GLCHECK(glVertexAttributePointerFn(attrLocation,
                                     count,
                                     type,
                                     convert(needNormalize),
                                     stride,
                                     reinterpret_cast<void *>(offset)));
}

int8_t GLFunctions::glGetUniformLocation(uint32_t programID, const string & name)
{
  ASSERT(glGetUniformLocationFn != NULL, ());
  int result = glGetUniformLocationFn(programID, name.c_str());
  GLCHECKCALL();
  ASSERT(result != -1, ());
  return result;
}

void GLFunctions::glUniformValue(int8_t location, int32_t v)
{
  ASSERT(glUniform1iFn != NULL, ());
  ASSERT(location != -1, ());
  GLCHECK(glUniform1iFn(location, v));
}

void GLFunctions::glUniformValue(int8_t location, int32_t v1, int32_t v2)
{
  ASSERT(glUniform2iFn != NULL, ());
  ASSERT(location != -1, ());
  GLCHECK(glUniform2iFn(location, v1, v2));
}

void GLFunctions::glUniformValue(int8_t location, int32_t v1, int32_t v2, int32_t v3)
{
  ASSERT(glUniform3iFn != NULL, ());
  ASSERT(location != -1, ());
  GLCHECK(glUniform3iFn(location, v1, v2, v3));
}

void GLFunctions::glUniformValue(int8_t location, int32_t v1, int32_t v2, int32_t v3, int32_t v4)
{
  ASSERT(glUniform4iFn != NULL, ());
  ASSERT(location != -1, ());
  GLCHECK(glUniform4iFn(location, v1, v2, v3, v4));
}

void GLFunctions::glUniformValue(int8_t location, float v)
{
  ASSERT(glUniform1fFn != NULL, ());
  ASSERT(location != -1, ());
  GLCHECK(glUniform1fFn(location, v));
}

void GLFunctions::glUniformValue(int8_t location, float v1, float v2)
{
  ASSERT(glUniform2fFn != NULL, ());
  ASSERT(location != -1, ());
  GLCHECK(glUniform2fFn(location, v1, v2));
}

void GLFunctions::glUniformValue(int8_t location, float v1, float v2, float v3)
{
  ASSERT(glUniform3fFn != NULL, ());
  ASSERT(location != -1, ());
  GLCHECK(glUniform3fFn(location, v1, v2, v3));
}

void GLFunctions::glUniformValue(int8_t location, float v1, float v2, float v3, float v4)
{
  ASSERT(glUniform4fFn != NULL, ());
  ASSERT(location != -1, ());
  GLCHECK(glUniform4fFn(location, v1, v2, v3, v4));
}

void GLFunctions::glUniformMatrix4x4Value(int8_t location, float * values)
{
  ASSERT(glUniformMatrix4fvFn != NULL, ());
  ASSERT(location != -1, ());
  GLCHECK(glUniformMatrix4fvFn(location, 1, GL_FALSE, values));
}

uint32_t GLFunctions::glGetCurrentProgram()
{
  GLint programIndex = 0;
  GLCHECK(glGetIntegerv(GL_CURRENT_PROGRAM, &programIndex));
  return programIndex;
}

void GLFunctions::glActiveTexture(uint32_t samplerBlock)
{
  GLCHECK(::glActiveTexture(GL_TEXTURE0 + samplerBlock));
}

uint32_t GLFunctions::glGenTexture()
{
  GLuint result = 0;
  GLCHECK(::glGenTextures(1, &result));
  return result;
}

void GLFunctions::glBindTexture(uint32_t textureID)
{
  GLCHECK(::glBindTexture(GL_TEXTURE_2D, textureID));
}

void GLFunctions::glTexImage2D(int width, int height, glConst pixelType, const void * data)
{
  GLCHECK(::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, pixelType, data));
}

void GLFunctions::glTexSubImage2D(int x, int y, int width, int height, glConst pixelType, const void * data)
{
  GLCHECK(::glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, pixelType, data));
}

uint32_t GLFunctions::glGetBindedTexture()
{
  int id = 0;
  GLCHECK(::glGetIntegerv(GL_TEXTURE_BINDING_2D, &id));
  return id;
}

void GLFunctions::glDrawElements(uint16_t indexCount)
{
  GLCHECK(::glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0));
}

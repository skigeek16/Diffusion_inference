/*
 * GLAD OpenGL Loader - Simplified for our use case
 * OpenGL 3.3 Core Profile
 */

#ifndef __GLAD_H_
#define __GLAD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* GLAD API */
#ifndef GLADAPI
  #define GLADAPI extern
#endif

#ifndef APIENTRY
  #ifdef _WIN32
    #define APIENTRY __stdcall
  #else
    #define APIENTRY
  #endif
#endif

#ifndef APIENTRYP
  #define APIENTRYP APIENTRY *
#endif

#ifndef GLAPI
  #define GLAPI GLADAPI
#endif

/* OpenGL Types */
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

/* OpenGL Constants */
#define GL_ZERO                           0
#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_NONE                           0
#define GL_NO_ERROR                       0
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_FLOAT                          0x1406
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_REPEAT                         0x2901
#define GL_LINEAR                         0x2601
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_VIEWPORT                       0x0BA2

/* Function Pointer Types */
typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLGETINTEGERVPROC)(GLenum pname, GLint *data);
typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

/* Function Pointers */
GLADAPI PFNGLGENTEXTURESPROC glad_glGenTextures;
#define glGenTextures glad_glGenTextures
GLADAPI PFNGLDELETETEXTURESPROC glad_glDeleteTextures;
#define glDeleteTextures glad_glDeleteTextures
GLADAPI PFNGLBINDTEXTUREPROC glad_glBindTexture;
#define glBindTexture glad_glBindTexture
GLADAPI PFNGLTEXPARAMETERIPROC glad_glTexParameteri;
#define glTexParameteri glad_glTexParameteri
GLADAPI PFNGLTEXIMAGE2DPROC glad_glTexImage2D;
#define glTexImage2D glad_glTexImage2D
GLADAPI PFNGLGETINTEGERVPROC glad_glGetIntegerv;
#define glGetIntegerv glad_glGetIntegerv
GLADAPI PFNGLVIEWPORTPROC glad_glViewport;
#define glViewport glad_glViewport
GLADAPI PFNGLCLEARPROC glad_glClear;
#define glClear glad_glClear
GLADAPI PFNGLCLEARCOLORPROC glad_glClearColor;
#define glClearColor glad_glClearColor

/* GLAD Loader */
typedef void* (*GLADloadproc)(const char *name);

GLADAPI int gladLoadGL(void);
GLADAPI int gladLoadGLLoader(GLADloadproc);

#ifdef __cplusplus
}
#endif

#endif /* __GLAD_H_ */

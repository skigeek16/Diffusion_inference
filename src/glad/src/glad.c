/*
 * GLAD OpenGL Loader Implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

#ifdef _WIN32
#include <windows.h>
static HMODULE libGL;

typedef void* (APIENTRYP PFNWGLGETPROCADDRESSPROC_PRIVATE)(const char*);
static PFNWGLGETPROCADDRESSPROC_PRIVATE gladGetProcAddressPtr;

static int open_gl(void) {
    libGL = LoadLibraryA("opengl32.dll");
    if(libGL != NULL) {
        gladGetProcAddressPtr = (PFNWGLGETPROCADDRESSPROC_PRIVATE)GetProcAddress(libGL, "wglGetProcAddress");
        return gladGetProcAddressPtr != NULL;
    }
    return 0;
}

static void close_gl(void) {
    if(libGL != NULL) {
        FreeLibrary(libGL);
        libGL = NULL;
    }
}

static void* get_proc(const char *namez) {
    void* result = NULL;
    if(libGL == NULL) return NULL;
    if(gladGetProcAddressPtr != NULL) {
        result = gladGetProcAddressPtr(namez);
    }
    if(result == NULL) {
        result = (void*)GetProcAddress(libGL, namez);
    }
    return result;
}
#else
#include <dlfcn.h>
static void* libGL;

static int open_gl(void) {
    libGL = dlopen("libGL.so.1", RTLD_LAZY | RTLD_GLOBAL);
    return libGL != NULL;
}

static void close_gl(void) {
    if(libGL != NULL) {
        dlclose(libGL);
        libGL = NULL;
    }
}

static void* get_proc(const char *namez) {
    void* result;
    if(libGL == NULL) return NULL;
    result = dlsym(libGL, namez);
    return result;
}
#endif

int gladLoadGL(void) {
    int status = 0;
    if(open_gl()) {
        status = gladLoadGLLoader((GLADloadproc)get_proc);
    }
    return status;
}

/* Function Pointers */
PFNGLGENTEXTURESPROC glad_glGenTextures = NULL;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures = NULL;
PFNGLBINDTEXTUREPROC glad_glBindTexture = NULL;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri = NULL;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D = NULL;
PFNGLGETINTEGERVPROC glad_glGetIntegerv = NULL;
PFNGLVIEWPORTPROC glad_glViewport = NULL;
PFNGLCLEARPROC glad_glClear = NULL;
PFNGLCLEARCOLORPROC glad_glClearColor = NULL;

int gladLoadGLLoader(GLADloadproc load) {
    if(!open_gl()) {
        return 0;
    }

    glad_glGenTextures = (PFNGLGENTEXTURESPROC)load("glGenTextures");
    glad_glDeleteTextures = (PFNGLDELETETEXTURESPROC)load("glDeleteTextures");
    glad_glBindTexture = (PFNGLBINDTEXTUREPROC)load("glBindTexture");
    glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC)load("glTexParameteri");
    glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC)load("glTexImage2D");
    glad_glGetIntegerv = (PFNGLGETINTEGERVPROC)load("glGetIntegerv");
    glad_glViewport = (PFNGLVIEWPORTPROC)load("glViewport");
    glad_glClear = (PFNGLCLEARPROC)load("glClear");
    glad_glClearColor = (PFNGLCLEARCOLORPROC)load("glClearColor");

    close_gl();
    return glad_glGenTextures != NULL;
}

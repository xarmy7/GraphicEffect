#pragma once

#include "demo.h"
#include "demo_base.h"

#include "opengl_headers.h"


#include "camera.h"

class demo_perso : public demo
{
public:
    demo_perso(platform_io IO, GL::cache& GLCache, GL::debug& GLDebug);
    virtual ~demo_perso();
    virtual void Update(const platform_io& IO);
    void RenderShadowMap();

private:
    // 3d camera
    camera Camera = {};
    
    // GL objects needed by this demo
    GLuint Program = 0;
    GLuint ProgramDepthMap = 0;
    GLuint ProgramShadowMap = 0;
    GLuint Texture = 0;

    GLuint VAO = 0;
    GLuint VertexBuffer = 0;
    int VertexCount = 0;

    unsigned int fbo = 0;
    unsigned int textureColorBuffer = 0;
    unsigned int depthMapFBO = 0;
    unsigned int depthMapText = 0;

    demo_base demoBase;
    camera cameraPlan;
};

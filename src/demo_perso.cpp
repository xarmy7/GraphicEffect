
#include <vector>

#include <imgui.h>
#include <iostream>

#include "opengl_helpers.h"
#include "opengl_helpers_wireframe.h"


#include "maths.h"
#include "mesh.h"
#include "color.h"

#include "demo_perso.h"

#include "pg.h"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024
const int LIGHT_BLOCK_BINDING_POINT = 0;
static const char* mode[]{ "Without PostProcess", "GrayScale", "Negative Color", "Kernel" };
int graphicSelection = 0;
float kernelSmoothness = 300.f;



//Vertex format
// ==================================================
struct vertex
{
    v3 Position;
    v2 UV;
};

#pragma region Shader Depth Map
static const char* gVertexShaderDepth = R"GLSL(
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}  )GLSL";

static const char* gFragmentShaderStrDepth = R"GLSL(
void main()
{                         
    gl_FragDepth = gl_FragCoord.z;
}    )GLSL";

#pragma endregion

#pragma region Shader Shadow Map
static const char* gVertexShaderStrShadow = R"GLSL(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

// Uniforms
uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uModelNormalMatrix;
uniform mat4 lightSpaceMatrix;


out vec3 vPos;
out vec3 vNormal;
out vec2 vUV;
out vec4 FragPosLightSpace;



void main()
{    
    vUV = aUV;
    vPos = vec3(uModel * vec4(aPosition, 1.0));
    vNormal = (uModelNormalMatrix * vec4(aNormal, 0.0)).xyz;
    FragPosLightSpace = lightSpaceMatrix * vec4(vPos, 1.0);
    gl_Position = uProjection * uView * vec4(vPos, 1.0);
} )GLSL";

static const char* gFragmentShaderStrShadow = R"GLSL(
// Shader outputs
out vec4 oColor;

in vec3 vPos;
in vec3 vNormal;
in vec2 vUV;
in vec4 FragPosLightSpace;

uniform int graphicSelection;
uniform float kernelSmoothness;
uniform sampler2D uEmissiveTexture;
uniform sampler2D uDiffuseTexture;
uniform sampler2D shadowMap;
uniform sampler2D uColorTexture;


// Uniforms
uniform vec3 uViewPosition;
uniform vec3 lightPos;

light_shade_result lightResult;

// Uniform blocks
layout(std140) uniform uLightBlock
{
	light uLight[LIGHT_COUNT];
};


light_shade_result get_lights_shading()
{
    lightResult = light_shade_result(vec3(0.0), vec3(0.0), vec3(0.0));
	for (int i = 0; i < LIGHT_COUNT; ++i)
    {
        light_shade_result light = light_shade(uLight[i], gDefaultMaterial.shininess, uViewPosition, vPos, normalize(vNormal));
        lightResult.ambient  += light.ambient;
        lightResult.diffuse  += light.diffuse;
        lightResult.specular += light.specular;
    }
    return lightResult;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}  

void main()
{           
    
// Compute phong shading
    light_shade_result lightResult = get_lights_shading();
    
    vec3 diffuseColor  = gDefaultMaterial.diffuse * lightResult.diffuse * texture(uDiffuseTexture, vUV).rgb;
    vec3 ambientColor  = gDefaultMaterial.ambient * lightResult.ambient * texture(uDiffuseTexture, vUV).rgb;
    vec3 specularColor = gDefaultMaterial.specular * lightResult.specular;
    vec3 emissiveColor = gDefaultMaterial.emission + texture(uEmissiveTexture, vUV).rgb;
    float shadow = ShadowCalculation(FragPosLightSpace);       
    
    oColor = vec4((ambientColor + (1-shadow) * (diffuseColor + specularColor) + emissiveColor), 1.0);
    //oColor = vec4(vec3(shadow), 1.0);
    float average = 0.2126 * oColor.r + 0.7152 * oColor.g + 0.0722 * oColor.b;


    if( graphicSelection == 1)
        oColor = vec4(average, average, average, 1.0);
    else if( graphicSelection == 2)
        oColor = vec4(vec3(1.0 - oColor), 1.0);
    else if( graphicSelection == 3)
    {
        float offset = 1.0 / kernelSmoothness;

        vec2 offsets[9] = vec2[](
            vec2(-offset,  offset), // top-left
            vec2( 0.0f,    offset), // top-center
            vec2( offset,  offset), // top-right
            vec2(-offset,  0.0f),   // center-left
            vec2( 0.0f,    0.0f),   // center-center
            vec2( offset,  0.0f),   // center-right
            vec2(-offset, -offset), // bottom-left
            vec2( 0.0f,   -offset), // bottom-center
            vec2( offset, -offset)  // bottom-right
        );

        float kernel[9] = float[](
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
        );

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(uColorTexture, vUV.st + offsets[i]));
        }
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];

        oColor = vec4(col + (ambientColor + (1-shadow) * (diffuseColor + specularColor) + emissiveColor), 1.0);
    }
        

}    )GLSL";

#pragma endregion

demo_perso::demo_perso(platform_io IO, GL::cache& GLCache, GL::debug& GLDebug)
    : demoBase(GLCache, GLDebug)
{
    char FragmentShaderConfig[] = "#define LIGHT_COUNT %d\n            ";
    snprintf(FragmentShaderConfig, ARRAY_SIZE(FragmentShaderConfig), "#define LIGHT_COUNT %d\n", demoBase.GetTavernInfo().LightCount);
    const char* FragmentShaderStrs[2] = {
           FragmentShaderConfig,
           gFragmentShaderStrShadow,
    };


    //Create render pipeline
    this->ProgramDepthMap = GL::CreateProgram(gVertexShaderDepth, gFragmentShaderStrDepth);
    this->ProgramShadowMap = GL::CreateProgramEx(1, &gVertexShaderStrShadow, 2, FragmentShaderStrs, true);
    //this->ProgramShadowMap = GL::CreateProgram(gVertexShaderStrShadow, gFragmentShaderStrShadow, true);

    //Create depth map
    glGenFramebuffers(1, &depthMapFBO);

    //Create 2D texture for depthMapFBO
    glGenTextures(1, &depthMapText);
    glBindTexture(GL_TEXTURE_2D, depthMapText);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    //Attach to framebuffer's depthMap
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapText, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);


    //Create Frame Buffer (FBO)
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);


    //Generate texture
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    //Create RenderBuffer
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Generation mesh
    {
        //Create a descriptor based on the `struct vertex` format
        vertex_descriptor Descriptor = {};
        Descriptor.Stride = sizeof(vertex);
        Descriptor.HasUV = true;
        Descriptor.PositionOffset = OFFSETOF(vertex, Position);
        Descriptor.UVOffset = OFFSETOF(vertex, UV);

        //Create a cube in RAM
        vertex Quad[6];
        this->VertexCount = 6;
        Mesh::BuildQuad(Quad, Quad + this->VertexCount, Descriptor);

        // Upload cube to gpu (VRAM)
        glGenBuffers(1, &this->VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, this->VertexCount * sizeof(vertex), Quad, GL_STATIC_DRAW);
    }

    //Generation texture
    {
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        GL::UploadCheckerboardTexture(64, 64, 8);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    
    //Create a vertex array
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)OFFSETOF(vertex, Position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)OFFSETOF(vertex, UV));
    glBindVertexArray(0);

    glUseProgram(ProgramShadowMap);
    glUniform1i(glGetUniformLocation(ProgramShadowMap, "uDiffuseTexture"), 0);
    glUniform1i(glGetUniformLocation(ProgramShadowMap, "uEmissiveTexture"), 1);
    glUniform1i(glGetUniformLocation(ProgramShadowMap, "shadowMap"), 2);
    glUniformBlockBinding(ProgramShadowMap, glGetUniformBlockIndex(ProgramShadowMap, "uLightBlock"), 0);

}

demo_perso::~demo_perso()
{
    // Cleanup GL
    glDeleteTextures(1, &Texture);
    glDeleteBuffers(1, &VertexBuffer);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(Program);
    glDeleteProgram(ProgramDepthMap);
    glDeleteProgram(ProgramShadowMap);
}

static void DrawQuad(GLuint Program, mat4 ModelViewProj)
{
    glUniformMatrix4fv(glGetUniformLocation(Program, "uModelViewProj"), 1, GL_FALSE, ModelViewProj.e);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void demo_perso::Update(const platform_io& IO)
{
    Camera = CameraUpdateFreefly(Camera, IO.CameraInputs);

    //Compute model-view-proj and send it to shader
    mat4 ProjectionMatrix = Mat4::Perspective(Math::ToRadians(60.f), (float)IO.WindowWidth / (float)IO.WindowHeight, 0.1f, 100.f);
    mat4 ViewMatrix = CameraGetInverseMatrix(Camera);

    //Light matrices
    float near_plane = -10.0f, far_plane = 7.5f;
    std::vector<GL::light> Lights = demoBase.GetTavernInfo().GetLight();
    ImGui::SliderFloat4("Position", Lights[0].Position.e, -4.f, 4.f);

    mat4 lightProjection = Mat4::Orthographic(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    mat4 lightView = Mat4::LookAt(v3{Lights[0].Position.x, Lights[0].Position.y, Lights[0].Position.z},
                                  v3{ 0.0f, 0.0f, 0.0f },
                                  v3{ 0.0f, 1.0f, 0.0f });
    
    mat4 lightSpaceMatrix = lightProjection * lightView;

    //Standard quad
    v3 ObjectPosition = { 0.f, 0.f, -3.f };
    mat4 ModelMatrix = Mat4::Translate(ObjectPosition);
    mat4 NormalMatrix = Mat4::Transpose(Mat4::Inverse(ModelMatrix));

    glUseProgram(ProgramDepthMap);
    glUniformMatrix4fv(glGetUniformLocation(ProgramDepthMap, "lightSpaceMatrix"),1 , GL_FALSE, lightSpaceMatrix.e);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, depthMapText);
    demoBase.RenderTavern(lightProjection, lightView, ModelMatrix);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);


    glUseProgram(ProgramShadowMap);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMapText);
   
    glUniform1i(glGetUniformLocation(ProgramShadowMap, "graphicSelection"), graphicSelection);
    glUniform1f(glGetUniformLocation(ProgramShadowMap, "kernelSmoothness"), kernelSmoothness);

    glUniformMatrix4fv(glGetUniformLocation(ProgramShadowMap, "uProjection"), 1, GL_FALSE, ProjectionMatrix.e);
    glUniformMatrix4fv(glGetUniformLocation(ProgramShadowMap, "uModel"), 1, GL_FALSE, ModelMatrix.e);
    glUniformMatrix4fv(glGetUniformLocation(ProgramShadowMap, "uView"), 1, GL_FALSE, ViewMatrix.e);
    glUniformMatrix4fv(glGetUniformLocation(ProgramShadowMap, "uModelNormalMatrix"), 1, GL_FALSE, NormalMatrix.e);
    glUniformMatrix4fv(glGetUniformLocation(ProgramShadowMap, "lightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix.e);
    glUniform3fv(glGetUniformLocation(ProgramShadowMap, "uViewPosition"), 1, Camera.Position.e);
    glUniform3fv(glGetUniformLocation(ProgramShadowMap, "lightPos"), 1, Lights[0].Position.e);

    //Draw tavern scene
    glViewport(0, 0, (float)IO.WindowWidth, (float)IO.WindowHeight);
    glClearColor(0.2f, 0.3f, 0.2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderShadowMap();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    if (ImGui::TreeNodeEx("Settings"))
    {
        ImGui::Text("Select a graphic Mode");
        ImGui::ListBox("\n", &graphicSelection, mode, IM_ARRAYSIZE(mode));
        
        ImGui::Text("\n");
        ImGui::Text("Kernel smooth");
        ImGui::SliderFloat("", &kernelSmoothness, 0.f, 1000.f);
        
        ImGui::TreePop();
    }

    ImVec2 wsize = ImGui::GetWindowSize();
    ImGui::Image((ImTextureID)depthMapText, wsize, ImVec2(0, 1), ImVec2(1, 0));

    //Draw origin
    PG::DebugRenderer()->DrawAxisGizmo(Mat4::Translate({ 0.f, 0.f, 0.f }), true, false);
}

void demo_perso::RenderShadowMap()
{
    glEnable(GL_DEPTH_TEST);

    // Use shader and configure its uniforms
    glUseProgram(ProgramShadowMap);

    // Bind uniform buffer and textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, demoBase.GetTavernInfo().DiffuseTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, demoBase.GetTavernInfo().EmissiveTexture);
    glActiveTexture(GL_TEXTURE0); // Reset active texture just in case

    // Draw mesh
    glBindVertexArray(demoBase.GetVAO());
    glDrawArrays(GL_TRIANGLES, 0, demoBase.GetTavernInfo().MeshVertexCount);
}
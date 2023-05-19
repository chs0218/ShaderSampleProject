#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Dependencies\glew.h"

struct PosCol {
	std::vector<std::array<float, 3>> m_vVertices;
};

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	bool IsInitialized();
	void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);
	void Class0310_Rendering();
	void DrawParticle();
	void DrawSandBox();
    void DrawAlphaClear();
    void DrawHorizontalLine();
    void DrawTextureSandBox();
    void DrawVertexFlag();
private:
	void Initialize(int windowSizeX, int windowSizeY);
	void SetVBO(GLuint nVBO, int vboLoc, int nFloatnum);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	void CreateVertexBufferObjects();
	void GetGLPosition(float x, float y, float *newX, float *newY);
	void Class0310();
	void CreateParticleVBO(int numParticleCount);
	void CreateSandBoxVBO();
    void CreateHorizontalVBO(int iHorizontalVertexCount);
    void CreateVertexFlagVBO();
    void CreateTextures();
    GLuint CreatePngTexture(char* filePath, GLuint samplingMethod);

	GLuint m_ParticleShader = -1;
	GLuint m_ParticleVBO = -1;
	GLuint m_ParticleVelVBO = -1;
	GLuint m_ParticleEmitTimeVBO = -1;
	GLuint m_ParticleLifeTimeVBO = -1;
	GLuint m_ParticlePeriodVBO = -1;
	GLuint m_ParticleAmpVBO = -1;
	GLuint m_ParticleValueVBO = -1;
	GLuint m_ParticleColorVBO = -1;
	GLuint m_ParticlePosColVBO = -1;
	GLuint m_ParticleVertexCount = -1;
    GLuint m_ParticleTexture = -1;

	//for fragment sandbox
	GLuint m_FragmentSandboxShader = 0;
	GLuint m_FragmentSandboxVBO = 0;

    //for trail
    GLuint m_AlphaClearShader = 0;
    GLuint m_AlphaClearVBO = 0;

    //for vertex anim
    GLuint m_VertexSandBoxShader = 0;
    GLuint m_HorizontalLineVBO = 0;
    GLuint m_HorizontalLineVertexCount = 0;

    GLuint m_TextureSandboxShader = 0;
    GLuint m_TextureSandboxVBO = 0;
    GLuint m_CheckerBoardTexture = 0;
    GLuint m_ExplosionTexture = 0;
    GLuint m_RGBTexture = 0;

    //for vertex flag
    GLuint m_VertexFlagShader = 0;
    GLuint m_VertexFlagVBO = 0;
    GLuint gDummyVertexCount = 0;

    GLuint m_0Texture = 0;
    GLuint m_1Texture = 0;
    GLuint m_2Texture = 0;
    GLuint m_3Texture = 0;
    GLuint m_4Texture = 0;
    GLuint m_5Texture = 0;
    GLuint m_6Texture = 0;
    GLuint m_MultiTexture = 0;

	bool m_Initialized = false;
	
	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;

	GLuint m_testVBO = 0;
	GLuint m_testVBO1 = 0;
	GLuint m_ColorVBO = 0;
};


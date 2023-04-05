#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Dependencies\glew.h"

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	bool IsInitialized();
	void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);
	void Class0310_Rendering();
	void DrawParticle();

private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	void CreateVertexBufferObjects();
	void GetGLPosition(float x, float y, float *newX, float *newY);
	void Class0310();

	// Particle
	void CreateParticleVBO(int numParticleCount);
	GLuint m_ParticleShader = 0;
	GLuint m_ParticleVBO = 0;
	GLuint m_ParticleVelVBO = 0;
	GLuint m_ParticleEmitTimeVBO = 0;
	GLuint m_ParticleLifeTimeVBO = 0;
	GLuint m_ParticleAmpVBO = 0;
	GLuint m_ParticlePeriodVBO = 0;
	GLuint m_ParticleCircleVBO = 0;
	GLuint m_ParticleVertexCount = 0;

	bool m_Initialized = false;
	
	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;

	GLuint m_testVBO = 0;
	GLuint m_testVBO1 = 0;
	GLuint m_ColorVBO = 0;

	float fTimeElapsed = 0.0f;
};


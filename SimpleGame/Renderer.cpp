#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
	Class0310();
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_ParticleShader = CompileShaders("./Shaders/Particle.vs", "./Shaders/Particle.fs");
	
	//Create VBOs
	CreateVertexBufferObjects();
	CreateParticleVBO(10000);

	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.";

	return ShaderProgram;
}

void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float g_time = 0.f;

void Renderer::Class0310_Rendering()
{	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), 0, 0, 0, 1);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), 1, 1, 1, 1);

	int attribLocation_Position = -1;
	attribLocation_Position = glGetAttribLocation(m_SolidRectShader,
		"a_Position");
	glEnableVertexAttribArray(attribLocation_Position);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
	glVertexAttribPointer(attribLocation_Position, 3, GL_FLOAT, 
		GL_FALSE, 0, 0);

	int attribLocation_Position1 = glGetAttribLocation(m_SolidRectShader, 
		"a_Position1");
	glEnableVertexAttribArray(attribLocation_Position1);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO1);
	glVertexAttribPointer(attribLocation_Position1, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 3, 0);

	int uniformScale = glGetUniformLocation(m_SolidRectShader, "u_Scale");
	glUniform1f(uniformScale, g_time);
	g_time += 0.016;
	if (g_time > 1.f)
		g_time = 0.f;

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::DrawParticle()
{
	GLuint program = m_ParticleShader;
	glUseProgram(program);

	int posLoc = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(posLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE,
							0, 0);

	int velLoc = glGetAttribLocation(program, "a_Vel");
	glEnableVertexAttribArray(velLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVelVBO);
	glVertexAttribPointer(velLoc, 3, GL_FLOAT, GL_FALSE,
		0, 0);

	int emitTimeLoc = glGetAttribLocation(program, "a_EmitTime");
	glEnableVertexAttribArray(emitTimeLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glVertexAttribPointer(emitTimeLoc, 1, GL_FLOAT, GL_FALSE,
		0, 0);

	int lifeTimeLoc = glGetAttribLocation(program, "a_LifeTime");
	glEnableVertexAttribArray(lifeTimeLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glVertexAttribPointer(lifeTimeLoc, 1, GL_FLOAT, GL_FALSE,
		0, 0);

	int periodLoc = glGetAttribLocation(program, "a_Period");
	glEnableVertexAttribArray(periodLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePeriodVBO);
	glVertexAttribPointer(periodLoc, 1, GL_FLOAT, GL_FALSE,
		0, 0);

	int ampLoc = glGetAttribLocation(program, "a_Amp");
	glEnableVertexAttribArray(ampLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleAmpVBO);
	glVertexAttribPointer(ampLoc, 1, GL_FLOAT, GL_FALSE,
		0, 0);

	int valueLoc = glGetAttribLocation(program, "a_Value");
	glEnableVertexAttribArray(valueLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleValueVBO);
	glVertexAttribPointer(valueLoc, 1, GL_FLOAT, GL_FALSE,
		0, 0);

	int timeLoc = glGetUniformLocation(program, "u_Time");
	glUniform1f(timeLoc, g_time);
	int accelLoc = glGetUniformLocation(program, "u_Accel");
	glUniform3f(accelLoc, 0.f, -2.8f, 0.f);

	g_time += 0.01;

	glDrawArrays(GL_TRIANGLES, 0, m_ParticleVertexCount);
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

void Renderer::Class0310()
{
	float vertices[] = { 0, 0, 0, 1, 0, 0, 1, 1, 0 };

	glGenBuffers(1, &m_testVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, 
		GL_STATIC_DRAW);

	float vertices1[] = { -1, -1, 0, 
						0, -1, 0, 
						0, 0, 0 };

	glGenBuffers(1, &m_testVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1,
		GL_STATIC_DRAW);

	float color[] = {  1, 0, 0, 1,
					   0, 1, 0, 1,
					   0, 0, 1, 1 };

	glGenBuffers(1, &m_ColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color,
		GL_STATIC_DRAW);

	/*int size = 40000000000000;
	float* testTemp = new float[size];
	memset(testTemp, 1, sizeof(float)*size);

	GLuint testVBO1 = 0;
	glGenBuffers(1, &testVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, testVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*size,
		testTemp,
		GL_STATIC_DRAW);*/

}

void Renderer::CreateParticleVBO(int numParticleCount)
{
	int vertexCount = 6;
	int particleCount = numParticleCount;
	int floatCount = 3;
	int totalFloatCount = particleCount
		* vertexCount
		* floatCount;
	int totalFloatCountSingle = particleCount
		* vertexCount
		* 1;

	m_ParticleVertexCount = particleCount * vertexCount;

	float* vertices = NULL;
	vertices = new float[totalFloatCount];

	float particleSize = 0.01f;

	int index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float particelCenterX = 0.f; // 2.f*(((float)rand() / (float)RAND_MAX) - 0.5f);
		float particelCenterY = 0.f; // 2.f*(((float)rand() / (float)RAND_MAX) - 0.5f);
		vertices[index] = particelCenterX - particleSize; index++;
		vertices[index] = particelCenterY + particleSize; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = particelCenterX - particleSize; index++;
		vertices[index] = particelCenterY - particleSize; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = particelCenterX + particleSize; index++;
		vertices[index] = particelCenterY + particleSize; index++;
		vertices[index] = 0.f; index++;

		vertices[index] = particelCenterX + particleSize; index++;
		vertices[index] = particelCenterY + particleSize; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = particelCenterX - particleSize; index++;
		vertices[index] = particelCenterY - particleSize; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = particelCenterX + particleSize; index++;
		vertices[index] = particelCenterY - particleSize; index++;
		vertices[index] = 0.f; index++;
	}

	glGenBuffers(1, &m_ParticleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCount,
		vertices, GL_STATIC_DRAW);
	delete[] vertices;

	float* verticesVel = NULL;
	verticesVel = new float[totalFloatCount];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float velX = 2.f*(((float)rand() / (float)RAND_MAX) - 0.5f);
		float velY = 4.f*(((float)rand() / (float)RAND_MAX));
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;;
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;
		
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;;
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;
	}

	glGenBuffers(1, &m_ParticleVelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCount,
		verticesVel, GL_STATIC_DRAW);
	delete[] verticesVel;
	
	//EmitTime
	float* verticesEmitTime = NULL;
	verticesEmitTime = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float emitTime = 10.f*(((float)rand() / (float)RAND_MAX));
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;;
	}

	glGenBuffers(1, &m_ParticleEmitTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCountSingle,
		verticesEmitTime, GL_STATIC_DRAW);
	delete[] verticesEmitTime;

	//LifeTime
	float* verticesLifeTime = NULL;
	verticesLifeTime = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float lifeTime = 1.f*(((float)rand() / (float)RAND_MAX));
		verticesLifeTime[index] = lifeTime; index++;
		verticesLifeTime[index] = lifeTime; index++;
		verticesLifeTime[index] = lifeTime; index++;
		verticesLifeTime[index] = lifeTime; index++;
		verticesLifeTime[index] = lifeTime; index++;
		verticesLifeTime[index] = lifeTime; index++;;
	}

	glGenBuffers(1, &m_ParticleLifeTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCountSingle,
		verticesLifeTime, GL_STATIC_DRAW);
	delete[] verticesLifeTime;

	//period
	float* verticesPeriodTime = NULL;
	verticesPeriodTime = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float period = 1.f*(((float)rand() / (float)RAND_MAX));
		verticesPeriodTime[index] = period; index++;
		verticesPeriodTime[index] = period; index++;
		verticesPeriodTime[index] = period; index++;
		verticesPeriodTime[index] = period; index++;
		verticesPeriodTime[index] = period; index++;
		verticesPeriodTime[index] = period; index++;;
	}

	glGenBuffers(1, &m_ParticlePeriodVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePeriodVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCountSingle,
		verticesPeriodTime, GL_STATIC_DRAW);
	delete[] verticesPeriodTime;

	//amp
	float* verticesAmpTime = NULL;
	verticesAmpTime = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float amp = 1.f*(2.f*(((float)rand() / (float)RAND_MAX))-1.f);
		verticesAmpTime[index] = amp; index++;
		verticesAmpTime[index] = amp; index++;
		verticesAmpTime[index] = amp; index++;
		verticesAmpTime[index] = amp; index++;
		verticesAmpTime[index] = amp; index++;
		verticesAmpTime[index] = amp; index++;;
	}

	glGenBuffers(1, &m_ParticleAmpVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleAmpVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCountSingle,
		verticesAmpTime, GL_STATIC_DRAW);
	delete[] verticesAmpTime;

	//value
	float* verticesValue = NULL;
	verticesValue = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float value = 1.f*((float)rand() / (float)RAND_MAX);
		verticesValue[index] = value; index++;
		verticesValue[index] = value; index++;
		verticesValue[index] = value; index++;
		verticesValue[index] = value; index++;
		verticesValue[index] = value; index++;
		verticesValue[index] = value; index++;;
	}

	glGenBuffers(1, &m_ParticleValueVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleValueVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCountSingle,
		verticesValue, GL_STATIC_DRAW);
	delete[] verticesValue;	
}

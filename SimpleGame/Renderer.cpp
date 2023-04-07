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

void Renderer::SetVBO(GLuint nVBO, int vboLoc, int nFloatnum)
{
	glEnableVertexAttribArray(vboLoc);
	glBindBuffer(GL_ARRAY_BUFFER, nVBO);
	glVertexAttribPointer(vboLoc, nFloatnum, GL_FLOAT, GL_FALSE, 0, 0);
}

void Renderer::DrawParticle()
{
	GLuint program = m_ParticleShader;
	glUseProgram(program);

	int PosLoc = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(PosLoc);
	int ColorLoc = glGetAttribLocation(program, "a_Color");
	glEnableVertexAttribArray(ColorLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColVBO);
	glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColVBO);
	glVertexAttribPointer(ColorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (GLvoid*)(sizeof(float) * 3));

	SetVBO(m_ParticleVelVBO, glGetAttribLocation(program, "a_Vel"), 3);
	SetVBO(m_ParticleEmitTimeVBO, glGetAttribLocation(program, "a_EmitTime"), 1);
	SetVBO(m_ParticleLifeTimeVBO, glGetAttribLocation(program, "a_LifeTime"), 1);
	SetVBO(m_ParticlePeriodVBO, glGetAttribLocation(program, "a_Period"), 1);
	SetVBO(m_ParticleAmpVBO, glGetAttribLocation(program, "a_Amp"), 1);
	SetVBO(m_ParticleValueVBO, glGetAttribLocation(program, "a_Value"), 1);

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

	m_ParticleVertexCount = particleCount * vertexCount;

	std::vector<std::array<float, 3>> v_Vertices;
	std::vector<std::array<float, 3>> v_Velocities;
	std::vector<std::array<float, 4>> v_Colors;
	std::vector<float> v_emitTime;
	std::vector<float> v_lifeTime;
	std::vector<float> v_Amp;
	std::vector<float> v_Period;
	std::vector<float> v_Values;

	std::vector<float> v_PosCol;

	float particleSize = 0.01f;

	for (int i = 0; i < numParticleCount; ++i)
	{
		float particleCenterX = 0.f/*2.0f * (((float)rand() / (float) RAND_MAX) - 0.5f)*/;
		float particleCenterY = 0.f/*2.0f * (((float)rand() / (float)RAND_MAX) - 0.5f)*/;

		v_Vertices.emplace_back(std::array<float, 3>{particleCenterX - particleSize, particleCenterY + particleSize, 0.0f});
		v_Vertices.emplace_back(std::array<float, 3>{particleCenterX - particleSize, particleCenterY - particleSize, 0.0f});
		v_Vertices.emplace_back(std::array<float, 3>{particleCenterX + particleSize, particleCenterY + particleSize, 0.0f});
		v_Vertices.emplace_back(std::array<float, 3>{particleCenterX - particleSize, particleCenterY - particleSize, 0.0f});
		v_Vertices.emplace_back(std::array<float, 3>{particleCenterX + particleSize, particleCenterY - particleSize, 0.0f});
		v_Vertices.emplace_back(std::array<float, 3>{particleCenterX + particleSize, particleCenterY + particleSize, 0.0f});
	}

	for (int i = 0; i < numParticleCount; ++i)
	{
		float velX = 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
		float velY = 4.f * (((float)rand() / (float)RAND_MAX));

		v_Velocities.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
		v_Velocities.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
		v_Velocities.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
		v_Velocities.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
		v_Velocities.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
		v_Velocities.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
	}

	for (int i = 0; i < numParticleCount; ++i)
	{
		float randTime = ((float)rand() / (float)RAND_MAX) * 1.0f;

		v_emitTime.emplace_back(randTime);
		v_emitTime.emplace_back(randTime);
		v_emitTime.emplace_back(randTime);
		v_emitTime.emplace_back(randTime);
		v_emitTime.emplace_back(randTime);
		v_emitTime.emplace_back(randTime);
	}

	for (int i = 0; i < numParticleCount; ++i)
	{
		float randTime = 1.f * (((float)rand() / (float)RAND_MAX));

		v_lifeTime.emplace_back(randTime);
		v_lifeTime.emplace_back(randTime);
		v_lifeTime.emplace_back(randTime);
		v_lifeTime.emplace_back(randTime);
		v_lifeTime.emplace_back(randTime);
		v_lifeTime.emplace_back(randTime);
	}

	for (int i = 0; i < numParticleCount; ++i)
	{
		float fRandomPeriod = 1.f * (((float)rand() / (float)RAND_MAX));

		v_Period.emplace_back(fRandomPeriod);
		v_Period.emplace_back(fRandomPeriod);
		v_Period.emplace_back(fRandomPeriod);
		v_Period.emplace_back(fRandomPeriod);
		v_Period.emplace_back(fRandomPeriod);
		v_Period.emplace_back(fRandomPeriod);
	}

	for (int i = 0; i < numParticleCount; ++i)
	{
		float fRandomAmp = 1.f * (2.f * (((float)rand() / (float)RAND_MAX)) - 1.f);

		v_Amp.emplace_back(fRandomAmp);
		v_Amp.emplace_back(fRandomAmp);
		v_Amp.emplace_back(fRandomAmp);
		v_Amp.emplace_back(fRandomAmp);
		v_Amp.emplace_back(fRandomAmp);
		v_Amp.emplace_back(fRandomAmp);
	}

	for (int i = 0; i < numParticleCount; ++i)
	{
		float fRandomCircle = 1.f * ((float)rand() / (float)RAND_MAX);

		v_Values.emplace_back(fRandomCircle);
		v_Values.emplace_back(fRandomCircle);
		v_Values.emplace_back(fRandomCircle);
		v_Values.emplace_back(fRandomCircle);
		v_Values.emplace_back(fRandomCircle);
		v_Values.emplace_back(fRandomCircle);
	}


	for (int i = 0; i < numParticleCount; ++i)
	{
		float fRandomR = 1.f * (((float)rand() / (float)RAND_MAX));
		float fRandomG = 1.f * (((float)rand() / (float)RAND_MAX));
		float fRandomB = 1.f * (((float)rand() / (float)RAND_MAX));
		float fRandomA = 1.f * (((float)rand() / (float)RAND_MAX));

		v_Colors.emplace_back(std::array<float, 4>{fRandomR, fRandomG, fRandomB, fRandomA});
		v_Colors.emplace_back(std::array<float, 4>{fRandomR, fRandomG, fRandomB, fRandomA});
		v_Colors.emplace_back(std::array<float, 4>{fRandomR, fRandomG, fRandomB, fRandomA});
		v_Colors.emplace_back(std::array<float, 4>{fRandomR, fRandomG, fRandomB, fRandomA});
		v_Colors.emplace_back(std::array<float, 4>{fRandomR, fRandomG, fRandomB, fRandomA});
		v_Colors.emplace_back(std::array<float, 4>{fRandomR, fRandomG, fRandomB, fRandomA});
	}

	for (int i = 0; i < numParticleCount; ++i)
	{
		float particleCenterX = 0.f/*2.0f * (((float)rand() / (float) RAND_MAX) - 0.5f)*/;
		float particleCenterY = 0.f/*2.0f * (((float)rand() / (float)RAND_MAX) - 0.5f)*/;

		float fRandomR = 1.f * (((float)rand() / (float)RAND_MAX));
		float fRandomG = 1.f * (((float)rand() / (float)RAND_MAX));
		float fRandomB = 1.f * (((float)rand() / (float)RAND_MAX));
		float fRandomA = 1.f * (((float)rand() / (float)RAND_MAX));

		v_PosCol.emplace_back(particleCenterX - particleSize);
		v_PosCol.emplace_back(particleCenterY + particleSize);
		v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(fRandomR);
		v_PosCol.emplace_back(fRandomG);
		v_PosCol.emplace_back(fRandomB);
		v_PosCol.emplace_back(fRandomA);

		v_PosCol.emplace_back(particleCenterX - particleSize);
		v_PosCol.emplace_back(particleCenterY - particleSize);
		v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(fRandomR);
		v_PosCol.emplace_back(fRandomG);
		v_PosCol.emplace_back(fRandomB);
		v_PosCol.emplace_back(fRandomA);

		v_PosCol.emplace_back(particleCenterX + particleSize);
		v_PosCol.emplace_back(particleCenterY + particleSize);
		v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(fRandomR);
		v_PosCol.emplace_back(fRandomG);
		v_PosCol.emplace_back(fRandomB);
		v_PosCol.emplace_back(fRandomA);

		v_PosCol.emplace_back(particleCenterX - particleSize);
		v_PosCol.emplace_back(particleCenterY - particleSize);
		v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(fRandomR);
		v_PosCol.emplace_back(fRandomG);
		v_PosCol.emplace_back(fRandomB);
		v_PosCol.emplace_back(fRandomA);

		v_PosCol.emplace_back(particleCenterX + particleSize);
		v_PosCol.emplace_back(particleCenterY - particleSize);
		v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(fRandomR);
		v_PosCol.emplace_back(fRandomG);
		v_PosCol.emplace_back(fRandomB);
		v_PosCol.emplace_back(fRandomA);

		v_PosCol.emplace_back(particleCenterX + particleSize);
		v_PosCol.emplace_back(particleCenterY + particleSize);
		v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(fRandomR);
		v_PosCol.emplace_back(fRandomG);
		v_PosCol.emplace_back(fRandomB);
		v_PosCol.emplace_back(fRandomA);
	}

	glGenBuffers(1, &m_ParticleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * v_Vertices.size(), v_Vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ParticleVelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * v_Velocities.size(), v_Velocities.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ParticleEmitTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * v_emitTime.size(), v_emitTime.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ParticleLifeTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * v_lifeTime.size(), v_lifeTime.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ParticleAmpVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleAmpVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * v_Amp.size(), v_Amp.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ParticlePeriodVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePeriodVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * v_Period.size(), v_Period.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ParticleValueVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleValueVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * v_Values.size(), v_Values.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ParticleColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * v_Colors.size(), v_Colors.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ParticlePosColVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * v_PosCol.size(), v_PosCol.data(), GL_STATIC_DRAW);
}

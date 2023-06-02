#include "stdafx.h"
#include "Renderer.h"
#include "LoadPng.h"
#include <assert.h>

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
	m_FragmentSandboxShader = CompileShaders("./Shaders/SandBox.vs", "./Shaders/SandBox.fs");
    m_AlphaClearShader = CompileShaders("./Shaders/AlphaClear.vs", "./Shaders/AlphaClear.fs");
    m_VertexSandBoxShader = CompileShaders("./Shaders/VertexSandbox.vs", "./Shaders/VertexSandbox.fs");
    m_TextureSandboxShader = CompileShaders("./Shaders/TextureSandBox.vs", "./Shaders/TextureSandBox.fs");
    m_VertexFlagShader = CompileShaders("./Shaders/Flag_By_Vertex.vs", "./Shaders/Flag_By_Vertex.fs");
    m_DrawTextureShader = CompileShaders("./Shaders/DrawTexture.vs", "./Shaders/DrawTexture.fs");

	//Create VBOs
	CreateVertexBufferObjects();
	CreateParticleVBO(10000);
	CreateSandBoxVBO();
    CreateHorizontalVBO(50);
    CreateVertexFlagVBO();

    //Create FrameBufferObjects
    CreateFBOs();

    //Create Textures
    CreateTextures();

    //Load Textures
    m_RGBTexture = CreatePngTexture("./rgb.png", GL_NEAREST);
    m_0Texture = CreatePngTexture("./Textures/0.png", GL_NEAREST);
    m_1Texture = CreatePngTexture("./Textures/1.png", GL_NEAREST);
    m_2Texture = CreatePngTexture("./Textures/2.png", GL_NEAREST);
    m_3Texture = CreatePngTexture("./Textures/3.png", GL_NEAREST);
    m_4Texture = CreatePngTexture("./Textures/4.png", GL_NEAREST);
    m_5Texture = CreatePngTexture("./Textures/5.png", GL_NEAREST);
    m_MultiTexture = CreatePngTexture("./Textures/6.png", GL_NEAREST);
    m_ParticleTexture = CreatePngTexture("./Textures/particle.png", GL_NEAREST);
    m_ExplosionTexture = CreatePngTexture("./Textures/explosion.png", GL_NEAREST);
    m_FlagTexture = CreatePngTexture("./Textures/korea.png", GL_NEAREST);

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
    std::vector<std::array<float, 5>> v_PosTex;

    v_PosTex.emplace_back(std::array<float, 5>{-0.5f, 0.5f, 0.f, 0.f, 0.f});
    v_PosTex.emplace_back(std::array<float, 5>{-0.5f, -0.5f, 0.f, 0.f, 1.f});
    v_PosTex.emplace_back(std::array<float, 5>{0.5f, 0.5f, 0.f, 1.f, 0.f});

    v_PosTex.emplace_back(std::array<float, 5>{0.5f, 0.5f, 0.f, 1.f, 0.f});
    v_PosTex.emplace_back(std::array<float, 5>{ -0.5f, -0.5f, 0.f, 0.f, 1.f});
    v_PosTex.emplace_back(std::array<float, 5>{0.5f, -0.5f, 0.f, 1.f, 1.f});

    glGenBuffers(1, &m_TextureSandboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_TextureSandboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * v_PosTex.size(), v_PosTex.data(), GL_STATIC_DRAW);

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
    glBindFramebuffer(GL_FRAMEBUFFER, m_B_FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    GLenum drawBuffers = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, &drawBuffers);

	GLuint program = m_ParticleShader;
	glUseProgram(program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int PosLoc = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(PosLoc);
	int ColorLoc = glGetAttribLocation(program, "a_Color");
	glEnableVertexAttribArray(ColorLoc);
    int UVLoc = glGetAttribLocation(program, "a_UV");
    glEnableVertexAttribArray(UVLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColVBO);
	glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColVBO);
	glVertexAttribPointer(ColorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * 3));

    glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColVBO);
    glVertexAttribPointer(UVLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * 7));

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

	g_time += 0.016;

    int texULoc = glGetUniformLocation(program, "u_Texture");
    glUniform1i(texULoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ParticleTexture);

	glDrawArrays(GL_TRIANGLES, 0, m_ParticleVertexCount);

	glDisable(GL_BLEND);
}

void Renderer::DrawSandBox()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_A_FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    GLenum drawBuffers[5] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(5, drawBuffers);

	GLuint program = m_FragmentSandboxShader;
	glUseProgram(program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int PosLoc = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(PosLoc);
	int TexLoc = glGetAttribLocation(program, "a_UV");
	glEnableVertexAttribArray(TexLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);
	glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);
	glVertexAttribPointer(TexLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

    int timeLoc = glGetUniformLocation(program, "u_Time");
    glUniform1f(timeLoc, g_time);
    //g_time += 0.008f;

    int pointULoc = glGetUniformLocation(program, "u_Point");
    glUniform2f(pointULoc, 0.3f, 0.3f);

    int pointsULoc = glGetUniformLocation(program, "u_Points");
    std::vector<float> points = {0.1f, 0.1f, 0.5f, 0.5f, 0.8f, 0.8f};
    glUniform2fv(pointsULoc, points.size(), points.data());

    int texULoc = glGetUniformLocation(program, "u_Texture");
    glUniform1i(texULoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_RGBTexture);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisable(GL_BLEND);
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

	float particleSize = 0.1f;

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
        
        v_PosCol.emplace_back(0.0f);
        v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(particleCenterX - particleSize);
		v_PosCol.emplace_back(particleCenterY - particleSize);
		v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(fRandomR);
		v_PosCol.emplace_back(fRandomG);
		v_PosCol.emplace_back(fRandomB);
		v_PosCol.emplace_back(fRandomA);

        v_PosCol.emplace_back(0.0f);
        v_PosCol.emplace_back(1.0f);

		v_PosCol.emplace_back(particleCenterX + particleSize);
		v_PosCol.emplace_back(particleCenterY + particleSize);
		v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(fRandomR);
		v_PosCol.emplace_back(fRandomG);
		v_PosCol.emplace_back(fRandomB);
		v_PosCol.emplace_back(fRandomA);

        v_PosCol.emplace_back(1.0f);
        v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(particleCenterX - particleSize);
		v_PosCol.emplace_back(particleCenterY - particleSize);
		v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(fRandomR);
		v_PosCol.emplace_back(fRandomG);
		v_PosCol.emplace_back(fRandomB);
		v_PosCol.emplace_back(fRandomA);

        v_PosCol.emplace_back(0.0f);
        v_PosCol.emplace_back(1.0f);

		v_PosCol.emplace_back(particleCenterX + particleSize);
		v_PosCol.emplace_back(particleCenterY - particleSize);
		v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(fRandomR);
		v_PosCol.emplace_back(fRandomG);
		v_PosCol.emplace_back(fRandomB);
		v_PosCol.emplace_back(fRandomA);

        v_PosCol.emplace_back(1.0f);
        v_PosCol.emplace_back(1.0f);

		v_PosCol.emplace_back(particleCenterX + particleSize);
		v_PosCol.emplace_back(particleCenterY + particleSize);
		v_PosCol.emplace_back(0.0f);

		v_PosCol.emplace_back(fRandomR);
		v_PosCol.emplace_back(fRandomG);
		v_PosCol.emplace_back(fRandomB);
		v_PosCol.emplace_back(fRandomA);

        v_PosCol.emplace_back(1.0f);
        v_PosCol.emplace_back(0.0f);
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

void Renderer::CreateSandBoxVBO()
{
	std::vector<std::array<float, 5>> v_PosTex;

	v_PosTex.emplace_back(std::array<float, 5>{-1.f, -1.f, 0.f, 0.f, 1.f});
	v_PosTex.emplace_back(std::array<float, 5>{-1.f, 1.f, 0.f, 0.f, 0.f});
	v_PosTex.emplace_back(std::array<float, 5>{1.f, 1.f, 0.f, 1.f, 0.f});

	v_PosTex.emplace_back(std::array<float, 5>{-1.f, -1.f, 0.f, 0.f, 1.f});
	v_PosTex.emplace_back(std::array<float, 5>{ 1.f, 1.f, 0.f, 1.f, 0.f});
	v_PosTex.emplace_back(std::array<float, 5>{1.f, -1.f, 0.f, 1.f, 1.f});

	glGenBuffers(1, &m_FragmentSandboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * v_PosTex.size(), v_PosTex.data(), GL_STATIC_DRAW);

    std::vector<std::array<float, 3>> v_PosTex2;
    
    v_PosTex2.emplace_back(std::array<float, 3>{-1.f, -1.f, 0.f});
    v_PosTex2.emplace_back(std::array<float, 3>{-1.f, 1.f, 0.f});
    v_PosTex2.emplace_back(std::array<float, 3>{1.f, 1.f, 0.f});

    v_PosTex2.emplace_back(std::array<float, 3>{-1.f, -1.f, 0.f});
    v_PosTex2.emplace_back(std::array<float, 3>{ 1.f, 1.f, 0.f});
    v_PosTex2.emplace_back(std::array<float, 3>{1.f, -1.f, 0.f});

    glGenBuffers(1, &m_AlphaClearVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_AlphaClearVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * v_PosTex2.size(), v_PosTex2.data(), GL_STATIC_DRAW);

    float sizeX = 1.f / m_WindowSizeX;
    float sizeY = 1.f / m_WindowSizeY;

    float drawTextureRect[] =
    {
        -sizeX, sizeY, 0.f, 0.f, 0.f,   //x, y, z, tx, ty
        -sizeX, -sizeY, 0.f, 0.f, 1.f,
        sizeX, sizeY, 0.f, 1.f, 0.f,
        sizeX, sizeY, 0.f, 1.f, 0.f,
        -sizeX, -sizeY, 0.f, 0.f, 1.f,
        sizeX, -sizeY, 0.f, 1.f, 1.f,
    };

    glGenBuffers(1, &m_DrawTextureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_DrawTextureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(drawTextureRect), drawTextureRect, GL_STATIC_DRAW);
}

void Renderer::CreateHorizontalVBO(int iHorizontalVertexCount)
{
    m_HorizontalLineVertexCount = iHorizontalVertexCount;
    std::vector<float> verticesLine;
    float gap = 2.f / ((float)m_HorizontalLineVertexCount);

    for (int i = 0; i < m_HorizontalLineVertexCount; ++i)
    {
        verticesLine.emplace_back((float)i * gap - 1.0f);
        verticesLine.emplace_back(0.f);
        verticesLine.emplace_back(0.f);
    }

    glGenBuffers(1, &m_HorizontalLineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_HorizontalLineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesLine.size(), verticesLine.data(), GL_STATIC_DRAW);
}

void Renderer::CreateVertexFlagVBO()
{
    float basePosX = -0.5f;
    float basePosY = -0.5f;
    float targetPosX = 0.5f;
    float targetPosY = 0.5f;

    int pointCountX = 64;
    int pointCountY = 64;

    gDummyVertexCount = (pointCountX - 1) * (pointCountY - 1) * 2 * 3;

    float width = targetPosX - basePosX;
    float height = targetPosY - basePosY;

    std::vector<float> point;
    point.resize(pointCountX * pointCountY * 2);

    std::vector<float> vertices;

    std::vector<float> morphMuls;

    //Prepare points
    for (int x = 0; x < pointCountX; x++)
    {
        for (int y = 0; y < pointCountY; y++)
        {
            point[(y * pointCountX + x) * 2 + 0] = basePosX + width * (x / (float)(pointCountX - 1));
            point[(y * pointCountX + x) * 2 + 1] = basePosY + height * (y / (float)(pointCountY - 1));
        }
    }

    //Make triangles
    int vertIndex = 0;
    for (int x = 0; x < pointCountX - 1; x++)
    {
        for (int y = 0; y < pointCountY - 1; y++)
        {
            vertices.push_back(point[(y * pointCountX + x) * 2 + 0]);
            vertices.push_back(point[(y * pointCountX + x) * 2 + 1]);
            vertices.push_back(0.f);

            morphMuls.push_back(rand() % 100 * 0.01);

            vertices.push_back(point[((y + 1) * pointCountX + (x + 1)) * 2 + 0]);
            vertices.push_back(point[((y + 1) * pointCountX + (x + 1)) * 2 + 1]);
            vertices.push_back(0.f);

            morphMuls.push_back(rand() % 100 * 0.01);

            vertices.push_back(point[((y + 1) * pointCountX + x) * 2 + 0]);
            vertices.push_back(point[((y + 1) * pointCountX + x) * 2 + 1]);
            vertices.push_back(0.f);

            morphMuls.push_back(rand() % 100 * 0.01);

            vertices.push_back(point[(y * pointCountX + x) * 2 + 0]);
            vertices.push_back(point[(y * pointCountX + x) * 2 + 1]);
            vertices.push_back(0.f);

            morphMuls.push_back(rand() % 100 * 0.01);

            vertices.push_back(point[(y * pointCountX + (x + 1)) * 2 + 0]);
            vertices.push_back(point[(y * pointCountX + (x + 1)) * 2 + 1]);
            vertices.push_back(0.f);

            morphMuls.push_back(rand() % 100 * 0.01);

            vertices.push_back(point[((y + 1) * pointCountX + (x + 1)) * 2 + 0]);
            vertices.push_back(point[((y + 1) * pointCountX + (x + 1)) * 2 + 1]);
            vertices.push_back(0.f);

            morphMuls.push_back(rand() % 100 * 0.01);
        }
    }

    glGenBuffers(1, &m_VertexFlagVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexFlagVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
}

void Renderer::CreateTextures()
{
    static const GLulong checkerboard[] =
    {
    0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF
    };

    glGenTextures(1, &m_CheckerBoardTexture);
    glBindTexture(GL_TEXTURE_2D, m_CheckerBoardTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerboard);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

GLuint Renderer::CreatePngTexture(char* filePath, GLuint samplingMethod)
{
    //Load Png
    std::vector<unsigned char> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, filePath);
    if (error != 0)
    {
        std::cout << "PNG image loading failed:" << filePath << std::endl;
        assert(0);
    }

    GLuint temp;
    glGenTextures(1, &temp);
    glBindTexture(GL_TEXTURE_2D, temp);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingMethod);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingMethod);

    return temp;
}

void Renderer::CreateFBOs()
{
    glGenTextures(1, &m_AFBOTexture);
    glBindTexture(GL_TEXTURE_2D, m_AFBOTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenTextures(1, &m_AFBOAttach_1_Texture);
    glBindTexture(GL_TEXTURE_2D, m_AFBOAttach_1_Texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenTextures(1, &m_AFBOAttach_2_Texture);
    glBindTexture(GL_TEXTURE_2D, m_AFBOAttach_2_Texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenTextures(1, &m_AFBOAttach_3_Texture);
    glBindTexture(GL_TEXTURE_2D, m_AFBOAttach_3_Texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenTextures(1, &m_AFBOAttach_4_Texture);
    glBindTexture(GL_TEXTURE_2D, m_AFBOAttach_4_Texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenTextures(1, &m_BFBOTexture);
    glBindTexture(GL_TEXTURE_2D, m_BFBOTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenTextures(1, &m_CFBOTexture);
    glBindTexture(GL_TEXTURE_2D, m_CFBOTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenTextures(1, &m_DFBOTexture);
    glBindTexture(GL_TEXTURE_2D, m_DFBOTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenRenderbuffers(1, &m_DepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &m_A_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_A_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AFBOTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_AFBOAttach_1_Texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_AFBOAttach_2_Texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_AFBOAttach_3_Texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_AFBOAttach_4_Texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "fbo creation failed\n" << std::endl;
    }

    glGenFramebuffers(1, &m_B_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_B_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BFBOTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "fbo creation failed\n" << std::endl;
    }

    glGenFramebuffers(1, &m_C_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_C_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_CFBOTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "fbo creation failed\n" << std::endl;
    }

    glGenFramebuffers(1, &m_D_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_D_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_DFBOTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "fbo creation failed\n" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawTexture(float x, float y, float scaleX, float scaleY, GLuint texID)
{
    GLuint shader = m_DrawTextureShader;
    glUseProgram(shader);

    GLuint posLoc = glGetAttribLocation(shader, "a_Position");
    glEnableVertexAttribArray(posLoc);

    GLuint texLoc = glGetAttribLocation(shader, "a_TexPos");
    glEnableVertexAttribArray(texLoc);

    glBindBuffer(GL_ARRAY_BUFFER, m_DrawTextureVBO);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

    GLuint samplerULoc = glGetUniformLocation(shader, "u_TexSampler");
    glUniform1i(samplerULoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);


    GLuint posScaleLoc = glGetUniformLocation(shader, "u_PosScale");
    glUniform4f(posScaleLoc, x, y, scaleX, scaleY);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawResult()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    DrawTexture(-0.75, -0.75, 128, 128, m_AFBOAttach_1_Texture);
    DrawTexture(-0.25, -0.75, 128, 128, m_AFBOAttach_2_Texture);
    DrawTexture(0.25, -0.75, 128, 128, m_AFBOAttach_3_Texture);
    DrawTexture(0.75, -0.75, 128, 128, m_AFBOAttach_4_Texture);
    DrawTexture(-0.75, -0.25, 128, 128, m_BFBOTexture);
    DrawTexture(-0.25, -0.25, 128, 128, m_CFBOTexture);
    DrawTexture(0.25, -0.25, 128, 128, m_DFBOTexture);
}

void Renderer::DrawAlphaClear()
{
    GLuint program = m_AlphaClearShader;
    glUseProgram(program);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int PosLoc = glGetAttribLocation(program, "a_Position");
    glEnableVertexAttribArray(PosLoc);

    glBindBuffer(GL_ARRAY_BUFFER, m_AlphaClearVBO);
    glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisable(GL_BLEND);
}

void Renderer::DrawHorizontalLine()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_C_FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    GLenum drawBuffers = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, &drawBuffers);

    GLuint program = m_VertexSandBoxShader;
    glUseProgram(program);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int PosLoc = glGetAttribLocation(program, "a_Position");
    glEnableVertexAttribArray(PosLoc);

    glBindBuffer(GL_ARRAY_BUFFER, m_HorizontalLineVBO);
    glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    int timeLoc = glGetUniformLocation(program, "u_Time");
    glUniform1f(timeLoc, g_time);

    //g_time += 0.016f;

    for (int i = 0; i < 10; i++)
    {
        glUniform1f(timeLoc, g_time + (float)i * 0.2f);
        glDrawArrays(GL_LINE_STRIP, 0, m_HorizontalLineVertexCount);
    }

    glDisable(GL_BLEND);
}

void Renderer::DrawTextureSandBox()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_D_FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    GLenum drawBuffers = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, &drawBuffers);

    GLuint program = m_TextureSandboxShader;
    glUseProgram(program);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int PosLoc = glGetAttribLocation(program, "a_Position");
    glEnableVertexAttribArray(PosLoc);

    glBindBuffer(GL_ARRAY_BUFFER, m_TextureSandboxVBO);
    glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);

    int TexLoc = glGetAttribLocation(program, "a_TexPos");
    glEnableVertexAttribArray(TexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, m_TextureSandboxVBO);
    glVertexAttribPointer(TexLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_0Texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_1Texture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_2Texture);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_3Texture);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_4Texture);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_5Texture);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, m_6Texture);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, m_ExplosionTexture);

    int texID[] = { 0, 1 };
    GLuint multisamplerULoc = glGetUniformLocation(program, "uMultiTexSampler");
    glUniform1iv(multisamplerULoc, 2, texID);

    GLuint samplerULoc = glGetUniformLocation(program, "uTexSampler");
    //glUniform1i(samplerULoc, (int)(g_time) % 6);
    glUniform1i(samplerULoc, 7);

    GLuint stepLoc = glGetUniformLocation(program, "u_Step");
    glUniform1f(stepLoc, (int)g_time % 6);

    GLuint seqLoc = glGetUniformLocation(program, "u_SeuqNum");
    glUniform1f(seqLoc, (int)(g_time * 12.5f) % 48);

    /*glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_RGBTexture);*/

    GLuint repeatULoc = glGetUniformLocation(program, "u_XYRepeat");
    glUniform2f(repeatULoc, 2.0f, 2.0f);
    //g_time += 0.08f;

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisable(GL_BLEND);
}

void Renderer::DrawVertexFlag()
{
    glUseProgram(m_VertexFlagShader);
    int attrribPosition = glGetAttribLocation(m_VertexFlagShader, "a_Position");

    glEnableVertexAttribArray(attrribPosition);

    glBindBuffer(GL_ARRAY_BUFFER, m_VertexFlagVBO);
    glVertexAttribPointer(attrribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint texULoc = glGetUniformLocation(m_VertexFlagShader, "u_Texture");
    glUniform1i(texULoc, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_FlagTexture);

    GLuint morphULoc = glGetUniformLocation(m_VertexFlagShader, "u_MorphPos");
    glUniform2f(morphULoc, 1.0f, -1.0f);

    GLuint timeULoc = glGetUniformLocation(m_VertexFlagShader, "u_Time");
    glUniform1f(timeULoc, g_time);
    g_time += 0.016f;

    glDrawArrays(GL_TRIANGLES, 0, gDummyVertexCount);

    DrawTexture(-0.75f, -0.75f, 128.f, 128.f, m_FlagTexture);
}

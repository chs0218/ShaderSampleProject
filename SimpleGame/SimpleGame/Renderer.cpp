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
	CreateParticleVBO(1000);

	if (m_ParticleShader > 0 && m_SolidRectShader > 0 && m_VBORect > 0)
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
	//���̴� ������Ʈ ����
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//���̴� �ڵ带 ���̴� ������Ʈ�� �Ҵ�
	glShaderSource(ShaderObj, 1, p, Lengths);

	//�Ҵ�� ���̴� �ڵ带 ������
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj �� ���������� ������ �Ǿ����� Ȯ��
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL �� shader log �����͸� ������
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram �� attach!!
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
	GLuint ShaderProgram = glCreateProgram(); //�� ���̴� ���α׷� ����

	if (ShaderProgram == 0) { //���̴� ���α׷��� ����������� Ȯ��
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs �� vs ������ �ε���
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs �� fs ������ �ε���
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram �� vs.c_str() ���ؽ� ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram �� fs.c_str() �����׸�Ʈ ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach �Ϸ�� shaderProgram �� ��ŷ��
	glLinkProgram(ShaderProgram);

	//��ũ�� �����ߴ��� Ȯ��
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program �α׸� �޾ƿ�
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

void Renderer::Class0310_Rendering()
{
	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), 0, 0, 0, 1);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), 1, 1, 1, 1);

	int attribLocation_Position = -1;
	attribLocation_Position = glGetAttribLocation(m_SolidRectShader, "a_Position");		// a_Position�̶�� ����ڰ� ������ �Է��� ��ȣ�� ������

	glEnableVertexAttribArray(attribLocation_Position);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
	glVertexAttribPointer(attribLocation_Position, 3, GL_FLOAT, GL_FALSE, 0, 0);		// Draw�� �����͸� �о ������ ũ�� �� ������ ����
	
	int attribLocation_Position1 = -1;
	attribLocation_Position1 = glGetAttribLocation(m_SolidRectShader, "a_Position1");		// a_Position�̶�� ����ڰ� ������ �Է��� ��ȣ�� ������

	glEnableVertexAttribArray(attribLocation_Position1);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO1);
	glVertexAttribPointer(attribLocation_Position1, 3, GL_FLOAT, GL_FALSE, 0, 0);		// Draw�� �����͸� �о ������ ũ�� �� ������ ����
	
	
	float fScale = abs(sinf(0.05f * fTimeElapsed));
	int uniformScale = glGetUniformLocation(m_SolidRectShader, "u_Scale");			// uniform ���� u_Scale�� ��ȣ�� �����´�.
	glUniform1f(uniformScale, fScale);												// uniform ���� u_Scale�� ���� �����Ѵ�.

	fTimeElapsed += 0.016;

	glDrawArrays(GL_TRIANGLES, 0, 3);		// ��� Primitive�� �����ϸ� Vertex ��� �׸� ������ ����, �� �Լ� ȣ�� ��� GPU�� ������
}

void Renderer::DrawParticle()
{
	GLuint program = m_ParticleShader;
	glUseProgram(program);

	int posLoc = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(posLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int velLoc = glGetAttribLocation(program, "a_Velocity");
	glEnableVertexAttribArray(velLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVelVBO);
	glVertexAttribPointer(velLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int emitTimeLoc = glGetAttribLocation(program, "a_EmitTime");
	glEnableVertexAttribArray(emitTimeLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glVertexAttribPointer(emitTimeLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);

	int lifeTimeLoc = glGetAttribLocation(program, "a_LifeTime");
	glEnableVertexAttribArray(lifeTimeLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glVertexAttribPointer(lifeTimeLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);

	float f3Transform = fTimeElapsed;
	int timeLoc = glGetUniformLocation(m_ParticleShader, "u_Time");			
	glUniform1f(timeLoc, f3Transform);

	int accelLoc = glGetUniformLocation(m_ParticleShader, "u_Accel");
	glUniform3f(accelLoc, 0.f, -2.8f, 0.f);

	fTimeElapsed += 0.003;

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
	float vertices1[] = { -1, -1, 0, 0, -1, 0, 0, 0, 0 };
	float colors[] = { 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1 };

	glGenBuffers(1, &m_testVBO);		//Buffer Object�� �����ϰ� Object ID�� testVBO�� ����, testVBO�� CPU->GPU�� �ø� �� ��� 
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);		// ������ Buffer Object�� ���¿� �뵵�� ��üȭ ���ִ� Bind��� �۾��� ����
												// ������ ������ array�� �Ѱ��� ���̱� ������ GL_ARRAY_BUFFER�� ���
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);		// Bind�� Buffer Object�� �����͸� �Ҵ�
																					// GL_STATIC_DRAW�� ���ؽ� ���۰� �����ǰ� ������� �ʴ� ��쿡 ���
																					// �ϵ����� ȿ�������� �����͸� ó���� �� ����
	glGenBuffers(1, &m_testVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
						
	glGenBuffers(1, &m_ColorVBO); 
	glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	
	/*int size = 400000000000000;
	float* testTemp = new float[size];
	memset(testTemp, 1, sizeof(float) * size);

	GLuint testVBO1 = 0;
	glGenBuffers(1, &testVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, testVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, testTemp, GL_STATIC_DRAW);*/
}

void Renderer::CreateParticleVBO(int numParticleCount)
{
	int vertexCount = 6;
	int particleCount = numParticleCount;

	m_ParticleVertexCount = particleCount * vertexCount;

	std::vector<std::array<float, 3>> vertices;
	std::vector<std::array<float, 3>> vel_Vertices;
	std::vector<float> emit_Time;
	std::vector<float> life_Time;
	
	float particleSize = 0.01f;

	for (int i = 0; i < numParticleCount; ++i)
	{
		float particleCenterX = 0.f/*2.0f * (((float)rand() / (float) RAND_MAX) - 0.5f)*/;
		float particleCenterY = 0.f/*2.0f * (((float)rand() / (float)RAND_MAX) - 0.5f)*/;

		vertices.emplace_back(std::array<float, 3>{particleCenterX - particleSize, particleCenterY + particleSize, 0.0f});
		vertices.emplace_back(std::array<float, 3>{particleCenterX - particleSize, particleCenterY - particleSize, 0.0f});
		vertices.emplace_back(std::array<float, 3>{particleCenterX + particleSize, particleCenterY + particleSize, 0.0f});
		vertices.emplace_back(std::array<float, 3>{particleCenterX - particleSize, particleCenterY - particleSize, 0.0f});
		vertices.emplace_back(std::array<float, 3>{particleCenterX + particleSize, particleCenterY - particleSize, 0.0f});
		vertices.emplace_back(std::array<float, 3>{particleCenterX + particleSize, particleCenterY + particleSize, 0.0f});
	}
	
	for (int i = 0; i < numParticleCount; ++i)
	{
		float velX = 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
		float velY = 5.f * (((float)rand() / (float)RAND_MAX));

		vel_Vertices.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
		vel_Vertices.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
		vel_Vertices.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
		vel_Vertices.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
		vel_Vertices.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
		vel_Vertices.emplace_back(std::array<float, 3>{velX, velY, 0.0f});
	}

	for (int i = 0; i < numParticleCount; ++i)
	{
		float randTime = ((float)rand() / (float)RAND_MAX) * 10.0f;

		emit_Time.emplace_back(randTime);
		emit_Time.emplace_back(randTime);
		emit_Time.emplace_back(randTime);
		emit_Time.emplace_back(randTime);
		emit_Time.emplace_back(randTime);
		emit_Time.emplace_back(randTime);
	}

	for (int i = 0; i < numParticleCount; ++i)
	{
		float randTime = ((float)rand() / (float)RAND_MAX) * 1.0f;

		life_Time.emplace_back(randTime);
		life_Time.emplace_back(randTime);
		life_Time.emplace_back(randTime);
		life_Time.emplace_back(randTime);
		life_Time.emplace_back(randTime);
		life_Time.emplace_back(randTime);
	}

	glGenBuffers(1, &m_ParticleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ParticleVelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vel_Vertices.size(), vel_Vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ParticleEmitTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * emit_Time.size(), emit_Time.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ParticleLifeTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * life_Time.size(), life_Time.data(), GL_STATIC_DRAW);
}

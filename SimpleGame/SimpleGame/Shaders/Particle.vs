#version 330

in vec3 a_Position;	
in vec3 a_Velocity;	
in float a_EmitTime;
in float a_LifeTime;
in float a_Amp;
in float a_Period;
in float a_Circle;
in vec4 a_Color;

uniform float u_Time;
uniform vec3 u_Accel;

const vec3 c_Vel = vec3(0.1, 0.1, 0.0);
const float c_resetTime = 0.5f;
const float PI = 3.141592;
const float c_Amp = 0.5f;
const float c_Period = 2.0f;

out vec4 v_Color;

vec4 GraphSin()
{
	vec4 newPos = vec4(0, 0, 0, 1);
	float t = u_Time - a_EmitTime;
	if(t < 0.0f)
	{
	}
	else
	{
		float newT = fract(t / a_LifeTime) * a_LifeTime;

		newPos.x = a_Position.x + c_Vel.x * newT;
		newPos.y = a_Position.y + c_Vel.y * newT;

		vec2 nVel = vec2(-c_Vel.y, c_Vel.x);

		nVel = normalize(nVel);

		newPos.xy = newPos.xy + 
					nVel * newT * a_Amp * sin((1.0 + newT) * a_Period * newT * 2.0 * PI);

		newPos.z = a_Position.z + c_Vel.z * newT;
		newPos.w = 1.f;
	}
	return newPos;
}

vec4 P1()
{
	vec4 newPosition = vec4(0, 0, 0, 1);

	float fTime = u_Time - a_EmitTime;

	if(fTime < 0.0f)
	{
	}
	else
	{
		float newfTime = fract(fTime / a_LifeTime) * a_LifeTime;
		newPosition.xyz = a_Position 
							+ a_Velocity * newfTime
							+ 0.5f * u_Accel * newfTime * newfTime;
	}

	newPosition.w= 1;

	return newPosition;
}

vec4 P2()
{
	vec4 newPos = vec4(0, 0, 0, 1);
	float t = u_Time - a_EmitTime;
	if(t < 0.0f)
	{
	}
	else
	{
		float newT = fract(t / a_LifeTime) * a_LifeTime;
		float paraX = sin(a_Circle * 2 * PI);
		float paraY = cos(a_Circle * 2 * PI);

		newPos.x = a_Position.x + paraX + c_Vel.x * newT;
		newPos.y = a_Position.y + paraY + c_Vel.y * newT;

		vec2 nVel = vec2(-c_Vel.y, c_Vel.x);

		nVel = normalize(nVel);

		newPos.xy = newPos.xy + 
					nVel * newT * a_Amp * sin((1.0 + newT) * a_Period * newT * 2.0 * PI);

		newPos.z = a_Position.z + c_Vel.z * newT;
		newPos.w = 1.f;
	}
	return newPos;
}

void main()
{
	//gl_Position = GraphSin();
	//gl_Position = P1();
	gl_Position = P2();

	v_Color = a_Color;
}

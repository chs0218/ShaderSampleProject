#version 330

in vec3 a_Position;	
in vec3 a_Velocity;	
in float a_EmitTime;
in float a_LifeTime;
in vec4 a_Color;

uniform float u_Time;
uniform vec3 u_Accel;

const vec3 c_Vel = vec3(1.0, 0.0, 0.0);
const float c_resetTime = 0.5f;
const float PI = 3.141592;

out vec4 v_Color;

void main()
{
	vec4 newPos = vec4(0, 0, 0, 1);
	float t = fract(u_Time);
	newPos.x = a_Position.x + c_Vel.x * t;
	newPos.y = a_Position.y + c_Vel.y * t + sin(t * 2.0 * PI);
	newPos.z = a_Position.z + c_Vel.z * t;
	newPos.w = 1.f;
	gl_Position = newPos;

	/*vec4 newPosition = vec4(0, 0, 0, 1);

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
	gl_Position = newPosition;
	v_Color = a_Color;*/
}

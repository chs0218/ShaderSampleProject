#version 330

in vec3 a_Position;	
in vec3 a_Velocity;	
in vec4 a_Color;

uniform float u_Time;
uniform vec3 u_Accel;

out vec4 v_Color;

void main()
{
	vec4 newPosition;
	newPosition.xyz = a_Position 
						+ a_Velocity * u_Time
						+ 0.5f * u_Accel * u_Time;
	newPosition.w= 1;
	gl_Position = newPosition;
	v_Color = a_Color;
}

#version 330

in vec3 a_Position;
in vec3 a_Position1;
//in vec2 a_Texcoord;
uniform vec4 u_Trans;

uniform float u_Scale;


void main()
{
	vec4 newPosition;
	newPosition.xy = u_Scale * (a_Position.xy + a_Position1.xy)
						* u_Trans.w + u_Trans.xy;
	newPosition.z = 0;
	newPosition.w= 1;
	gl_Position = newPosition;
}

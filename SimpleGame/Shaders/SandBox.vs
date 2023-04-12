#version 330

in vec3 a_Position;
in vec2 a_Texcoord;

out vec2 v_Texcoord;

void main()
{
	gl_Position = vec4(a_Position, 1.0f);
	float tX = (a_Position.x + 1.0f) / 2.0f;
	float tY = (-a_Position.y + 1.0f) / 2.0f;
	v_Texcoord = vec2(tX, tY);
}

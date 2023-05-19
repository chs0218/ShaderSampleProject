#version 330

in vec3 a_Position;
const float PI = 3.141592;

void main()
{
    float x = a_Position.x;
    float value = (a_Position.x + 0.5) * 2.0f * PI;
    float y = a_Position.y + 0.5f * sin(value);
    vec4 newPosition = vec4(x, y, 0.0f, 1.0f);
	gl_Position = newPosition;
}

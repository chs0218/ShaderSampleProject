#version 330

in vec3 a_Position;

out float v_alpha;
uniform float u_Time;

const float c_PI = 3.141592;

void sinLine()
{
    vec3 newPosition = a_Position;
    float value = (a_Position.x + 1.0f) * 1 * c_PI - u_Time * 2;
    float d = a_Position.x + 1.0f;
    float newAlpha =  1.0f - d / 2.0f;
    float weight =  d * 0.5f;

    newPosition.y = weight * sin(value);
	gl_Position = vec4(newPosition, 1.0f);

    v_alpha = newAlpha;
}

void main()
{
    sinLine();
}

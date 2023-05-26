#version 330

in vec3 a_Position;
const float PI = 3.141592;

out float v_greyScale;
out vec2 v_TexPos;

uniform float u_Time;
uniform vec2 u_MorphPos;

void main()
{
    float x = a_Position.x;
    float temp = (x + 0.5f);
 
    float value = (a_Position.x + 0.5) * 2.0f * PI - u_Time;
    float y = a_Position.y + 0.25f * temp * sin(value);

    float timeY = length(a_Position.xy - u_MorphPos.xy);
    //(a_Position.y + 0.5f);
    float newTime = u_Time - timeY * 3.0f;
    newTime = max(0, newTime);

    vec2 morphPos = mix(vec2(x, y), u_MorphPos, newTime);
    vec4 newPosition = vec4(morphPos, 0.0f, 1.0f);

    v_greyScale = (1.0f + sin(value)) / 2.0f;
    v_TexPos.x = (a_Position.x + 0.5f);
    v_TexPos.y = (0.5f - a_Position.y);


	gl_Position = newPosition;
}

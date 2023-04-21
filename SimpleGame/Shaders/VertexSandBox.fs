#version 330

layout(location=0) out vec4 FragColor;

in float v_alpha;

uniform float u_Time;

void main()
{
    float newV = 1 - v_alpha;
    newV *= 50.0f;
    newV -= u_Time * 2;
    //int lineMask = int(newV)%2;
    float lineMask = sin(newV);
    FragColor = vec4(1.0f, 1.0f, 1.0f, v_alpha * lineMask);
}

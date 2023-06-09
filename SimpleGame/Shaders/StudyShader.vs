#version 330

in vec3 a_Position;
in vec2 a_UV;

out vec2 v_UV;

uniform sampler2D u_Texture;

void main()
{
    v_UV = a_UV;
    gl_Position = vec4(a_Position, 1.0f);
}

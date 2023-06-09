#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_TexPos;

uniform sampler2D u_Texture;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 tex_offset = 1.0f / textureSize(u_Texture, 0);
    vec3 result = texture(u_Texture, v_TexPos).rgb * weight[0];

    for(int i= 1; i < 5; ++i)
    {
        result += texture(u_Texture, v_TexPos + vec2(0.0f, tex_offset.y * i)).rgb * weight[i];
        result += texture(u_Texture, v_TexPos - vec2(0.0f, tex_offset.y * i)).rgb * weight[i];   
    }

    FragColor = vec4(result, 1.0f);
}
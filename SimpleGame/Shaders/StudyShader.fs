#version 330

layout(location=0) out vec4 FragColor;

uniform sampler2D u_Texture;
uniform float u_Time;

in vec2 v_UV;

const float c_PI = 3.141592;

void RenderTexture()
{
    FragColor = texture(u_Texture, v_UV);
}

void sinLine()
{
    float x = v_UV.x;
    float y = v_UV.y;
    float padding = 0.01f;
    
    float fSinValue = sin(2.0f * (x + u_Time * 2.0f) * c_PI) * x;
    float renewalY = 1.0f - y * 2.0f;

    if(renewalY < fSinValue + padding  && renewalY > fSinValue - padding)
        FragColor = vec4(1.0f);
    else
        FragColor = vec4(0.0f);
}

void main()
{
    FragColor = texture(u_Texture, v_UV);
    //FragColor = vec4(v_UV, 0.f, 1.f);
}
